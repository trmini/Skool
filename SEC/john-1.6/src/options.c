/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "misc.h"
#include "params.h"
#include "memory.h"
#include "list.h"
#include "loader.h"
#include "status.h"
#include "recovery.h"
#include "options.h"

struct options_main options;

static struct opt_entry opt_list[] = {
	{"", FLG_PASSWD, 0, 0, 0, OPT_FMT_ADD_LIST, &options.passwd},
	{"single", FLG_SINGLE_SET, FLG_CRACKING_CHK},
	{"wordfile", FLG_WORDLIST_SET, FLG_CRACKING_CHK,
		0, OPT_REQ_PARAM, OPT_FMT_STR_ALLOC, &options.wordlist},
	{"stdin", FLG_STDIN_SET, FLG_CRACKING_CHK},
	{"rules", FLG_RULES, FLG_RULES, FLG_WORDLIST_CHK, FLG_STDIN_CHK},
	{"incremental", FLG_INC_SET, FLG_CRACKING_CHK,
		0, 0, OPT_FMT_STR_ALLOC, &options.charset},
	{"external", FLG_EXTERNAL_SET, FLG_EXTERNAL_CHK,
		0, OPT_REQ_PARAM, OPT_FMT_STR_ALLOC, &options.external},
	{"stdout", FLG_STDOUT, FLG_STDOUT,
		FLG_CRACKING_SUP, FLG_SINGLE_CHK | FLG_BATCH_CHK,
		"%u", &options.length},
	{"restore", FLG_RESTORE_SET, FLG_RESTORE_CHK,
		0, ~FLG_RESTORE_SET & ~OPT_REQ_PARAM,
		OPT_FMT_STR_ALLOC, &rec_name},
	{"session", FLG_SESSION, FLG_SESSION,
		FLG_CRACKING_SUP, OPT_REQ_PARAM,
		OPT_FMT_STR_ALLOC, &rec_name},
	{"status", FLG_STATUS_SET, FLG_STATUS_CHK,
		0, ~FLG_STATUS_SET & ~OPT_REQ_PARAM,
		OPT_FMT_STR_ALLOC, &rec_name},
	{"makechars", FLG_MAKECHARS_SET, FLG_MAKECHARS_CHK,
		0, FLG_CRACKING_CHK | FLG_SESSION | OPT_REQ_PARAM,
		OPT_FMT_STR_ALLOC, &options.charset},
	{"show", FLG_SHOW_SET, FLG_SHOW_CHK,
		0, FLG_CRACKING_SUP | FLG_MAKECHARS_CHK},
	{"test", FLG_TEST_SET, FLG_TEST_CHK,
		0, ~FLG_TEST_SET & ~FLG_FORMAT & ~FLG_SAVEMEM &
		~OPT_REQ_PARAM},
	{"users", FLG_NONE, 0, FLG_PASSWD, OPT_REQ_PARAM,
		OPT_FMT_ADD_LIST_MULTI, &options.loader.users},
	{"groups", FLG_NONE, 0, FLG_PASSWD, OPT_REQ_PARAM,
		OPT_FMT_ADD_LIST_MULTI, &options.loader.groups},
	{"shells", FLG_NONE, 0, FLG_PASSWD, OPT_REQ_PARAM,
		OPT_FMT_ADD_LIST_MULTI, &options.loader.shells},
	{"salts", FLG_SALTS, FLG_SALTS, FLG_PASSWD, OPT_REQ_PARAM,
		"%d", &options.loader.min_pps},
	{"format", FLG_FORMAT, FLG_FORMAT,
		FLG_CRACKING_SUP,
		FLG_MAKECHARS_CHK | FLG_STDOUT | OPT_REQ_PARAM,
		OPT_FMT_STR_ALLOC, &options.format},
	{"savemem", FLG_SAVEMEM, FLG_SAVEMEM, 0, OPT_REQ_PARAM,
		"%u", &mem_saving_level},
	{NULL}
};

#define JOHN_USAGE \
"\nJohn the Ripper  Version " JOHN_VERSION \
"  Copyright (c) 1996-98 by Solar Designer\n" \
"\n" \
"Usage: %s [OPTIONS] [PASSWORD-FILES]\n" \
"-single                   \"single crack\" mode\n" \
"-wordfile:FILE -stdin     wordlist mode, read words from FILE or stdin\n" \
"-rules                    enable rules for wordlist mode\n" \
"-incremental[:MODE]       incremental mode [using section MODE]\n" \
"-external:MODE            external mode or word filter\n" \
"-stdout[:LENGTH]          no cracking, just write words to stdout\n" \
"-restore[:FILE]           restore an interrupted session [from FILE]\n" \
"-session:FILE             set session file name to FILE\n" \
"-status[:FILE]            print status of a session [from FILE]\n" \
"-makechars:FILE           make a charset, FILE will be overwritten\n" \
"-show                     show cracked passwords\n" \
"-test                     perform a benchmark\n" \
"-users:[-]LOGIN|UID[,..]  load this (these) user(s) only\n" \
"-groups:[-]GID[,..]       load users of this (these) group(s) only\n" \
"-shells:[-]SHELL[,..]     load users with this (these) shell(s) only\n" \
"-salts:[-]COUNT           load salts with at least COUNT passwords only\n" \
"-format:NAME              force ciphertext format NAME " \
	"(DES/BSDI/MD5/BF/AFS/LM)\n" \
"-savemem:LEVEL            enable memory saving, at LEVEL 1..3\n"

void opt_init(int argc, char **argv)
{
	if (!argv[0]) error();

	if (!argv[1]) {
		printf(JOHN_USAGE, argv[0]);
		exit(0);
	}

	memset(&options, 0, sizeof(options));

	list_init(&options.passwd);

	options.loader.flags = DB_LOGIN;
	list_init(&options.loader.users);
	list_init(&options.loader.groups);
	list_init(&options.loader.shells);

	options.length = -1;

	opt_process(opt_list, &options.flags, argv);

	if ((options.flags &
	    (FLG_EXTERNAL_CHK | FLG_CRACKING_CHK | FLG_MAKECHARS_CHK)) ==
	    FLG_EXTERNAL_CHK)
		options.flags |= FLG_CRACKING_SET;

	if (!(options.flags & FLG_ACTION))
		options.flags |= FLG_BATCH_SET;

	opt_check(opt_list, options.flags, argv);

	if (options.flags & FLG_RESTORE_CHK) {
		rec_restore_args(1);
		return;
	}

	if (options.flags & FLG_STATUS_CHK) {
		rec_restore_args(0);
		options.flags |= FLG_STATUS_SET;
		status_init(NULL, 1);
		status_print();
		exit(0);
	}

	if (options.flags & FLG_SALTS)
	if (options.loader.min_pps < 0) {
		options.loader.max_pps = -1 - options.loader.min_pps;
		options.loader.min_pps = 0;
	}

	if (options.length < 0)
		options.length = PLAINTEXT_BUFFER_SIZE - 3;
	else
	if (options.length < 1 || options.length > PLAINTEXT_BUFFER_SIZE - 3) {
		fprintf(stderr, "Invalid plaintext length requested\n");
		error();
	}

	if (options.flags & FLG_STDOUT) options.flags &= ~FLG_PWD_REQ;

	if ((options.flags & (FLG_PASSWD | FLG_PWD_REQ)) == FLG_PWD_REQ) {
		fprintf(stderr, "Password files required, "
			"but none specified\n");
		error();
	}

	if ((options.flags & (FLG_PASSWD | FLG_PWD_SUP)) == FLG_PASSWD) {
		fprintf(stderr, "Password files specified, "
			"but no option would use them\n");
		error();
	}

	rec_argc = argc; rec_argv = argv;
}
