/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

/*
 * Architecture specific parameters for PowerPC.
 */

#ifndef _JOHN_ARCH_H
#define _JOHN_ARCH_H

#define ARCH_GENERIC			1
#define ARCH_WORD			long
#define ARCH_SIZE			4
#define ARCH_BITS			32
#define ARCH_BITS_STR			"32"
#define ARCH_LITTLE_ENDIAN		0
#define ARCH_INT_GT_32			0
#define ARCH_ALLOWS_UNALIGNED		0
#define ARCH_INDEX			unsigned int

#define OS_TIMER			1
#define OS_FLOCK			1

#define CPU_DETECT			0

#define DES_ASM				0
#define DES_128K			0
#define DES_X2				0
#define DES_MASK			1
#define DES_SCALE			0
#define DES_EXTB			1
#define DES_COPY			0
#define DES_BS				1

#define MD5_ASM				0
#define MD5_IMM				1

#define BF_ASM				0
#define BF_SCALE			0

#endif
