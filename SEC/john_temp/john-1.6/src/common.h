/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

/*
 * Things common to many ciphertext formats.
 */

#ifndef _JOHN_COMMON_H
#define _JOHN_COMMON_H

#include "arch.h"

#if ARCH_INT_GT_32
typedef unsigned short ARCH_WORD_32;
#else
typedef unsigned int ARCH_WORD_32;
#endif

/*
 * ASCII <-> binary conversion tables.
 */
extern char itoa64[64], atoi64[0x100];
extern char itoa16[16], atoi16[0x100];

/*
 * Initializes the tables.
 */
extern void common_init();

#endif
