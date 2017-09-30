/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

#include <string.h>

#include "arch.h"
#include "DES_std.h"

#if ARCH_GENERIC

#if ((__GNUC__ == 2) && (__GNUC_MINOR__ >= 7)) || (__GNUC__ > 2)
#define DES_ALIGN			__attribute__ ((aligned (128)))
#else
#define DES_ALIGN			/* nothing */
#endif

#endif

#if ARCH_BITS >= 64

#if ARCH_GENERIC

static union {
	double dummy;
	struct {
		DES_KS KS;
		ARCH_WORD SPE_F[8][64];
#if DES_128K
		ARCH_WORD SPE_W[4][64 * 64];
#endif
	} data;
} DES_ALIGN DES_all;

#else

extern ARCH_WORD DES_SPE_F[8][64];
#if DES_128K
extern ARCH_WORD DES_SPE_W[4][64 * 64];
#endif

#endif

static ARCH_WORD DES_SPE[8][64];

#else

#if ARCH_GENERIC

static union {
	double dummy;
	struct {
		DES_KS KS;
		ARCH_WORD SPE_L[8][64];
		ARCH_WORD cache_bank_shift;
		ARCH_WORD SPE_H[8][64];
#if DES_128K
		ARCH_WORD restore_double_word_alignment;
		ARCH_WORD SPE_W[4][64 * 64][2];
#endif
	} data;
} DES_ALIGN DES_all;

#else

#if DES_X2
extern ARCH_WORD DES_SPE_F[8][64][2];
#else
extern ARCH_WORD DES_SPE_L[8][64], DES_SPE_H[8][64];
#endif

#if DES_128K
extern ARCH_WORD DES_SPE_W[4][64 * 64][2];
#endif

#endif

static ARCH_WORD DES_SPE[8][64][2];

#endif

#if ARCH_GENERIC

#define DES_KS_copy			DES_all.data.KS
#define DES_SPE_F			DES_all.data.SPE_F
#define DES_SPE_L			DES_all.data.SPE_L
#define DES_SPE_H			DES_all.data.SPE_H
#define DES_SPE_W			DES_all.data.SPE_W

DES_binary DES_IV;
ARCH_WORD DES_count;

DES_KS DES_ALIGN DES_KS_current;
DES_KS DES_ALIGN DES_KS_table[8][128];

#endif

static int DES_KS_updates;
static char DES_key[16];

#if DES_COPY
#if !ARCH_GENERIC
extern DES_KS DES_KS_copy;
#endif
unsigned ARCH_WORD *DES_out;
#endif

extern DES_KS DES_KS_table[8][128];

#if ARCH_BITS >= 64
static ARCH_WORD DES_IP_E[8][16], DES_C_FP[16][16];
#else
static ARCH_WORD DES_IP_E[8][16][2], DES_C_FP[16][16][2];
#endif

/*
 * Some architecture-dependent definitions follow. Be sure to use correct
 * options while compiling, this might affect the performance a lot.
 */

#if (ARCH_BITS >= 64 && (DES_SCALE || !DES_MASK) || DES_128K) && \
	DES_SIZE_FIX == 2
/*
 * 64 bit architectures which can shift addresses left by 1 bit with no extra
 * time required (for example by adding a register to itself).
 */
#define DES_INDEX(SPE, i) \
	(*((ARCH_WORD *)(((unsigned char (*)[2])SPE) + (i))))
#else
#if DES_SIZE_FIX == 0
/*
 * 64 bit architectures which can shift addresses left by 3 bits (but maybe
 * not by 1) with no extra time required (for example by using the S8ADDQ
 * instruction on DEC Alphas; we would need an ADDQ anyway).
 */
#define DES_INDEX(SPE, i) \
	SPE[i]
#else
/*
 * Architectures with no complicated addressing modes supported, or when those
 * are not required.
 */
#define DES_INDEX(SPE, i) \
	(*((ARCH_WORD *)(((unsigned char *)SPE) + (i))))
#if ARCH_BITS < 64 && DES_128K
#define DES_INDEX_L(SPE, i) \
	(*((ARCH_WORD *)(((unsigned char *)&SPE[0][0]) + (i))))
#define DES_INDEX_H(SPE, i) \
	(*((ARCH_WORD *)(((unsigned char *)&SPE[0][1]) + (i))))
#endif
#endif
#endif

/*
 * You can choose between using shifts/masks, and using memory store and load
 * instructions.
 */

#if DES_MASK
#if ARCH_BITS >= 64 && !DES_SCALE && DES_SIZE_FIX == 2
/*
 * This method might be good for some 64 bit architectures with no complicated
 * addressing modes supported. It would be the best one for DEC Alphas if they
 * didn't have the S8ADDQ instruction.
 */
#define DES_MASK_6			(0x3F << 3)
#else
#if DES_EXTB
/*
 * Masking whole bytes allows the compiler to use Move with Zero Extension
 * instructions (where supported), like MOVZBL (MOVZX in Intel's syntax) on
 * x86s, or EXTBL on DEC Alphas. It might only be reasonable to disable this
 * if such instructions exist, but are slower than masks/shifts (as they are
 * on Pentiums).
 */
#define DES_MASK_6			((0x3F << DES_SIZE_FIX) | 0xFF)
#else
/*
 * Forces using plain shifts/masks, sometimes it's the only choice. Note that
 * you don't have to set DES_MASK if this is the case -- store/load method
 * might be faster.
 */
#define DES_MASK_6			(0x3F << DES_SIZE_FIX)
#endif
#endif
#endif

#if !DES_COPY
#undef DES_KS_copy
#define DES_KS_copy			KS
#endif
#define DES_KS_INDEX(i)			(DES_KS_copy + (i * (16 / DES_SIZE)))

#define DES_TO_6_BIT(x) \
	(((x) & 077) | \
	(((x) & 07700) << 2) | \
	(((x) & 0770000) << 4) | \
	(((x) & 077000000) << 6))

#if DES_128K
#define DES_UNDO_SIZE_FIX(x) \
	((((x) >> 1) & 0xFF00FF00) | (((x) & 0x00FF00FF) >> 3))
#else
#define DES_UNDO_SIZE_FIX(x) \
	((x) >> DES_SIZE_FIX)
#endif

static unsigned char DES_S[8][4][16] = {
	{
		{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
		{0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
		{4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
		{15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
	}, {
		{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
		{3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
		{0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
		{13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
	}, {
		{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
		{13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
		{13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
		{1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
	}, {
		{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
		{13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
		{10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
		{3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
	}, {
		{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
		{14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
		{4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
		{11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
	}, {
		{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
		{10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
		{9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
		{4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
	}, {
		{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
		{13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
		{1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
		{6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
	}, {
		{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
		{1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
		{7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
		{2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
	}
};

static unsigned char DES_P[32] = {
	15, 6, 19, 20,
	28, 11, 27, 16,
	0, 14, 22, 25,
	4, 17, 30, 9,
	1, 7, 23, 13,
	31, 26, 2, 8,
	18, 12, 29, 5,
	21, 10, 3, 24
};

unsigned char DES_E[48] = {
	31, 0, 1, 2, 3, 4,
	3, 4, 5, 6, 7, 8,
	7, 8, 9, 10, 11, 12,
	11, 12, 13, 14, 15, 16,
	15, 16, 17, 18, 19, 20,
	19, 20, 21, 22, 23, 24,
	23, 24, 25, 26, 27, 28,
	27, 28, 29, 30, 31, 0
};

static unsigned char DES_IP[64] = {
	57, 49, 41, 33, 25, 17, 9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7,
	56, 48, 40, 32, 24, 16, 8, 0,
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6
};

static unsigned char DES_C[64] = {
	0, 1, 2, 3, 16, 17, 18, 19,
	4, 5, 6, 7, 20, 21, 22, 23,
	8, 9, 10, 11, 24, 25, 26, 27,
	12, 13, 14, 15, 28, 29, 30, 31,
	32, 33, 34, 35, 48, 49, 50, 51,
	36, 37, 38, 39, 52, 53, 54, 55,
	40, 41, 42, 43, 56, 57, 58, 59,
	44, 45, 46, 47, 60, 61, 62, 63
};

unsigned char DES_K_bits[0x300] = {
	9, 50, 33, 59, 48, 16, 32, 56, 1, 8, 18, 41, 2, 34, 25, 24,
	43, 57, 58, 0, 35, 26, 17, 40, 21, 27, 38, 53, 36, 3, 46, 29,
	4, 52, 22, 28, 60, 20, 37, 62, 14, 19, 44, 13, 12, 61, 54, 30,
	1, 42, 25, 51, 40, 8, 24, 48, 58, 0, 10, 33, 59, 26, 17, 16,
	35, 49, 50, 57, 56, 18, 9, 32, 13, 19, 30, 45, 28, 62, 38, 21,
	27, 44, 14, 20, 52, 12, 29, 54, 6, 11, 36, 5, 4, 53, 46, 22,
	50, 26, 9, 35, 24, 57, 8, 32, 42, 49, 59, 17, 43, 10, 1, 0,
	48, 33, 34, 41, 40, 2, 58, 16, 60, 3, 14, 29, 12, 46, 22, 5,
	11, 28, 61, 4, 36, 27, 13, 38, 53, 62, 20, 52, 19, 37, 30, 6,
	34, 10, 58, 48, 8, 41, 57, 16, 26, 33, 43, 1, 56, 59, 50, 49,
	32, 17, 18, 25, 24, 51, 42, 0, 44, 54, 61, 13, 27, 30, 6, 52,
	62, 12, 45, 19, 20, 11, 60, 22, 37, 46, 4, 36, 3, 21, 14, 53,
	18, 59, 42, 32, 57, 25, 41, 0, 10, 17, 56, 50, 40, 43, 34, 33,
	16, 1, 2, 9, 8, 35, 26, 49, 28, 38, 45, 60, 11, 14, 53, 36,
	46, 27, 29, 3, 4, 62, 44, 6, 21, 30, 19, 20, 54, 5, 61, 37,
	2, 43, 26, 16, 41, 9, 25, 49, 59, 1, 40, 34, 24, 56, 18, 17,
	0, 50, 51, 58, 57, 48, 10, 33, 12, 22, 29, 44, 62, 61, 37, 20,
	30, 11, 13, 54, 19, 46, 28, 53, 5, 14, 3, 4, 38, 52, 45, 21,
	51, 56, 10, 0, 25, 58, 9, 33, 43, 50, 24, 18, 8, 40, 2, 1,
	49, 34, 35, 42, 41, 32, 59, 17, 27, 6, 13, 28, 46, 45, 21, 4,
	14, 62, 60, 38, 3, 30, 12, 37, 52, 61, 54, 19, 22, 36, 29, 5,
	35, 40, 59, 49, 9, 42, 58, 17, 56, 34, 8, 2, 57, 24, 51, 50,
	33, 18, 48, 26, 25, 16, 43, 1, 11, 53, 60, 12, 30, 29, 5, 19,
	61, 46, 44, 22, 54, 14, 27, 21, 36, 45, 38, 3, 6, 20, 13, 52,
	56, 32, 51, 41, 1, 34, 50, 9, 48, 26, 0, 59, 49, 16, 43, 42,
	25, 10, 40, 18, 17, 8, 35, 58, 3, 45, 52, 4, 22, 21, 60, 11,
	53, 38, 36, 14, 46, 6, 19, 13, 28, 37, 30, 62, 61, 12, 5, 44,
	40, 16, 35, 25, 50, 18, 34, 58, 32, 10, 49, 43, 33, 0, 56, 26,
	9, 59, 24, 2, 1, 57, 48, 42, 54, 29, 36, 19, 6, 5, 44, 62,
	37, 22, 20, 61, 30, 53, 3, 60, 12, 21, 14, 46, 45, 27, 52, 28,
	24, 0, 48, 9, 34, 2, 18, 42, 16, 59, 33, 56, 17, 49, 40, 10,
	58, 43, 8, 51, 50, 41, 32, 26, 38, 13, 20, 3, 53, 52, 28, 46,
	21, 6, 4, 45, 14, 37, 54, 44, 27, 5, 61, 30, 29, 11, 36, 12,
	8, 49, 32, 58, 18, 51, 2, 26, 0, 43, 17, 40, 1, 33, 24, 59,
	42, 56, 57, 35, 34, 25, 16, 10, 22, 60, 4, 54, 37, 36, 12, 30,
	5, 53, 19, 29, 61, 21, 38, 28, 11, 52, 45, 14, 13, 62, 20, 27,
	57, 33, 16, 42, 2, 35, 51, 10, 49, 56, 1, 24, 50, 17, 8, 43,
	26, 40, 41, 48, 18, 9, 0, 59, 6, 44, 19, 38, 21, 20, 27, 14,
	52, 37, 3, 13, 45, 5, 22, 12, 62, 36, 29, 61, 60, 46, 4, 11,
	41, 17, 0, 26, 51, 48, 35, 59, 33, 40, 50, 8, 34, 1, 57, 56,
	10, 24, 25, 32, 2, 58, 49, 43, 53, 28, 3, 22, 5, 4, 11, 61,
	36, 21, 54, 60, 29, 52, 6, 27, 46, 20, 13, 45, 44, 30, 19, 62,
	25, 1, 49, 10, 35, 32, 48, 43, 17, 24, 34, 57, 18, 50, 41, 40,
	59, 8, 9, 16, 51, 42, 33, 56, 37, 12, 54, 6, 52, 19, 62, 45,
	20, 5, 38, 44, 13, 36, 53, 11, 30, 4, 60, 29, 28, 14, 3, 46,
	17, 58, 41, 2, 56, 24, 40, 35, 9, 16, 26, 49, 10, 42, 33, 32,
	51, 0, 1, 8, 43, 34, 25, 48, 29, 4, 46, 61, 44, 11, 54, 37,
	12, 60, 30, 36, 5, 28, 45, 3, 22, 27, 52, 21, 20, 6, 62, 38
};

static void init_SPE()
{
	int box, index, row, column, bit;
	ARCH_WORD mask, l, h;

	for (box = 0; box < 8; box++)
	for (index = 0; index < 64; index++) {
		row =
			((index & 1) << 1) |
			(index >> 5);
		column =
			(((index >> 1) & 1) << 3) |
			(((index >> 2) & 1) << 2) |
			(((index >> 3) & 1) << 1) |
			((index >> 4) & 1);

		mask = (ARCH_WORD)DES_S[box][row][column] << ((7 - box) << 2);

		h = l = 0;
		for (bit = 0; bit < 24; bit++) {
			if (((unsigned ARCH_WORD)0x80000000 >>
			    DES_P[DES_E[bit]]) & mask)
				l |= 1 << bit;
			if (((unsigned ARCH_WORD)0x80000000 >>
			    DES_P[DES_E[bit + 24]]) & mask)
				h |= 1 << bit;
		}

		l = DES_TO_6_BIT(l); h = DES_TO_6_BIT(h);

#if ARCH_BITS >= 64
		DES_SPE[box][index] =
			DES_DO_SIZE_FIX(l) |
			(DES_DO_SIZE_FIX(h) << 32);
#else
		DES_SPE[box][index][0] = DES_DO_SIZE_FIX(l);
		DES_SPE[box][index][1] = DES_DO_SIZE_FIX(h);
#endif
	}
}

static void init_IP_E()
{
	int src, dst, dst1, dst2;
	int chunk, mask, value;

	memset(DES_IP_E, 0, sizeof(DES_IP_E));

	for (dst1 = 0; dst1 < 8; dst1++)
	for (dst2 = 0; dst2 < 6; dst2++) {
		dst = (dst1 << 3) + dst2;
		src = DES_IP[DES_E[dst1 * 6 + dst2]];
		if (src >= 32) src -= 32; else src--;
		src ^= 7;

		chunk = src >> 2;
		mask = 1 << (src & 3);

		for (value = 0; value < 16; value++)
		if (value & mask)
#if ARCH_BITS >= 64
			DES_IP_E[chunk][value] |=
				(ARCH_WORD)1 << dst;
#else
			DES_IP_E[chunk][value][dst >> 5] |=
				(ARCH_WORD)1 << (dst & 0x1F);
#endif
	}
}

static void init_C_FP()
{
	int src, dst;
	int chunk, mask, value;

	memset(DES_C_FP, 0, sizeof(DES_C_FP));

	for (src = 0; src < 64; src++) {
		dst = DES_IP[DES_C[src]] ^ 7;

		chunk = src >> 2;
		mask = 1 << (src & 3);

		for (value = 0; value < 16; value++)
		if (value & mask)
#if ARCH_BITS >= 64
			DES_C_FP[chunk][value] |=
				(ARCH_WORD)1 << dst;
#else
			DES_C_FP[chunk][value][dst >> 5] |=
				(ARCH_WORD)1 << (dst & 0x1F);
#endif
	}
}

static void init_KS()
{
	int pos, chr, bit, ofs, index;
	unsigned char block[64];
	unsigned char *k;
	ARCH_WORD value;

	memset(block, 0, sizeof(block));

	for (pos = 0; pos < 8; pos++)
	for (chr = 0x7F; chr >= 0; chr--) {
		for (bit = 0; bit < 7; bit++)
			block[(pos << 3) + bit] = (chr >> (6 - bit)) & 1;

		k = DES_K_bits;
		for (index = 0; index < 32; index++) {
			value = 0;
			for (ofs = 0; ofs < 32; ofs += 8)
			for (bit = 0; bit < 6; bit++) value |=
				(ARCH_WORD)block[*k++] << (ofs + bit);

#if ARCH_BITS >= 64
			if (index & 1)
				DES_KS_table[pos][chr][index >> 1] |=
					DES_DO_SIZE_FIX(value) << 32;
			else
				DES_KS_table[pos][chr][index >> 1] =
					DES_DO_SIZE_FIX(value);
#else
			DES_KS_table[pos][chr][index] = DES_DO_SIZE_FIX(value);
#endif
		}
	}

	DES_KS_updates = 0;
	memset(DES_key, 0, sizeof(DES_key));
	memcpy(DES_KS_current, DES_KS_table, sizeof(DES_KS));
}

void DES_std_init()
{
	init_SPE();
	init_IP_E();
	init_C_FP();
	init_KS();

	memset(DES_IV, 0, sizeof(DES_IV));
	DES_count = 25;
}

void DES_std_set_salt(ARCH_WORD salt)
{
	int box, index;
	ARCH_WORD xor;
#if ARCH_BITS >= 64
	unsigned ARCH_WORD src;
#else
	ARCH_WORD l, h;
#endif

	for (box = 0; box < 8; box++)
	for (index = 0; index < 64; index++) {
#if ARCH_BITS >= 64
		src = DES_SPE[box][index];
		xor = (src ^ (src >> 32)) & salt;

		DES_SPE_F[box][index] = src ^ (xor | (xor << 32));
#else
		l = DES_SPE[box][index][0];
		h = DES_SPE[box][index][1];
		xor = (l ^ h) & salt;

#if DES_X2
		DES_SPE_F[box][index][0] = l ^ xor;
		DES_SPE_F[box][index][1] = h ^ xor;
#else
		DES_SPE_L[box][index] = l ^ xor;
		DES_SPE_H[box][index] = h ^ xor;
#endif
#endif
	}

#if DES_128K
	for (box = 0; box < 4; box++)
	for (index = 0; index < 64 * 64; index++) {
#if ARCH_BITS >= 64
		DES_SPE_W[box][index] =
			DES_SPE_F[box << 1][index & 0x3F] ^
			DES_SPE_F[(box << 1) + 1][index >> 6];
#else
		DES_SPE_W[box][index][0] =
			DES_SPE_L[box << 1][index & 0x3F] ^
			DES_SPE_L[(box << 1) + 1][index >> 6];
		DES_SPE_W[box][index][1] =
			DES_SPE_H[box << 1][index & 0x3F] ^
			DES_SPE_H[(box << 1) + 1][index >> 6];
#endif
	}
#endif
}

/*
 * The new DES_std_set_key() idea is originally by Roman Rusakov. I extended
 * the original idea to detect whether it's faster to calculate from scratch
 * or modify the previous key schedule. This code assumes that DES_xor_key1()
 * is 1.5 times faster than DES_xor_key2().
 */

#if !DES_ASM

#define DES_xor1_1(ofs) \
	DES_KS_current[ofs] ^= value1[ofs];
#define DES_xor1_4(ofs) \
	DES_xor1_1(ofs); \
	DES_xor1_1(ofs + 1); \
	DES_xor1_1(ofs + 2); \
	DES_xor1_1(ofs + 3);
#define DES_xor1_16(ofs) \
	DES_xor1_4(ofs); \
	DES_xor1_4(ofs + 4); \
	DES_xor1_4(ofs + 8); \
	DES_xor1_4(ofs + 12);

#if ARCH_BITS >= 64
#define DES_xor_key1(_value) { \
	value1 = _value; \
	DES_xor1_16(0); \
}
#else
#define DES_xor_key1(_value) { \
	value1 = _value; \
	DES_xor1_16(0); \
	DES_xor1_16(16); \
}
#endif

#define DES_xor2_1(ofs) \
	DES_KS_current[ofs] ^= value1[ofs] ^ value2[ofs];
#define DES_xor2_4(ofs) \
	DES_xor2_1(ofs); \
	DES_xor2_1(ofs + 1); \
	DES_xor2_1(ofs + 2); \
	DES_xor2_1(ofs + 3);
#define DES_xor2_16(ofs) \
	DES_xor2_4(ofs); \
	DES_xor2_4(ofs + 4); \
	DES_xor2_4(ofs + 8); \
	DES_xor2_4(ofs + 12);

#if ARCH_BITS >= 64
#define DES_xor_key2(_value1, _value2) { \
	value1 = _value1; value2 = _value2; \
	DES_xor2_16(0); \
}
#else
#define DES_xor_key2(_value1, _value2) { \
	value1 = _value1; value2 = _value2; \
	DES_xor2_16(0); \
	DES_xor2_16(16); \
}
#endif

#else

extern void DES_xor_key1(ARCH_WORD *value);
extern void DES_xor_key2(ARCH_WORD *value1, ARCH_WORD *value2);

#endif

void DES_raw_set_key(char *key)
{
	register int i;
	DES_KS *pos;
#if !DES_ASM
	register ARCH_WORD *value1;
#endif

	memcpy(DES_KS_current,
		DES_KS_table[0][(ARCH_INDEX)(DES_key[0] = key[0] & 0x7F)],
		sizeof(DES_KS));

	pos = (DES_KS *)DES_KS_table[1][0];
	for (i = 1; i < 8; i++) {
		DES_xor_key1(pos[(ARCH_INDEX)(DES_key[i] = key[i] & 0x7F)]);
		pos += 128;
	}
}

void DES_std_set_key(char *key)
{
	register int i, j, k, l;
#if !DES_ASM
	register ARCH_WORD *value1, *value2;
#endif

	for (k = i = 0; (j = key[i]) && (l = DES_key[i]); i++)
	if (j != l) k += 3;

	if (j) {
		while (i < 8 && key[i]) {
			i++; k += 2;
		}
		j = i;
	} else {
		j = i;
		while (DES_key[i++]) k += 2;
	}

	if ((k < (j << 1)) && (++DES_KS_updates & 0xFFF)) {
		j = 0; l = 1;
		for (i = 0; (k = key[i]) && i < 8; i++) {
			if (l)
			if (k == (l = DES_key[j++])) continue;

			if (l) {
				DES_xor_key2(DES_KS_table[i][l],
					DES_KS_table[i][k & 0x7F]);
			} else
				DES_xor_key1(DES_KS_table[i][k & 0x7F]);
		}

		if (l)
		for (; j < 8 && (l = DES_key[j]); j++)
			DES_xor_key1(DES_KS_table[j][l]);
	} else {
		memcpy(DES_KS_current, DES_KS_table[0][(k = key[0]) & 0x7F],
			sizeof(DES_KS));

		if (k)
		for (i = 1; (k = key[i]) && i < 8; i++)
			DES_xor_key1(DES_KS_table[i][k & 0x7F]);
	}

	DES_key[0] = key[0] & 0x7F;
	DES_key[1] = key[1] & 0x7F;
	DES_key[2] = key[2] & 0x7F;
	DES_key[3] = key[3] & 0x7F;
	DES_key[4] = key[4] & 0x7F;
	DES_key[5] = key[5] & 0x7F;
	DES_key[6] = key[6] & 0x7F;
	DES_key[7] = key[7] & 0x7F;
}

void DES_std_set_block(ARCH_WORD R, ARCH_WORD L)
{
	register ARCH_WORD Rl, Rh, Ll, Lh;
	register unsigned ARCH_WORD C;
#if ARCH_BITS >= 64
	register ARCH_WORD mask;
#else
	register ARCH_WORD *mask;
#endif
	int chunk;

	C = (R & 0xAAAAAAAA) | ((L & 0xAAAAAAAA) >> 1);
	Rh = Rl = 0;
	for (chunk = 0; chunk < 8; chunk++) {
		mask = DES_IP_E[chunk][C & 0xF];
#if ARCH_BITS >= 64
		Rl |= mask & 0xFFFFFFFF;
		Rh |= mask >> 32;
#else
		Rl |= mask[0];
		Rh |= mask[1];
#endif
		C >>= 4;
	}

	C = ((R & 0x55555555) << 1) | (L & 0x55555555);
	Lh = Ll = 0;
	for (chunk = 0; chunk < 8; chunk++) {
		mask = DES_IP_E[chunk][C & 0xF];
#if ARCH_BITS >= 64
		Ll |= mask & 0xFFFFFFFF;
		Lh |= mask >> 32;
#else
		Ll |= mask[0];
		Lh |= mask[1];
#endif
		C >>= 4;
	}

	Rl = DES_DO_SIZE_FIX(Rl);
	Rh = DES_DO_SIZE_FIX(Rh);
	Ll = DES_DO_SIZE_FIX(Ll);
	Lh = DES_DO_SIZE_FIX(Lh);

#if ARCH_BITS >= 64
	DES_IV[0] = Rl | (Rh << 32);
	DES_IV[1] = Ll | (Lh << 32);
#else
	DES_IV[0] = Rl;
	DES_IV[1] = Rh;
	DES_IV[2] = Ll;
	DES_IV[3] = Lh;
#endif
}

void DES_std_get_block(DES_binary binary, unsigned ARCH_WORD out[2])
{
	register ARCH_WORD Rl, Rh, Ll, Lh;
	register ARCH_WORD R, L;
	register unsigned ARCH_WORD C;
#if ARCH_BITS >= 64
	register unsigned ARCH_WORD mask;
#else
	register ARCH_WORD *mask;
#endif
	int chunk;

#if ARCH_BITS >= 64
	Rl = binary[0];
	Rh = binary[0] >> 32;
	Ll = binary[1];
	Lh = binary[1] >> 32;
#else
	Rl = binary[0];
	Rh = binary[1];
	Ll = binary[2];
	Lh = binary[3];
#endif

	Rl = DES_UNDO_SIZE_FIX(Rl);
	Rh = DES_UNDO_SIZE_FIX(Rh);
	Ll = DES_UNDO_SIZE_FIX(Ll);
	Lh = DES_UNDO_SIZE_FIX(Lh);

	R = L = 0;
	C = ((Ll >> 1) & 0x0F0F0F0F) | ((Lh << 3) & 0xF0F0F0F0);
	for (chunk = 0; chunk < 16; chunk++) {
		if (chunk == 8)
			C = ((Rl >> 1) & 0x0F0F0F0F) | ((Rh << 3) & 0xF0F0F0F0);
		mask = DES_C_FP[chunk][C & 0xF];
#if ARCH_BITS >= 64
		R |= mask & 0xFFFFFFFF;
		L |= mask >> 32;
#else
		R |= mask[0];
		L |= mask[1];
#endif
		C >>= 4;
	}

	out[0] = R;
	out[1] = L;
}

#if !DES_ASM

#if !DES_MASK
#define DES_D				(DES_tmp.w)
#if ARCH_LITTLE_ENDIAN
#define DES_Dl				(DES_tmp.w[0])
#define DES_Dh				(DES_tmp.w[1])
#define DES_B0				(DES_tmp.b[0])
#define DES_B1				(DES_tmp.b[1])
#define DES_B2				(DES_tmp.b[2])
#define DES_B3				(DES_tmp.b[3])
#define DES_B4				(DES_tmp.b[4])
#define DES_B5				(DES_tmp.b[5])
#define DES_B6				(DES_tmp.b[6])
#define DES_B7				(DES_tmp.b[7])
#else
#define DES_Dl				(DES_tmp.w[1])
#define DES_Dh				(DES_tmp.w[0])
#if ARCH_BITS > 64
#define DES_START			ARCH_SIZE
#else
#define DES_START			8
#endif
#define DES_B0				(DES_tmp.b[DES_START - 1])
#define DES_B1				(DES_tmp.b[DES_START - 2])
#define DES_B2				(DES_tmp.b[DES_START - 3])
#define DES_B3				(DES_tmp.b[DES_START - 4])
#define DES_B4				(DES_tmp.b[DES_START - 5])
#define DES_B5				(DES_tmp.b[DES_START - 6])
#define DES_B6				(DES_tmp.b[DES_START - 7])
#define DES_B7				(DES_tmp.b[DES_START - 8])
#endif
#endif

/*
 * The code below is heavily optimized, looking at the assembly output for
 * several architectures, enjoy the speed.
 */

#if ARCH_BITS >= 64
#if !DES_128K
/*
 * An extra temporary register is used for better instruction scheduling.
 */
#if DES_MASK
#if DES_SCALE
/* 64 bit, 4K, mask, scale */
#define DES_ROUND(L, H, B) \
	T = DES_INDEX(DES_SPE_F[0], DES_D & DES_MASK_6); \
	B ^= DES_INDEX(DES_SPE_F[1], (DES_D >> 8) & DES_MASK_6); \
	T ^= DES_INDEX(DES_SPE_F[2], (DES_D >> 16) & DES_MASK_6); \
	B ^= DES_INDEX(DES_SPE_F[3], (DES_D >> 24) & DES_MASK_6); \
	T ^= DES_INDEX(DES_SPE_F[4], (DES_D >> 32) & DES_MASK_6); \
	B ^= DES_INDEX(DES_SPE_F[5], (DES_D >> 40) & DES_MASK_6); \
	T ^= DES_INDEX(DES_SPE_F[6], (DES_D >> 48) & DES_MASK_6); \
	B ^= T ^ DES_INDEX(DES_SPE_F[7], DES_D >> 56);
#else
/*
 * This code assumes that the indices are already shifted left by 2, and still
 * should be shifted left by 1 more bit. Weird shift counts allow doing this
 * with only one extra shift, in the first line. By shifting the mask right
 * (which is done at compile time), I moved the extra shift out of the mask,
 * which allows doing this with special addressing modes, where possible.
 */
/* 64 bit, 4K, mask, no scale */
#define DES_ROUND(L, H, B) \
	T = DES_INDEX(DES_SPE_F[0], (DES_D & (DES_MASK_6 >> 1)) << 1); \
	B ^= DES_INDEX(DES_SPE_F[1], (DES_D >> 7) & DES_MASK_6); \
	T ^= DES_INDEX(DES_SPE_F[2], (DES_D >> 15) & DES_MASK_6); \
	B ^= DES_INDEX(DES_SPE_F[3], (DES_D >> 23) & DES_MASK_6); \
	T ^= DES_INDEX(DES_SPE_F[4], (DES_D >> 31) & DES_MASK_6); \
	B ^= DES_INDEX(DES_SPE_F[5], (DES_D >> 39) & DES_MASK_6); \
	T ^= DES_INDEX(DES_SPE_F[6], (DES_D >> 47) & DES_MASK_6); \
	B ^= T ^ DES_INDEX(DES_SPE_F[7], (DES_D >> 55) & DES_MASK_6);
#endif
#else
/* 64 bit, 4K, store/load, scale */
#define DES_ROUND(L, H, B) \
	T = DES_INDEX(DES_SPE_F[0], DES_B0); \
	B ^= DES_INDEX(DES_SPE_F[1], DES_B1); \
	T ^= DES_INDEX(DES_SPE_F[2], DES_B2); \
	B ^= DES_INDEX(DES_SPE_F[3], DES_B3); \
	T ^= DES_INDEX(DES_SPE_F[4], DES_B4); \
	B ^= DES_INDEX(DES_SPE_F[5], DES_B5); \
	T ^= DES_INDEX(DES_SPE_F[6], DES_B6); \
	B ^= T ^ DES_INDEX(DES_SPE_F[7], DES_B7);
#endif
#else
/* 64 bit, 128K, mask, no scale */
#define DES_ROUND(L, H, B) \
	B ^= DES_INDEX(DES_SPE_W[0], DES_D & 0xFFFF); \
	B ^= DES_INDEX(DES_SPE_W[1], (DES_D >>= 16) & 0xFFFF); \
	B ^= DES_INDEX(DES_SPE_W[2], (DES_D >>= 16) & 0xFFFF); \
	B ^= DES_INDEX(DES_SPE_W[3], DES_D >> 16);
#endif
#else
#if !DES_128K
#if DES_MASK
/* 32 bit, 4K, mask */
#define DES_ROUND(L, H, B) \
	L ^= DES_INDEX(DES_SPE_L[0], DES_Dl & DES_MASK_6); \
	H ^= DES_INDEX(DES_SPE_H[0], DES_Dl & DES_MASK_6); \
	L ^= DES_INDEX(DES_SPE_L[1], (DES_Dl >>= 8) & DES_MASK_6); \
	H ^= DES_INDEX(DES_SPE_H[1], DES_Dl & DES_MASK_6); \
	L ^= DES_INDEX(DES_SPE_L[2], (DES_Dl >>= 8) & DES_MASK_6); \
	H ^= DES_INDEX(DES_SPE_H[2], DES_Dl & DES_MASK_6); \
	L ^= DES_INDEX(DES_SPE_L[3], (DES_Dl >>= 8) & DES_MASK_6); \
	H ^= DES_INDEX(DES_SPE_H[3], DES_Dl & DES_MASK_6); \
	L ^= DES_INDEX(DES_SPE_L[4], DES_Dh & DES_MASK_6); \
	H ^= DES_INDEX(DES_SPE_H[4], DES_Dh & DES_MASK_6); \
	L ^= DES_INDEX(DES_SPE_L[5], (DES_Dh >>= 8) & DES_MASK_6); \
	H ^= DES_INDEX(DES_SPE_H[5], DES_Dh & DES_MASK_6); \
	L ^= DES_INDEX(DES_SPE_L[6], (DES_Dh >>= 8) & DES_MASK_6); \
	H ^= DES_INDEX(DES_SPE_H[6], DES_Dh & DES_MASK_6); \
	L ^= DES_INDEX(DES_SPE_L[7], (DES_Dh >>= 8) & DES_MASK_6); \
	H ^= DES_INDEX(DES_SPE_H[7], DES_Dh & DES_MASK_6);
#else
/* 32 bit, 4K, store/load */
#define DES_ROUND(L, H, B) \
	L ^= DES_INDEX(DES_SPE_L[0], DES_B0); \
	H ^= DES_INDEX(DES_SPE_H[0], DES_B0); \
	L ^= DES_INDEX(DES_SPE_L[1], DES_B1); \
	H ^= DES_INDEX(DES_SPE_H[1], DES_B1); \
	L ^= DES_INDEX(DES_SPE_L[2], DES_B2); \
	H ^= DES_INDEX(DES_SPE_H[2], DES_B2); \
	L ^= DES_INDEX(DES_SPE_L[3], DES_B3); \
	H ^= DES_INDEX(DES_SPE_H[3], DES_B3); \
	L ^= DES_INDEX(DES_SPE_L[4], DES_B4); \
	H ^= DES_INDEX(DES_SPE_H[4], DES_B4); \
	L ^= DES_INDEX(DES_SPE_L[5], DES_B5); \
	H ^= DES_INDEX(DES_SPE_H[5], DES_B5); \
	L ^= DES_INDEX(DES_SPE_L[6], DES_B6); \
	H ^= DES_INDEX(DES_SPE_H[6], DES_B6); \
	L ^= DES_INDEX(DES_SPE_L[7], DES_B7); \
	H ^= DES_INDEX(DES_SPE_H[7], DES_B7);
#endif
#else
/* 32 bit, 128K, mask */
#define DES_ROUND(L, H, B) \
	L ^= DES_INDEX_L(DES_SPE_W[0], DES_Dl & 0xFFFF); \
	H ^= DES_INDEX_H(DES_SPE_W[0], DES_Dl & 0xFFFF); \
	L ^= DES_INDEX_L(DES_SPE_W[1], DES_Dl >>= 16); \
	H ^= DES_INDEX_H(DES_SPE_W[1], DES_Dl); \
	L ^= DES_INDEX_L(DES_SPE_W[2], DES_Dh & 0xFFFF); \
	H ^= DES_INDEX_H(DES_SPE_W[2], DES_Dh & 0xFFFF); \
	L ^= DES_INDEX_L(DES_SPE_W[3], DES_Dh >>= 16); \
	H ^= DES_INDEX_H(DES_SPE_W[3], DES_Dh);
#endif
#endif

#if ARCH_BITS >= 64
#define DES_2_ROUNDS(KS) \
	DES_D = R ^ KS[0]; \
	DES_ROUND(Ll, Lh, L); \
	DES_D = L ^ KS[1]; \
	DES_ROUND(Rl, Rh, R);
#else
#define DES_2_ROUNDS(KS) \
	DES_Dl = Rl ^ KS[0]; \
	DES_Dh = Rh ^ KS[1]; \
	DES_ROUND(Ll, Lh, L); \
	DES_Dl = Ll ^ KS[2]; \
	DES_Dh = Lh ^ KS[3]; \
	DES_ROUND(Rl, Rh, R);
#endif

#if DES_COPY
static void crypt_body()
#else
void DES_std_crypt(DES_KS KS, DES_binary DES_out)
#endif
{
#if DES_MASK
#if ARCH_BITS >= 64
	register unsigned ARCH_WORD DES_D;
#else
	register unsigned ARCH_WORD DES_Dl, DES_Dh;
#endif
#else
	union {
#if ARCH_BITS >= 64
		ARCH_WORD w;
#else
		ARCH_WORD w[2];
#endif
		unsigned char b[8];
	} DES_tmp;
#endif
#if ARCH_BITS >= 64
	register ARCH_WORD R, L;
#if !DES_128K
	register ARCH_WORD T;
#endif
#else
	register ARCH_WORD Rl, Rh, Ll, Lh;
#endif
	register int count;

#if ARCH_BITS >= 64
	R = DES_IV[0];
	L = DES_IV[1];
#else
	Rl = DES_IV[0];
	Rh = DES_IV[1];
	Ll = DES_IV[2];
	Lh = DES_IV[3];
#endif
	count = DES_count;

	do {
		DES_2_ROUNDS(DES_KS_INDEX(0));
		DES_2_ROUNDS(DES_KS_INDEX(1));
		DES_2_ROUNDS(DES_KS_INDEX(2));
		DES_2_ROUNDS(DES_KS_INDEX(3));
		DES_2_ROUNDS(DES_KS_INDEX(4));
		DES_2_ROUNDS(DES_KS_INDEX(5));
		DES_2_ROUNDS(DES_KS_INDEX(6));
		DES_2_ROUNDS(DES_KS_INDEX(7));

#if ARCH_BITS >= 64
		L ^= R;
		R ^= L;
		L ^= R;
#else
		Ll ^= Rl;
		Lh ^= Rh;
		Rl ^= Ll;
		Rh ^= Lh;
		Ll ^= Rl;
		Lh ^= Rh;
#endif
	} while (--count);

#if ARCH_BITS >= 64
	DES_out[0] = R;
	DES_out[1] = L;
#else
	DES_out[0] = Rl;
	DES_out[1] = Rh;
	DES_out[2] = Ll;
	DES_out[3] = Lh;
#endif
}

#if DES_COPY
void DES_std_crypt(DES_KS KS, DES_binary out)
{
	memcpy(DES_KS_copy, KS, sizeof(DES_KS));
	DES_out = out;

	crypt_body();
}
#endif

#endif

static unsigned char DES_atoi64[0x100] = {
	18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
	34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
	50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 0, 1,
	2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 5, 6, 7, 8, 9, 10,
	11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
	27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 32, 33, 34, 35, 36,
	37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
	53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 0, 1, 2, 3, 4,
	5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
	21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
	37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
	53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 0, 1, 2, 3, 4,
	5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
	21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
	37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
	53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 0, 1, 2, 3, 4
};

ARCH_WORD DES_raw_get_salt(char *ciphertext)
{
	if (ciphertext[13]) return DES_atoi64[(ARCH_INDEX)ciphertext[5]] |
		((ARCH_WORD)DES_atoi64[(ARCH_INDEX)ciphertext[6]] << 6) |
		((ARCH_WORD)DES_atoi64[(ARCH_INDEX)ciphertext[7]] << 12) |
		((ARCH_WORD)DES_atoi64[(ARCH_INDEX)ciphertext[8]] << 18);
	else return DES_atoi64[(ARCH_INDEX)ciphertext[0]] |
		((ARCH_WORD)DES_atoi64[(ARCH_INDEX)ciphertext[1]] << 6);
}

ARCH_WORD DES_std_get_salt(char *ciphertext)
{
	ARCH_WORD salt;

	salt = DES_raw_get_salt(ciphertext);
	salt = DES_TO_6_BIT(salt);
	return DES_DO_SIZE_FIX(salt);
}

ARCH_WORD DES_raw_get_count(char *ciphertext)
{
	if (ciphertext[13]) return DES_atoi64[(ARCH_INDEX)ciphertext[1]] |
		((ARCH_WORD)DES_atoi64[(ARCH_INDEX)ciphertext[2]] << 6) |
		((ARCH_WORD)DES_atoi64[(ARCH_INDEX)ciphertext[3]] << 12) |
		((ARCH_WORD)DES_atoi64[(ARCH_INDEX)ciphertext[4]] << 18);
	else return 25;
}

ARCH_WORD *DES_raw_get_binary(char *ciphertext)
{
	static ARCH_WORD masks[8] = {
		0x1000, 0x0800, 0x0400, 0x0200, 0x0010, 0x0008, 0x0004, 0x0002
	};

	static ARCH_WORD out[4];

	int ofs, chr, bit, index, value;
	unsigned char block[66], *ptr;
	ARCH_WORD mask1, mask2;
	ARCH_WORD Rl, Rh, Ll, Lh;

	if (ciphertext[13]) ofs = 9; else ofs = 2;
	ptr = block;

	for (chr = 0; chr < 11; chr++) {
		value = DES_atoi64[(ARCH_INDEX)ciphertext[chr + ofs]];
		mask1 = 0x20;

		for (bit = 0; bit < 6; bit++) {
			*ptr++ = value & mask1;
			mask1 >>= 1;
		}
	}

	Lh = Ll = Rh = Rl = 0;
	ptr = block;

	for (index = 0; index < 8; index++) {
		mask2 = (mask1 = masks[index]) << 16;

		if (*ptr++) Rl |= mask1;
		if (*ptr++) Ll |= mask1;
		if (*ptr++) Rl |= mask2;
		if (*ptr++) Ll |= mask2;
		if (*ptr++) Rh |= mask1;
		if (*ptr++) Lh |= mask1;
		if (*ptr++) Rh |= mask2;
		if (*ptr++) Lh |= mask2;
	}

	out[0] = Rl;
	out[1] = Rh;
	out[2] = Ll;
	out[3] = Lh;

	return out;
}

ARCH_WORD *DES_std_get_binary(char *ciphertext)
{
	ARCH_WORD *out;
	ARCH_WORD salt, mask;

	out = DES_raw_get_binary(ciphertext);

	out[0] = DES_DO_SIZE_FIX(out[0]);
	out[1] = DES_DO_SIZE_FIX(out[1]);
	out[2] = DES_DO_SIZE_FIX(out[2]);
	out[3] = DES_DO_SIZE_FIX(out[3]);

	salt = DES_std_get_salt(ciphertext);

	mask = (out[0] ^ out[1]) & salt;
	out[0] ^= mask;
	out[1] ^= mask;

	mask = (out[2] ^ out[3]) & salt;
	out[2] ^= mask;
	out[3] ^= mask;

#if ARCH_BITS >= 64
	out[0] |= out[1] << 32;
	out[1] = out[2] | (out[3] << 32);
#endif

	return out;
}
