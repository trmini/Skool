/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

#include <stdio.h>
#include <string.h>

#include "arch.h"
#include "misc.h"
#include "params.h"
#include "path.h"
#include "memory.h"
#include "signals.h"
#include "formats.h"
#include "loader.h"
#include "status.h"
#include "recovery.h"
#include "config.h"
#include "charset.h"
#include "external.h"
#include "cracker.h"

extern struct fmt_main fmt_LM;

typedef char (*char2_table)
	[CHARSET_SIZE + 1][CHARSET_SIZE + 1];
typedef char (*chars_table)
	[CHARSET_SIZE + 1][CHARSET_SIZE + 1][CHARSET_SIZE + 1];

static int rec_entry;
static int rec_numbers[CHARSET_LENGTH];

static int entry;
static int numbers[CHARSET_LENGTH];

static void save_state(FILE *file)
{
	int pos;

	fprintf(file, "%d\n", rec_entry);
	for (pos = 0; pos < CHARSET_LENGTH; pos++)
		fprintf(file, "%d\n", rec_numbers[pos]);
}

static int restore_state(FILE *file)
{
	int pos;

	if (fscanf(file, "%d\n", &rec_entry) != 1) return 1;
	for (pos = 0; pos < CHARSET_LENGTH; pos++) {
		if (fscanf(file, "%d\n", &rec_numbers[pos]) != 1) return 1;
		if ((unsigned int)rec_numbers[pos] >= CHARSET_SIZE) return 1;
	}

	return 0;
}

static void fix_state()
{
	rec_entry = entry;
	memcpy(rec_numbers, numbers, sizeof(rec_numbers));
}

static void inc_format_error(char *charset)
{
	fprintf(stderr, "Incorrect charset file format: %s\n", charset);
	error();
}

static void inc_new_length(unsigned int length,
	struct charset_header *header, FILE *file, char *charset,
	char *char1, char2_table char2, chars_table *chars)
{
	long offset;
	int value, pos, i, j;
	char *buffer;
	int count;

	char1[0] = 0;
	if (length)
		memset(char2, 0, sizeof(*char2));
	for (pos = 0; pos <= (int)length - 2; pos++)
		memset(chars[pos], 0, sizeof(**chars));

	offset =
		(long)header->offsets[length][0] +
		((long)header->offsets[length][1] << 8) +
		((long)header->offsets[length][2] << 16) +
		((long)header->offsets[length][3] << 24);
	if (fseek(file, offset, SEEK_SET)) pexit("fseek");

	i = j = pos = -1;
	if ((value = getc(file)) != EOF)
	do {
		if (value != CHARSET_ESC) {
			switch (pos) {
			case -1:
				inc_format_error(charset);

			case 0:
				buffer = char1;
				break;

			case 1:
				if (j < 0)
					inc_format_error(charset);
				buffer = (*char2)[j];
				break;

			default:
				if (i < 0 || j < 0)
					inc_format_error(charset);
				buffer = (*chars[pos - 2])[i][j];
			}

			buffer[count = 0] = value;
			while ((value = getc(file)) != EOF) {
				buffer[++count] = value;
				if (value == CHARSET_ESC) break;
				if (count >= CHARSET_SIZE)
					inc_format_error(charset);
			}
			buffer[count] = 0;

			continue;
		}

		if ((value = getc(file)) == EOF) break; else
		if (value == CHARSET_NEW) {
			if ((value = getc(file)) != (int)length) break;
			if ((value = getc(file)) == EOF) break;
			if ((unsigned int)value > length)
				inc_format_error(charset);
			pos = value;
		} else
		if (value == CHARSET_LINE) {
			if (pos < 0)
				inc_format_error(charset);
			if ((value = getc(file)) == EOF) break;
			if ((unsigned int)(i = value) > CHARSET_SIZE)
				inc_format_error(charset);
			if ((value = getc(file)) == EOF) break;
			if ((unsigned int)(j = value) > CHARSET_SIZE)
				inc_format_error(charset);
		} else
			inc_format_error(charset);

		value = getc(file);
	} while (value != EOF);

	if (value == EOF) {
		if (ferror(file)) pexit("getc"); else
			inc_format_error(charset);
	}
}

static void expand(char *dst, char *src, int size)
{
	char *dptr = dst, *sptr = src;
	int count = size;
	char present[CHARSET_SIZE];

	memset(present, 0, sizeof(present));
	while (*dptr) {
		if (--count <= 1) return;
		present[((ARCH_INDEX)*dptr++) - CHARSET_MIN] = 1;
	}

	while (*sptr)
	if (!present[(ARCH_INDEX)*sptr - CHARSET_MIN]) {
		*dptr++ = *sptr++;
		if (--count <= 1) break;
	} else
		sptr++;
	*dptr = 0;
}

static void inc_new_count(unsigned int length, int count,
	char *allchars, char *char1, char2_table char2, chars_table *chars)
{
	int pos, i, j;
	int size;

	size = count + 2;

	expand(char1, allchars, size);
	if (length)
		expand((*char2)[CHARSET_SIZE], allchars, size);
	for (pos = 0; pos <= (int)length - 2; pos++)
		expand((*chars[pos])[CHARSET_SIZE][CHARSET_SIZE],
			allchars, size);

	for (i = 0; i < CHARSET_SIZE; i++) {
		if (length)
			expand((*char2)[i], (*char2)[CHARSET_SIZE], size);

		for (j = 0; j < CHARSET_SIZE; j++)
		for (pos = 0; pos <= (int)length - 2; pos++) {
			expand((*chars[pos])[i][j], (*chars[pos])
				[CHARSET_SIZE][j], size);
			expand((*chars[pos])[i][j], (*chars[pos])
				[CHARSET_SIZE][CHARSET_SIZE], size);
		}
	}
}

static int inc_key_loop(int length, int fixed, int count,
	char *char1, char2_table char2, chars_table *chars)
{
	char key[PLAINTEXT_BUFFER_SIZE];
	int pos;

	key[length + 1] = 0;
	numbers[fixed] = count;

	do {
		key[0] = char1[numbers[0]];
		if (length)
			key[1] = (*char2)[key[0] - CHARSET_MIN][numbers[1]];
		for (pos = 2; pos <= length; pos++)
			key[pos] = (*chars[pos - 2])
				[(ARCH_INDEX)key[pos - 2] - CHARSET_MIN]
				[(ARCH_INDEX)key[pos - 1] - CHARSET_MIN]
				[numbers[pos]];

		if (ext_filter(key))
		if (crk_process_key(key)) return 1;

		if (length) {
			pos = 0;
			do {
				if (pos == fixed) continue;
				if (++numbers[pos] <= count -
					((pos < fixed) ? 1 : 0)) break;
				numbers[pos] = 0;
			} while (++pos <= length);
		} else
			if (++numbers[0] <= count) pos = 0; else break;
	} while (pos <= length);

	return 0;
}

void do_incremental_crack(struct db_main *db, char *mode)
{
	char *charset;
	int min_length, max_length, max_count;
	char *extra;
	FILE *file;
	struct charset_header *header;
	char allchars[CHARSET_SIZE + 1];
	char char1[CHARSET_SIZE + 1];
	char2_table char2;
	chars_table chars[CHARSET_LENGTH - 2];
	unsigned char *ptr;
	unsigned int length, fixed, count;
	unsigned int real_count;
	int last_length, last_count;
	int pos;

	if (!mode) {
		if (db->format == &fmt_LM)
			mode = "LanMan";
		else
			mode = "All";
	}

	if (!(charset = cfg_get_param(SECTION_INC, mode, "File"))) {
		fprintf(stderr, "No charset defined for mode: %s\n", mode);
		error();
	}

	extra = cfg_get_param(SECTION_INC, mode, "Extra");

	if ((min_length = cfg_get_int(SECTION_INC, mode, "MinLen")) < 0)
		min_length = 0;
	if ((max_length = cfg_get_int(SECTION_INC, mode, "MaxLen")) < 0)
		max_length = CHARSET_LENGTH;
	max_count = cfg_get_int(SECTION_INC, mode, "CharCount");

	if (!(file = fopen(path_expand(charset), "rb")))
		pexit("fopen: %s", path_expand(charset));

	header = (struct charset_header *)mem_alloc(sizeof(*header));

	charset_read_header(file, header);
	if (ferror(file)) pexit("fread");

	if (feof(file) ||
		memcmp(header->version, CHARSET_VERSION,
			sizeof(header->version)) ||
		header->min != CHARSET_MIN || header->max != CHARSET_MAX ||
		header->length != CHARSET_LENGTH ||
		header->count > CHARSET_SIZE ||
		!header->count) inc_format_error(charset);

	fread(allchars, header->count, 1, file);
	if (ferror(file)) pexit("fread");
	if (feof(file)) inc_format_error(charset);

	allchars[header->count] = 0;
	if (extra)
		expand(allchars, extra, sizeof(allchars));
	real_count = strlen(allchars);

	if (max_count < 0) max_count = CHARSET_SIZE; else
	if ((unsigned int)max_count > real_count)
		fprintf(stderr,"Warning: only %d characters available\n",
			real_count);

	if (header->length >= 2)
		char2 = (char2_table)mem_alloc(sizeof(*char2));
	else
		char2 = NULL;
	for (pos = 0; pos < (int)header->length - 2; pos++)
		chars[pos] = (chars_table)mem_alloc(sizeof(*chars[0]));

	rec_entry = 0;
	memset(rec_numbers, 0, sizeof(rec_numbers));

	status_init(NULL, !status.pass);

	rec_restore_mode(restore_state);
	rec_init(db, save_state);

	ptr = header->order + (entry = rec_entry) * 3;
	memcpy(numbers, rec_numbers, sizeof(numbers));

	crk_init(db, fix_state, NULL);

	last_count = last_length = -1;

	entry--;
	while (ptr < &header->order[sizeof(header->order) - 1]) {
		entry++;
		length = *ptr++; fixed = *ptr++; count = *ptr++;

		if (length >= CHARSET_LENGTH ||
			fixed > length ||
			count >= CHARSET_SIZE) inc_format_error(charset);

		if (entry != rec_entry)
			memset(numbers, 0, sizeof(numbers));

		if (count >= real_count ||
			(int)length >= db->format->params.plaintext_length ||
			(fixed && !count)) continue;

		if ((int)length + 1 < min_length ||
			(int)length >= max_length ||
			(int)count >= max_count) continue;

		if ((int)length != last_length) {
			inc_new_length(last_length = length,
				header, file, charset, char1, char2, chars);
			last_count = -1;
		}
		if ((int)count > last_count)
			inc_new_count(length, last_count = count,
				allchars, char1, char2, chars);

		if (!length && !min_length) {
			min_length = 1;
			if (crk_process_key("")) break;
		}

		if (inc_key_loop(length, fixed, count, char1, char2, chars))
			break;
	}

	crk_done();
	rec_done(event_abort);

	for (pos = 0; pos < (int)header->length - 2; pos++)
		mem_free((void **)&chars[pos]);
	mem_free((void **)&char2);
	mem_free((void **)&header);

	fclose(file);
}
