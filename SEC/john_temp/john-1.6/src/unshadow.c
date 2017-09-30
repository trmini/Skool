/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

#include <stdio.h>
#include <string.h>

#include "misc.h"
#include "params.h"
#include "memory.h"

struct shadow_entry {
	struct shadow_entry *next;
	char *login, *passwd;
};

static struct shadow_entry **shadow_table;

static int login_hash(char *login)
{
	int hash = 0;

	while (*login) {
		hash <<= 1;
		hash ^= *login++;
	}

	hash ^= hash >> SHADOW_HASH_LOG;
	hash ^= hash >> (2 * SHADOW_HASH_LOG);
	hash &= SHADOW_HASH_SIZE - 1;

	return hash;
}

static void read_file(char *name, void (*process_line)(char *line))
{
	FILE *file;
	char line[LINE_BUFFER_SIZE];

	if (!(file = fopen(name, "r")))
		pexit("fopen: %s", name);

	while (fgetl(line, sizeof(line), file))
		process_line(line);

	if (ferror(file)) pexit("fgets");

	if (fclose(file)) pexit("fclose");
}

static void process_shadow_line(char *line)
{
	char *pos1, *pos2;
	struct shadow_entry **entry, *last;

	if (!(pos1 = strchr(line, ':'))) return;
	*pos1++ = 0;

	if (!(pos2 = strchr(pos1, ':')))
		pos2 = pos1 + strlen(pos1);
	if (pos2 - pos1 < 1) return;
	*pos2 = 0;

	entry = &shadow_table[login_hash(line)];
	last = *entry;
	*entry = mem_alloc_tiny(sizeof(struct shadow_entry), MEM_ALIGN_WORD);
	(*entry)->next = last;
	(*entry)->login = str_alloc_copy(line);
	(*entry)->passwd = str_alloc_copy(pos1);
}

static void process_passwd_line(char *line)
{
	char *pos1, *pos2;
	struct shadow_entry *current;

	if (!(pos1 = strchr(line, ':'))) return;
	*pos1++ = 0;

	if (!(pos2 = strchr(pos1, ':')))
		pos2 = pos1 + strlen(pos1);

	if ((current = shadow_table[login_hash(line)]))
	do {
		if (!strcmp(current->login, line)) {
			printf("%s:%s%s\n", line, current->passwd, pos2);
			return;
		}
	} while ((current = current->next));

	printf("%s:%s\n", line, pos1);
}

int unshadow(int argc, char **argv)
{
	if (argc != 3) {
		printf("Usage: %s PASSWORD-FILE SHADOW-FILE\n",
			argv[0] ? argv[0] : "unshadow");

		if (argc <= 1)
			return 0;
		else
			error();
	}

	shadow_table = (struct shadow_entry **)
		mem_alloc(SHADOW_HASH_SIZE * sizeof(struct shadow_entry *));
	memset(shadow_table, 0, sizeof(shadow_table));

	read_file(argv[2], process_shadow_line);
	read_file(argv[1], process_passwd_line);

	return 0;
}
