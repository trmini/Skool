/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>

#include "times.h"

#include "arch.h"
#include "misc.h"
#include "math.h"
#include "params.h"
#include "cracker.h"
#include "options.h"
#include "status.h"

struct status_main status;
unsigned int status_restored_time = 0;
int (*status_get_progress)() = NULL;

static clock_t get_time()
{
	struct tms buf;

	return times(&buf);
}

void status_init(int (*get_progress)(), int start)
{
	if (start) {
		if (!status_restored_time)
			memset(&status, 0, sizeof(status));
		status.start_time =
			get_time() - status_restored_time * CLK_TCK;
	}

	status_get_progress = get_progress;
}

void status_update_crypts(unsigned int count)
{
	add32to64(&status.crypts, count);
}

unsigned int status_get_time()
{
	return (get_time() - status.start_time) / CLK_TCK;
}

static char *status_get_cps(char *buffer)
{
	unsigned int time, cps_hi, cps_lo;
	int64 tmp;

	if (!(time = status_get_time())) time = 1;

	cps_hi = div64by32lo(&status.crypts, time);

	tmp = status.crypts;
	mul64by32(&tmp, 100);
	cps_lo = div64by32lo(&tmp, time) % 100;

	sprintf(buffer, cps_hi < 100 ? "%u.%02u" : "%u", cps_hi, cps_lo);

	return buffer;
}

static void status_print_stdout(char *percent)
{
	unsigned int time = status_get_time();
	char s_wps[64];
	char s_words[32];
	int64 current, next, rem;
	char *s_words_ptr;

	s_words_ptr = &s_words[sizeof(s_words)];
	*--s_words_ptr = 0;

	current = status.crypts;
	do {
		next = current;
		div64by32(&next, 10);
		rem = next;
		mul64by32(&rem, 10);
		neg64(&rem);
		add64to64(&rem, &current);
		*--s_words_ptr = rem.lo + '0';
		current = next;
	} while (current.lo || current.hi);

	fprintf(stderr,
		"words: %s  "
		"time: %u:%02u:%02u:%02u"
		"%s  "
		"w/s: %s",
		s_words_ptr,
		time / 86400, time % 86400 / 3600, time % 3600 / 60, time % 60,
		percent,
		status_get_cps(s_wps));

	if (options.flags & FLG_STATUS_CHK)
		fputc('\n', stderr);
	else
		fprintf(stderr,
			"  current: %s\n",
			crk_get_key1());
}

static void status_print_cracking(char *percent)
{
	unsigned int time = status_get_time();
	char *key, saved_key[PLAINTEXT_BUFFER_SIZE];
	char s_cps[64];

	if (!(options.flags & FLG_STATUS_CHK)) {
		if ((key = crk_get_key2()))
			strnzcpy(saved_key, key, PLAINTEXT_BUFFER_SIZE);
		else
			saved_key[0] = 0;
	}

	fprintf(stderr,
		"guesses: %u  "
		"time: %u:%02u:%02u:%02u"
		"%s  "
		"c/s: %s",
		status.guess_count,
		time / 86400, time % 86400 / 3600, time % 3600 / 60, time % 60,
		percent,
		status_get_cps(s_cps));

	if (options.flags & FLG_STATUS_CHK)
		fputc('\n', stderr);
	else
		fprintf(stderr,
			"  trying: %s%s%s\n",
			crk_get_key1(),
			saved_key[0] ? " - " : "",
			saved_key ? saved_key : "");
}

void status_print()
{
	int percent_value;
	char s_percent[32];

	percent_value = -1;
	if (options.flags & FLG_STATUS_CHK)
		percent_value = status.progress;
	else
	if (status_get_progress)
		percent_value = status_get_progress();

	s_percent[0] = 0;
	if (percent_value >= 0)
		sprintf(s_percent, status.pass ? " %d%% (%d)" : " %d%%",
			percent_value, status.pass);
	else
	if (status.pass)
		sprintf(s_percent, " (%d)", status.pass);

	if (options.flags & FLG_STDOUT)
		status_print_stdout(s_percent);
	else
		status_print_cracking(s_percent);
}
