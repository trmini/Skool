/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

/*
 * Bitslice DES implementation.
 */

#ifndef _JOHN_DES_BS_H
#define _JOHN_DES_BS_H

#include "arch.h"

#define DES_BS_ALGORITHM_NAME		ARCH_BITS_STR "/" ARCH_BITS_STR " BS"

/*
 * Non-zero when memory saving is enabled, with a performance impact. This
 * should be either set at the very beginning, or never set at all.
 */
extern int DES_bs_mem_saving;

/*
 * All bitslice DES parameters combined into one struct for more efficient
 * cache usage.
 */
typedef struct {
	ARCH_WORD *Kp[0x300];	/* Key schedule (key bit pointers) */
	ARCH_WORD *E[96];	/* Expansion function (data bit pointers) */
	ARCH_WORD K[56];	/* Keys */
	ARCH_WORD B[64];	/* Data blocks */
} DES_bs_combined;

extern DES_bs_combined DES_bs_all;

/*
 * Initializes the internal structures.
 */
extern void DES_bs_init();

/*
 * Sets a salt for DES_bs_crypt().
 */
extern void DES_bs_set_salt(ARCH_WORD salt);

/*
 * Clears the bitslice keys, should be done before DES_bs_set_key() calls.
 */
extern void DES_bs_clear_keys();

/*
 * Sets a key for DES_bs_crypt().
 */
extern void DES_bs_set_key(char *key, int index);

/*
 * Main encryption routine, accepts the iteration count.
 */
extern void DES_bs_crypt(int count);

/*
 * Converts an ASCII ciphertext to binary to be used with one of the
 * comparison functions.
 */
extern ARCH_WORD *DES_bs_get_binary(char *ciphertext);

/*
 * Calculates a hash for a ciphertext, for faster comparison.
 */
extern int DES_bs_binary_hash(ARCH_WORD *binary, int count);

/*
 * Calculates a hash for a DES_bs_crypt() output.
 */
extern int DES_bs_get_hash(int index, int count);

/*
 * Compares count bits of a given ciphertext against all the DES_bs_crypt()
 * outputs and returns zero if no matches detected.
 */
extern int DES_bs_cmp_all(ARCH_WORD *binary, int count);

/*
 * Same as the above, except the comparison is done against only one of the
 * DES_bs_crypt() outputs.
 */
extern int DES_bs_cmp_one(ARCH_WORD *binary, int count, int index);

#endif
