/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

#include <stdio.h>
#include <string.h>

#include "misc.h"
#include "params.h"
#include "memory.h"

static char *home_path = NULL;
static size_t home_path_length;

void path_init(char **argv)
{
	char *pos;

	if (!argv[0]) error();

	if (!home_path && (pos = strrchr(argv[0], '/'))) {
		home_path_length = pos - argv[0] + 1;
		if (home_path_length >= PATH_BUFFER_SIZE)
			home_path_length = PATH_BUFFER_SIZE - 1;

		home_path = mem_alloc(PATH_BUFFER_SIZE);

		memcpy(home_path, argv[0], home_path_length);
	}
}

char *path_expand(char *name)
{
	if (!strncmp(name, "~/", 2)) {
		if (home_path) {
			strnzcpy(&home_path[home_path_length], &name[2],
				PATH_BUFFER_SIZE - home_path_length);

			return home_path;
		} else
			return name + 2;
	} else
		return name;
}

void path_done()
{
	mem_free((void **)&home_path);
}
