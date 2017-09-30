/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

#include <string.h>

#include "arch.h"
#include "DES_std.h"
#include "DES_bs.h"

DES_bs_combined DES_bs_all;
int DES_bs_mem_saving = 0;

extern void DES_bs_body();

void DES_bs_init()
{
	int index, bit;

	for (index = 0; index < 0x300; index++) {
		bit = DES_K_bits[index];
		bit -= bit >> 3;
		DES_bs_all.Kp[index] = &DES_bs_all.K[55 - bit];
	}
}

void DES_bs_set_salt(ARCH_WORD salt)
{
	register int src, dst;
	register ARCH_WORD mask;

	mask = 1;
	for (dst = 0; dst < 48; dst++) {
		if (dst == 24) mask = 1;

		if (salt & mask) {
			if (dst < 24) src = dst + 24; else src = dst - 24;
		} else src = dst;

		DES_bs_all.E[dst] = &DES_bs_all.B[DES_E[src]];
		DES_bs_all.E[dst + 48] = &DES_bs_all.B[DES_E[src] + 32];

		mask <<= 1;
	}
}

void DES_bs_clear_keys()
{
	memset(DES_bs_all.K, 0, sizeof(DES_bs_all.K));
}

void DES_bs_set_key(char *key, int index)
{
	register char *ptr;
	register int ofs, bit;
	register ARCH_WORD value;

	ofs = 56;
	for (ptr = key; *ptr && ofs; ptr++) {
		bit = (ofs -= 7);
		value = *ptr & 0x7F;

		do {
			DES_bs_all.K[bit++] |= (value & 1) << index;
		} while (value >>= 1);
	}
}

void DES_bs_crypt(int count)
{
	register int bit;
	register ARCH_WORD R, L;

	memset(DES_bs_all.B, 0, sizeof(DES_bs_all.B));

	do {
		DES_bs_body();

		if (!--count) break;

		for (bit = 0; bit < 32; bit++) {
			R = DES_bs_all.B[bit];
			L = DES_bs_all.B[bit + 32];
			DES_bs_all.B[bit + 32] = R;
			DES_bs_all.B[bit] = L;
		}
	} while (1);
}

ARCH_WORD *DES_bs_get_binary(char *ciphertext)
{
	static ARCH_WORD out[64];
	ARCH_WORD *raw;
	int bit;
	int index, shift;
	int value;

	raw = DES_raw_get_binary(ciphertext);

	out[1] = out[0] = 0;
	for (bit = 0; bit < 64; bit++) {
		index = bit >> 4;

/* Swap L and R here instead of doing it one more time in DES_bs_crypt() */
		index ^= 2;

/* Calculate the number of one of the 16 data bits in raw[index] */
		shift = ((bit & 0xC) << 1) + (bit & 3) + 1;

/* Get the bit */
		value = (raw[index] >> shift) & 1;

		if (DES_bs_mem_saving)
/* Memory saving: pack the bits into two words */
			out[bit >> 5] |= value << (bit & 0x1F);
		else
/* We either set or clear all the bits in every word */
			out[bit] = value ? ~(ARCH_WORD)0 : 0;
	}

	return out;
}

int DES_bs_binary_hash(ARCH_WORD *binary, int count)
{
	int bit, result;

	if (DES_bs_mem_saving)
		return (int)*binary & ((1 << count) - 1);

	result = 0;
	for (bit = 0; bit < count; bit++)
		if (binary[bit]) result |= 1 << bit;

	return result;
}

int DES_bs_get_hash(int index, int count)
{
	register int bit, result;
	register ARCH_WORD mask;

	mask = (ARCH_WORD)1 << index;
	result = 0;
	for (bit = 0; bit < count; bit++)
		if (DES_bs_all.B[bit] & mask) result |= 1 << bit;

	return result;
}

/*
 * The trick I used here allows to compare one ciphertext against all the
 * DES_bs_crypt() outputs in just O(log2(ARCH_BITS)) operations.
 */
int DES_bs_cmp_all(ARCH_WORD *binary, int count)
{
	register int bit;
	register ARCH_WORD mask;

	mask = 0;
	if (DES_bs_mem_saving)
	for (bit = 0; bit < ((count < 32) ? count : 32); bit++) {
		mask |= DES_bs_all.B[bit] ^
			((binary[0] & (1 << bit)) ? ~(ARCH_WORD)0 : 0);
		if (mask == ~(ARCH_WORD)0) return 0;
	}
	else
	for (bit = 0; bit < count; bit++) {
		mask |= DES_bs_all.B[bit] ^ binary[bit];
		if (mask == ~(ARCH_WORD)0) return 0;
	}

	return 1;
}

int DES_bs_cmp_one(ARCH_WORD *binary, int count, int index)
{
	register int bit;
	register ARCH_WORD mask;

	if (DES_bs_mem_saving) {
		for (bit = 0; bit < count; bit++)
		if (((DES_bs_all.B[bit] >> index) ^
			(binary[bit >> 5] >> (bit & 0x1F))) & 1) return 0;

		return 1;
	}

	mask = (ARCH_WORD)1 << index;
	for (bit = 0; bit < count; bit++)
		if ((DES_bs_all.B[bit] ^ binary[bit]) & mask) return 0;

	return 1;
}
