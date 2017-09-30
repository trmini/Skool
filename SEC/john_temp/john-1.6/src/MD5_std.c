/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 *
 * Based on the RSA Data Security, Inc. MD5 Message-Digest Algorithm and
 * FreeBSD MD5-based crypt(3) by Poul-Henning Kamp <phk@login.dknet.dk>.
 * My implementation supports passwords of up to 15 characters long only,
 * since this allows using a significantly faster algorithm -- SD.
 */

#include <string.h>

#include "arch.h"
#include "common.h"
#include "MD5_std.h"

MD5_binary MD5_out;

typedef union {
	MD5_word w[15];
	char b[60];
} MD5_block;

#if !MD5_ASM

#define S11				7
#define S12				12
#define S13				17
#define S14				22
#define S21				5
#define S22				9
#define S23				14
#define S24				20
#define S31				4
#define S32				11
#define S33				16
#define S34				23
#define S41				6
#define S42				10
#define S43				15
#define S44				21

#define Ca				0x67452301
#define Cb				0xefcdab89
#define Cc				0x98badcfe
#define Cd				0x10325476

#if MD5_IMM

/*
 * Using immediate values is good for CISC.
 */

#define AC1				0xd76aa477
#define AC2				0xe8c7b756
#define AC3				0x242070db
#define AC4				0xc1bdceee
#define AC5				0xf57c0faf
#define AC6				0x4787c62a
#define AC7				0xa8304613
#define AC8				0xfd469501
#define AC9				0x698098d8
#define AC10				0x8b44f7af
#define AC11				0xffff5bb1
#define AC12				0x895cd7be
#define AC13				0x6b901122
#define AC14				0xfd987193
#define AC15				0xa679438e
#define AC16				0x49b40821
#define AC17				0xf61e2562
#define AC18				0xc040b340
#define AC19				0x265e5a51
#define AC20				0xe9b6c7aa
#define AC21				0xd62f105d
#define AC22				0x02441453
#define AC23				0xd8a1e681
#define AC24				0xe7d3fbc8
#define AC25				0x21e1cde6
#define AC26				0xc33707d6
#define AC27				0xf4d50d87
#define AC28				0x455a14ed
#define AC29				0xa9e3e905
#define AC30				0xfcefa3f8
#define AC31				0x676f02d9
#define AC32				0x8d2a4c8a
#define AC33				0xfffa3942
#define AC34				0x8771f681
#define AC35				0x6d9d6122
#define AC36				0xfde5380c
#define AC37				0xa4beea44
#define AC38				0x4bdecfa9
#define AC39				0xf6bb4b60
#define AC40				0xbebfbc70
#define AC41				0x289b7ec6
#define AC42				0xeaa127fa
#define AC43				0xd4ef3085
#define AC44				0x04881d05
#define AC45				0xd9d4d039
#define AC46				0xe6db99e5
#define AC47				0x1fa27cf8
#define AC48				0xc4ac5665
#define AC49				0xf4292244
#define AC50				0x432aff97
#define AC51				0xab9423a7
#define AC52				0xfc93a039
#define AC53				0x655b59c3
#define AC54				0x8f0ccc92
#define AC55				0xffeff47d
#define AC56				0x85845dd1
#define AC57				0x6fa87e4f
#define AC58				0xfe2ce6e0
#define AC59				0xa3014314
#define AC60				0x4e0811a1
#define AC61				0xf7537e82
#define AC62				0xbd3af235
#define AC63				0x2ad7d2bb
#define AC64				0xeb86d391
#define MASK1				0x77777777

#else

/*
 * If we used immediate values on RISC with 32 bit instruction size, it would
 * take 130 instructions to load all the 65 values. Keeping the values in an
 * array instead reduces the instruction count to 67.
 */

static MD5_word MD5_AC[65] = {
	0xd76aa477, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
	0x77777777
};

#define AC1				MD5_AC[0]
#define AC2				MD5_AC[1]
#define AC3				MD5_AC[2]
#define AC4				MD5_AC[3]
#define AC5				MD5_AC[4]
#define AC6				MD5_AC[5]
#define AC7				MD5_AC[6]
#define AC8				MD5_AC[7]
#define AC9				MD5_AC[8]
#define AC10				MD5_AC[9]
#define AC11				MD5_AC[10]
#define AC12				MD5_AC[11]
#define AC13				MD5_AC[12]
#define AC14				MD5_AC[13]
#define AC15				MD5_AC[14]
#define AC16				MD5_AC[15]
#define AC17				MD5_AC[16]
#define AC18				MD5_AC[17]
#define AC19				MD5_AC[18]
#define AC20				MD5_AC[19]
#define AC21				MD5_AC[20]
#define AC22				MD5_AC[21]
#define AC23				MD5_AC[22]
#define AC24				MD5_AC[23]
#define AC25				MD5_AC[24]
#define AC26				MD5_AC[25]
#define AC27				MD5_AC[26]
#define AC28				MD5_AC[27]
#define AC29				MD5_AC[28]
#define AC30				MD5_AC[29]
#define AC31				MD5_AC[30]
#define AC32				MD5_AC[31]
#define AC33				MD5_AC[32]
#define AC34				MD5_AC[33]
#define AC35				MD5_AC[34]
#define AC36				MD5_AC[35]
#define AC37				MD5_AC[36]
#define AC38				MD5_AC[37]
#define AC39				MD5_AC[38]
#define AC40				MD5_AC[39]
#define AC41				MD5_AC[40]
#define AC42				MD5_AC[41]
#define AC43				MD5_AC[42]
#define AC44				MD5_AC[43]
#define AC45				MD5_AC[44]
#define AC46				MD5_AC[45]
#define AC47				MD5_AC[46]
#define AC48				MD5_AC[47]
#define AC49				MD5_AC[48]
#define AC50				MD5_AC[49]
#define AC51				MD5_AC[50]
#define AC52				MD5_AC[51]
#define AC53				MD5_AC[52]
#define AC54				MD5_AC[53]
#define AC55				MD5_AC[54]
#define AC56				MD5_AC[55]
#define AC57				MD5_AC[56]
#define AC58				MD5_AC[57]
#define AC59				MD5_AC[58]
#define AC60				MD5_AC[59]
#define AC61				MD5_AC[60]
#define AC62				MD5_AC[61]
#define AC63				MD5_AC[62]
#define AC64				MD5_AC[63]
#define MASK1				MD5_AC[64]

#endif

/*
 * F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z)			((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z)			((y) ^ ((z) & ((x) ^ (y))))
#define H(x, y, z)			((x) ^ (y) ^ (z))
#define I(x, y, z)			((y) ^ ((x) | ~(z)))

/*
 * ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) \
	(x) = (((x) << (n)) | ((x) >> (32 - (n))))

/*
 * FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
 * Rotation is separate from addition to prevent recomputation.
 */

#define FF(a, b, c, d, x, s, ac) \
	(a) += F ((b), (c), (d)) + (x) + (ac); \
	ROTATE_LEFT ((a), (s)); \
	(a) += (b);

#define GG(a, b, c, d, x, s, ac) \
	(a) += G ((b), (c), (d)) + (x) + (ac); \
	ROTATE_LEFT ((a), (s)); \
	(a) += (b);

#define HH(a, b, c, d, x, s, ac) \
	(a) += H ((b), (c), (d)) + (x) + (ac); \
	ROTATE_LEFT ((a), (s)); \
	(a) += (b);

#define II(a, b, c, d, x, s, ac) \
	(a) += I ((b), (c), (d)) + (x) + (ac); \
	ROTATE_LEFT ((a), (s)); \
	(a) += (b);

static void MD5_body(MD5_word x[15], MD5_word out[4])
{
	register MD5_word a, b = Cb, c = Cc, d = Cd;

/* Round 1 */
	a = AC1 + x[0]; ROTATE_LEFT (a, S11); a += b;	/* 1 */
	d += (c ^ (a & MASK1)) + x[1] + AC2;
	ROTATE_LEFT (d, S12); d += a;			/* 2 */
	FF (c, d, a, b, x[ 2], S13, AC3);		/* 3 */
	FF (b, c, d, a, x[ 3], S14, AC4);		/* 4 */
	FF (a, b, c, d, x[ 4], S11, AC5);		/* 5 */
	FF (d, a, b, c, x[ 5], S12, AC6);		/* 6 */
	FF (c, d, a, b, x[ 6], S13, AC7);		/* 7 */
	FF (b, c, d, a, x[ 7], S14, AC8);		/* 8 */
	FF (a, b, c, d, x[ 8], S11, AC9);		/* 9 */
	FF (d, a, b, c, x[ 9], S12, AC10);		/* 10 */
	FF (c, d, a, b, x[10], S13, AC11);		/* 11 */
	FF (b, c, d, a, x[11], S14, AC12);		/* 12 */
	FF (a, b, c, d, x[12], S11, AC13);		/* 13 */
	FF (d, a, b, c, x[13], S12, AC14);		/* 14 */
	FF (c, d, a, b, x[14], S13, AC15);		/* 15 */
	b += F (c, d, a) + AC16;
	ROTATE_LEFT (b, S14); b += c;			/* 16 */

/* Round 2 */
	GG (a, b, c, d, x[ 1], S21, AC17);		/* 17 */
	GG (d, a, b, c, x[ 6], S22, AC18);		/* 18 */
	GG (c, d, a, b, x[11], S23, AC19);		/* 19 */
	GG (b, c, d, a, x[ 0], S24, AC20);		/* 20 */
	GG (a, b, c, d, x[ 5], S21, AC21);		/* 21 */
	GG (d, a, b, c, x[10], S22, AC22);		/* 22 */
	c += G (d, a, b) + AC23;
	ROTATE_LEFT (c, S23); c += d;			/* 23 */
	GG (b, c, d, a, x[ 4], S24, AC24);		/* 24 */
	GG (a, b, c, d, x[ 9], S21, AC25);		/* 25 */
	GG (d, a, b, c, x[14], S22, AC26);		/* 26 */
	GG (c, d, a, b, x[ 3], S23, AC27);		/* 27 */
	GG (b, c, d, a, x[ 8], S24, AC28);		/* 28 */
	GG (a, b, c, d, x[13], S21, AC29);		/* 29 */
	GG (d, a, b, c, x[ 2], S22, AC30);		/* 30 */
	GG (c, d, a, b, x[ 7], S23, AC31);		/* 31 */
	GG (b, c, d, a, x[12], S24, AC32);		/* 32 */

/* Round 3 */
	HH (a, b, c, d, x[ 5], S31, AC33);		/* 33 */
	HH (d, a, b, c, x[ 8], S32, AC34);		/* 34 */
	HH (c, d, a, b, x[11], S33, AC35);		/* 35 */
	HH (b, c, d, a, x[14], S34, AC36);		/* 36 */
	HH (a, b, c, d, x[ 1], S31, AC37);		/* 37 */
	HH (d, a, b, c, x[ 4], S32, AC38);		/* 38 */
	HH (c, d, a, b, x[ 7], S33, AC39);		/* 39 */
	HH (b, c, d, a, x[10], S34, AC40);		/* 40 */
	HH (a, b, c, d, x[13], S31, AC41);		/* 41 */
	HH (d, a, b, c, x[ 0], S32, AC42);		/* 42 */
	HH (c, d, a, b, x[ 3], S33, AC43);		/* 43 */
	HH (b, c, d, a, x[ 6], S34, AC44);		/* 44 */
	HH (a, b, c, d, x[ 9], S31, AC45);		/* 45 */
	HH (d, a, b, c, x[12], S32, AC46);		/* 46 */
	c += H (d, a, b) + AC47;
	ROTATE_LEFT (c, S33); c += d;			/* 47 */
	HH (b, c, d, a, x[ 2], S34, AC48);		/* 48 */

/* Round 4 */
	II (a, b, c, d, x[ 0], S41, AC49);		/* 49 */
	II (d, a, b, c, x[ 7], S42, AC50);		/* 50 */
	II (c, d, a, b, x[14], S43, AC51);		/* 51 */
	II (b, c, d, a, x[ 5], S44, AC52);		/* 52 */
	II (a, b, c, d, x[12], S41, AC53);		/* 53 */
	II (d, a, b, c, x[ 3], S42, AC54);		/* 54 */
	II (c, d, a, b, x[10], S43, AC55);		/* 55 */
	II (b, c, d, a, x[ 1], S44, AC56);		/* 56 */
	II (a, b, c, d, x[ 8], S41, AC57);		/* 57 */
	d += I (a, b, c) + AC58;
	ROTATE_LEFT (d, S42); d += a;			/* 58 */
	II (c, d, a, b, x[ 6], S43, AC59);		/* 59 */
	II (b, c, d, a, x[13], S44, AC60);		/* 60 */
	II (a, b, c, d, x[ 4], S41, AC61);		/* 61 */
	II (d, a, b, c, x[11], S42, AC62);		/* 62 */
	II (c, d, a, b, x[ 2], S43, AC63);		/* 63 */
	II (b, c, d, a, x[ 9], S44, AC64);		/* 64 */

	out[0] = Ca + a;
	out[1] = Cb + b;
	out[2] = Cc + c;
	out[3] = Cd + d;
}

#else

extern void MD5_body(MD5_word x[15], MD5_word out[4]);

#endif

#if ARCH_LITTLE_ENDIAN

#define MD5_swap(x, y, count)

#else

static void MD5_swap(MD5_word *x, MD5_word *y, int count)
{
	MD5_word tmp;

	do {
		tmp = *x++;
		ROTATE_LEFT(tmp, 16);
		*y++ = ((tmp & 0x00FF00FF) << 8) | ((tmp >> 8) & 0x00FF00FF);
	} while (--count);
}

#endif

static unsigned char PADDING[56] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static struct {
	char s[8];
	struct {
		int p, s, ps, pp, psp;
	} l;
	struct {
		MD5_block p, sp, pp, spp;
	} e;
	struct {
		MD5_block p, ps, pp, psp;
	} o;
} patterns;

struct pattern {
	int length;
	MD5_block *even, *odd;
};

static struct pattern order[21] = {
	{0, &patterns.e.p, &patterns.o.psp},
	{0, &patterns.e.spp, &patterns.o.pp},
	{0, &patterns.e.spp, &patterns.o.psp},
	{0, &patterns.e.pp, &patterns.o.ps},
	{0, &patterns.e.spp, &patterns.o.pp},
	{0, &patterns.e.spp, &patterns.o.psp},
	{0, &patterns.e.pp, &patterns.o.psp},
	{0, &patterns.e.sp, &patterns.o.pp},
	{0, &patterns.e.spp, &patterns.o.psp},
	{0, &patterns.e.pp, &patterns.o.psp},
	{0, &patterns.e.spp, &patterns.o.p},
	{0, &patterns.e.spp, &patterns.o.psp},
	{0, &patterns.e.pp, &patterns.o.psp},
	{0, &patterns.e.spp, &patterns.o.pp},
	{0, &patterns.e.sp, &patterns.o.psp},
	{0, &patterns.e.pp, &patterns.o.psp},
	{0, &patterns.e.spp, &patterns.o.pp},
	{0, &patterns.e.spp, &patterns.o.ps},
	{0, &patterns.e.pp, &patterns.o.psp},
	{0, &patterns.e.spp, &patterns.o.pp},
	{0, &patterns.e.spp, &patterns.o.psp},
};

void MD5_std_set_salt(char *salt)
{
	int length;

	for (length = 0; salt[length] && length < 8; length++);

	memcpy(patterns.s, salt, patterns.l.s = length);
}

void MD5_std_set_key(char *key)
{
	int length;

	for (length = 0; key[length] && length < 15; length++);

	memcpy(patterns.o.p.b, key, patterns.l.p = length);
	memcpy(&patterns.o.p.b[length + 16], PADDING, 40 - length);
	patterns.o.p.w[14] = (length + 16) << 3;

	memcpy(patterns.o.pp.b, key, length);
	memcpy(&patterns.o.pp.b[length], key, length);
	patterns.l.pp = length << 1;
	memcpy(&patterns.o.pp.b[patterns.l.pp + 16], PADDING,
		40 - patterns.l.pp);
	patterns.o.pp.w[14] = (patterns.l.pp + 16) << 3;

	memcpy(&patterns.e.p.b[16], key, length);
	memcpy(&patterns.e.p.b[16 + length], PADDING, 40 - length);
	patterns.e.p.w[14] = (length + 16) << 3;
	MD5_swap(patterns.e.p.w, patterns.e.p.w, 14);

	memcpy(&patterns.e.pp.b[16], patterns.o.pp.b, patterns.l.pp);
	memcpy(&patterns.e.pp.b[16 + patterns.l.pp], PADDING,
		40 - patterns.l.pp);
	patterns.e.pp.w[14] = (patterns.l.pp + 16) << 3;
	MD5_swap(patterns.e.pp.w, patterns.e.pp.w, 14);

	order[1].length = patterns.l.pp;
	order[4].length = patterns.l.pp;
	order[7].length = patterns.l.pp;
	order[10].length = length;
	order[13].length = patterns.l.pp;
	order[16].length = patterns.l.pp;
	order[19].length = patterns.l.pp;
}

void MD5_std_crypt()
{
	MD5_block block;
	int length, index;
	struct pattern *line;
#if ARCH_LITTLE_ENDIAN
	MD5_word *last;
#endif

	memcpy(patterns.o.ps.b, patterns.o.p.b, patterns.l.p);
	memcpy(&patterns.o.ps.b[patterns.l.p], patterns.s, patterns.l.s);
	patterns.l.ps = patterns.l.p + patterns.l.s;
	memcpy(&patterns.o.ps.b[patterns.l.ps + 16], PADDING,
		40 - patterns.l.ps);
	patterns.o.ps.w[14] = (patterns.l.ps + 16) << 3;

	memcpy(patterns.o.psp.b, patterns.o.ps.b, patterns.l.ps);
	memcpy(&patterns.o.psp.b[patterns.l.ps], patterns.o.p.b, patterns.l.p);
	patterns.l.psp = patterns.l.ps + patterns.l.p;
	memcpy(&patterns.o.psp.b[patterns.l.psp + 16], PADDING,
		40 - patterns.l.psp);
	patterns.o.psp.w[14] = (patterns.l.psp + 16) << 3;

	memcpy(&patterns.e.sp.b[16], patterns.s, patterns.l.s);
	memcpy(&patterns.e.sp.b[16 + patterns.l.s], patterns.o.p.b,
		patterns.l.p);
	memcpy(&patterns.e.sp.b[16 + patterns.l.ps], PADDING,
		40 - patterns.l.ps);
	patterns.e.sp.w[14] = (patterns.l.ps + 16) << 3;
	MD5_swap(patterns.e.sp.w, patterns.e.sp.w, 14);

	memcpy(&patterns.e.spp.b[16], patterns.s, patterns.l.s);
	memcpy(&patterns.e.spp.b[16 + patterns.l.s], patterns.o.pp.b,
		patterns.l.pp);
	memcpy(&patterns.e.spp.b[16 + patterns.l.psp], PADDING,
		40 - patterns.l.psp);
	patterns.e.spp.w[14] = (patterns.l.psp + 16) << 3;
	MD5_swap(patterns.e.spp.w, patterns.e.spp.w, 14);

	order[0].length = patterns.l.psp;
	order[2].length = patterns.l.psp;
	order[3].length = patterns.l.ps;
	order[5].length = patterns.l.psp;
	order[6].length = patterns.l.psp;
	order[8].length = patterns.l.psp;
	order[9].length = patterns.l.psp;
	order[11].length = patterns.l.psp;
	order[12].length = patterns.l.psp;
	order[14].length = patterns.l.psp;
	order[15].length = patterns.l.psp;
	order[17].length = patterns.l.ps;
	order[18].length = patterns.l.psp;
	order[20].length = patterns.l.psp;

	memcpy(&block, patterns.o.psp.b, patterns.l.psp);
	memcpy(&block.b[patterns.l.psp], PADDING, 56 - patterns.l.psp);
	block.w[14] = patterns.l.psp << 3;
	MD5_swap(block.w, block.w, 14);
	MD5_body(block.w, MD5_out);
	MD5_swap(MD5_out, MD5_out, 4);

	memcpy(&block, patterns.o.p.b, patterns.l.p);
	memcpy(&block.b[patterns.l.p], "$1$", 3);
	memcpy(&block.b[patterns.l.p + 3], patterns.s, patterns.l.s);
	memcpy(&block.b[patterns.l.ps + 3], MD5_out, patterns.l.p);
	length = patterns.l.psp + 3;
	if ((index = patterns.l.p))
	do {
		block.b[length++] = (index & 1) ? 0 : patterns.o.p.b[0];
	} while (index >>= 1);
	memcpy(&block.b[length], PADDING, 56 - length);
	block.w[14] = length << 3;
	MD5_swap(block.w, block.w, 14);
	MD5_body(block.w, order[0].even->w);

	index = 500; line = order;
	do {
#if ARCH_LITTLE_ENDIAN
#if ARCH_ALLOWS_UNALIGNED
		MD5_body(line->even->w,
			(MD5_word *)&line->odd->b[line->length]);
#else
		MD5_body(line->even->w, MD5_out);
		memcpy(&line->odd->b[line->length], MD5_out, 16);
#endif
		last = line->odd->w;
		if (++line >= order + 21) line = order;
		MD5_body(last, line->even->w);
#else
		MD5_body(line->even->w, MD5_out);
		MD5_swap(MD5_out, MD5_out, 4);
		memcpy(&line->odd->b[line->length], MD5_out, 16);
		MD5_swap(line->odd->w, block.w, 14);
		block.w[14] = line->odd->w[14];
		if (++line >= order + 21) line = order;
		MD5_body(block.w, line->even->w);
#endif
	} while (--index);

	memcpy(MD5_out, line->even, 16);
}

char *MD5_std_get_salt(char *ciphertext)
{
	static char out[9];
	int length;

	for (length = 0; length < 8; length++)
	if ((out[length] = ciphertext[3 + length]) == '$') break;
	out[length] = 0;

	return out;
}

#define TO_BINARY(b1, b2, b3) \
	value = \
		(MD5_word)atoi64[(ARCH_INDEX)pos[0]] | \
		((MD5_word)atoi64[(ARCH_INDEX)pos[1]] << 6) | \
		((MD5_word)atoi64[(ARCH_INDEX)pos[2]] << 12) | \
		((MD5_word)atoi64[(ARCH_INDEX)pos[3]] << 18); \
	pos += 4; \
	out.b[b1] = value >> 16; \
	out.b[b2] = value >> 8; \
	out.b[b3] = value;

MD5_word *MD5_std_get_binary(char *ciphertext)
{
	static union {
		MD5_binary w;
		char b[16];
	} out;
	char *pos;
	MD5_word value;

	pos = ciphertext + 3; while (*pos++ != '$');

	TO_BINARY(0, 6, 12);
	TO_BINARY(1, 7, 13);
	TO_BINARY(2, 8, 14);
	TO_BINARY(3, 9, 15);
	TO_BINARY(4, 10, 5);
	out.b[11] =
		(MD5_word)atoi64[(ARCH_INDEX)pos[0]] |
		((MD5_word)atoi64[(ARCH_INDEX)pos[1]] << 6);

	MD5_swap(out.w, out.w, 4);

	return out.w;
}
