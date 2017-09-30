/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

/*
 * Architecture specific parameters for Alpha with slow L2 cache.
 */

#ifndef _JOHN_ARCH_H
#define _JOHN_ARCH_H

#define ARCH_GENERIC			0
#define ARCH_WORD			long
#define ARCH_SIZE			8
#define ARCH_BITS			64
#define ARCH_BITS_STR			"64"
#define ARCH_LITTLE_ENDIAN		1
#define ARCH_INT_GT_32			0
#define ARCH_ALLOWS_UNALIGNED		0
#define ARCH_INDEX			unsigned long

#define OS_TIMER			1
#define OS_FLOCK			1

#define CPU_DETECT			0

#define DES_ASM				1
#define DES_128K			0
#define DES_X2				0
#define DES_MASK			1
#define DES_SCALE			1
#define DES_EXTB			1
#define DES_COPY			0
#define DES_BS				0

#define MD5_ASM				0
#define MD5_IMM				0

#define BF_ASM				0
#define BF_SCALE			1

#endif
