/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

/*
 * Architecture specific parameters detection program.
 */

#include <stdio.h>

int main(int argc, char **argv)
{
	int value = 1;

	if (argc != 6) return 1;

	puts(
"/*\n"
" * Architecture specific parameters. This is a generated file, do not edit.\n"
" */\n"
"\n"
"#ifndef _JOHN_ARCH_H\n"
"#define _JOHN_ARCH_H\n");

	printf(
"#define ARCH_GENERIC\t\t\t%d\n"
"#define ARCH_WORD\t\t\tlong\n"
"#define ARCH_SIZE\t\t\t%d\n"
"#define ARCH_BITS\t\t\t%d\n"
"#define ARCH_BITS_STR\t\t\t\"%d\"\n"
"#define ARCH_LITTLE_ENDIAN\t\t%d\n"
"#define ARCH_INT_GT_32\t\t\t%d\n"
"#define ARCH_ALLOWS_UNALIGNED\t\t0\n"
#ifdef __alpha__
"#define ARCH_INDEX\t\t\tunsigned long\n"
#else
"#define ARCH_INDEX\t\t\tunsigned int\n"
#endif
"\n"
#ifdef OS_TIMER
"#define OS_TIMER\t\t\t1\n"
#else
"#define OS_TIMER\t\t\t0\n"
#endif
#ifdef OS_FLOCK
"#define OS_FLOCK\t\t\t1\n"
#else
"#define OS_FLOCK\t\t\t0\n"
#endif
"\n",
#ifdef DES_ASM
		0,
#else
		1,
#endif
		(int)sizeof(long),
		(int)(sizeof(long) * 8),
		(int)(sizeof(long) * 8),
		(int)(*(char *)&value),
		(sizeof(int) > 4) ? 1 : 0);

	puts(
"#define CPU_DETECT\t\t\t0\n");

	printf(
"#define DES_ASM\t\t\t\t%d\n",
#ifdef DES_ASM
		1);
#else
		0);
#endif

	switch (argv[1][0]) {
	case '1':
		puts(
"#define DES_128K\t\t\t0\n"
"#define DES_X2\t\t\t\t0\n"
"#define DES_MASK\t\t\t0\n"
"#define DES_SCALE\t\t\t1\n"
"#define DES_EXTB\t\t\t0");
		break;

	case '2':
		puts(
"#define DES_128K\t\t\t1\n"
"#define DES_X2\t\t\t\t0\n"
"#define DES_MASK\t\t\t1\n"
"#define DES_SCALE\t\t\t1\n"
"#define DES_EXTB\t\t\t1");
		break;

	case '3':
		puts(
"#define DES_128K\t\t\t0\n"
"#define DES_X2\t\t\t\t0\n"
"#define DES_MASK\t\t\t1\n"
"#define DES_SCALE\t\t\t1\n"
"#define DES_EXTB\t\t\t0");
		break;

	case '4':
		puts(
"#define DES_128K\t\t\t0\n"
"#define DES_X2\t\t\t\t0\n"
"#define DES_MASK\t\t\t1\n"
"#define DES_SCALE\t\t\t1\n"
"#define DES_EXTB\t\t\t1");
		break;

	case '5':
		if (sizeof(long) >= 8) {
			puts(
"#define DES_128K\t\t\t0\n"
"#define DES_X2\t\t\t\t0\n"
"#define DES_MASK\t\t\t1\n"
"#define DES_SCALE\t\t\t0\n"
"#define DES_EXTB\t\t\t0");
			break;
		}

	default:
		return 1;
	}

	printf(
"#define DES_COPY\t\t\t%c\n"
"#define DES_BS\t\t\t\t%c\n"
"\n"
"#define MD5_ASM\t\t\t\t0\n"
"#define MD5_IMM\t\t\t\t%c\n"
"\n"
"#define BF_ASM\t\t\t\t0\n"
"#define BF_SCALE\t\t\t%c\n"
"\n"
"#endif\n",
		argv[2][0],
		argv[3][0],
		argv[4][0],
		argv[5][0]);

	return 0;
}
