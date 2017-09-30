/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

/*
 * External mode support.
 */

#ifndef _JOHN_EXTERNAL_H
#define _JOHN_EXTERNAL_H

#include "loader.h"

/*
 * Initializes an external mode.
 */
extern void ext_init(char *mode);

/*
 * Calls an external word filter. Returns 0 if the word is rejected.
 */
extern int ext_filter(char *word);

/*
 * Runs the external mode cracker.
 */
extern void do_external_crack(struct db_main *db);

#endif
