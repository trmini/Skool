/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

/*
 * Standard FreeBSD MD5 implementation.
 */

#ifndef _JOHN_MD5_STD_H
#define _JOHN_MD5_STD_H

#include "arch.h"
#include "common.h"

typedef ARCH_WORD_32 MD5_word;

/*
 * Binary ciphertext type.
 */
typedef MD5_word MD5_binary[4];

/*
 * MD5_std_crypt() output buffer.
 */
extern MD5_binary MD5_out;

#define MD5_ALGORITHM_NAME		"32/" ARCH_BITS_STR

/*
 * Sets a salt for MD5_std_crypt().
 */
extern void MD5_std_set_salt(char *salt);

/*
 * Sets a key for MD5_std_crypt().
 * Currently only supports keys up to 15 characters long.
 */
extern void MD5_std_set_key(char *key);

/*
 * Main encryption routine, sets MD5_out.
 */
extern void MD5_std_crypt();

/*
 * Returns the salt for MD5_std_set_salt().
 */
extern char *MD5_std_get_salt(char *ciphertext);

/*
 * Converts an ASCII ciphertext to binary.
 */
extern MD5_word *MD5_std_get_binary(char *ciphertext);

#endif
