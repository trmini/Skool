/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

#include <stdio.h>
#include <string.h>

#include "misc.h"
#include "params.h"
#include "path.h"
#include "signals.h"
#include "compiler.h"
#include "loader.h"
#include "status.h"
#include "recovery.h"
#include "config.h"
#include "cracker.h"

static char int_word[PLAINTEXT_BUFFER_SIZE];
static char rec_word[PLAINTEXT_BUFFER_SIZE];

static char *ext_mode = NULL;

static c_int ext_word[PLAINTEXT_BUFFER_SIZE];

static struct c_ident ext_globals = {
	NULL,
	"word",
	ext_word
};

static struct c_ident *f_generate, *f_filter;

static struct cfg_list *ext_source;
static struct cfg_line *ext_line;
static int ext_pos;

static int ext_getchar()
{
	char c;

	if (!ext_line || !ext_line->data) return -1;

	if ((c = ext_line->data[ext_pos++])) return c;

	ext_line = ext_line->next;
	ext_pos = 0;
	return '\n';
}

static void ext_rewind()
{
	ext_line = ext_source->head;
	ext_pos = 0;
}

void ext_init(char *mode)
{
	if (!(ext_source = cfg_get_list(SECTION_EXT, mode))) {
		fprintf(stderr, "Unknown external mode: %s\n", mode);
		error();
	}

	if (c_compile(ext_getchar, ext_rewind, &ext_globals)) {
		if (!ext_line) ext_line = ext_source->tail;

		fprintf(stderr, "Compiler error in %s at line %d: %s\n",
			path_expand(CFG_NAME), ext_line->number,
			c_errors[c_errno]);
		error();
	}

	ext_word[0] = 0;
	c_execute(c_lookup("init"));

	f_generate = c_lookup("generate");
	f_filter = c_lookup("filter");

	ext_mode = mode;
}

int ext_filter(char *word)
{
	unsigned char *internal;
	c_int *external;

	if (!ext_mode || !f_filter) return 1;

	internal = (unsigned char *)word;
	external = ext_word;
	while (*internal)
		*external++ = *internal++;
	*external = 0;

	c_execute(f_filter);

	if (word[0] && !ext_word[0]) return 0;

	internal = (unsigned char *)word;
	external = ext_word;
	while (*external)
		*internal++ = *external++;
	*internal = 0;

	return 1;
}

static void save_state(FILE *file)
{
	unsigned char *ptr;

	ptr = (unsigned char *)rec_word;
	do {
		fprintf(file, "%d\n", (int)*ptr);
	} while (*ptr++);
}

static int restore_state(FILE *file)
{
	int c;
	unsigned char *internal;
	c_int *external;
	int count;

	internal = (unsigned char *)int_word;
	external = ext_word;
	count = 0;
	do {
		if (fscanf(file, "%d\n", &c) != 1) return 1;
		if (++count >= PLAINTEXT_BUFFER_SIZE) return 1;
	} while ((*internal++ = *external++ = c));

	c_execute(c_lookup("restore"));

	return 0;
}

static void fix_state()
{
	strcpy(rec_word, int_word);
}

void do_external_crack(struct db_main *db)
{
	unsigned char *internal;
	c_int *external;

	if (!f_generate) {
		fprintf(stderr, "No generate() for external mode: %s\n",
			ext_mode);
		error();
	}

	internal = (unsigned char *)int_word;
	external = ext_word;
	while (*external)
		*internal++ = *external++;
	*internal = 0;

	status_init(NULL, 1);

	rec_restore_mode(restore_state);
	rec_init(db, save_state);

	crk_init(db, fix_state, NULL);

	do {
		c_execute(f_generate);
		if (!ext_word[0]) break;

		c_execute(f_filter);
		if (!ext_word[0]) continue;

		internal = (unsigned char *)int_word;
		external = ext_word;
		while (*external)
			*internal++ = *external++;
		*internal = 0;

		if (crk_process_key(int_word)) break;
	} while (1);

	crk_done();
	rec_done(event_abort);
}
