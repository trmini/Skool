/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

#include <string.h>

#include "arch.h"
#include "DES_std.h"
#include "common.h"
#include "formats.h"

#define FORMAT_LABEL			"des"
#define FORMAT_NAME			"Standard DES"

#define BENCHMARK_COMMENT		""
#define BENCHMARK_LENGTH		0

#define PLAINTEXT_LENGTH		8
#define CIPHERTEXT_LENGTH_1		13
#define CIPHERTEXT_LENGTH_2		24

static struct fmt_tests tests[] = {
	{"CCNf8Sbh3HDfQ", "U*U*U*U*"},
	{"CCX.K.MFy4Ois", "U*U***U"},
	{"CC4rMpbg9AMZ.", "U*U***U*"},
	{"XXxzOu6maQKqQ", "*U*U*U*U"},
	{"SDbsugeBiC58A", ""},
	{NULL}
};

#if DES_BS

#include "memory.h"
#include "DES_bs.h"

#define ALGORITHM_NAME			DES_BS_ALGORITHM_NAME

#define BINARY_BITS			16

#define BINARY_SIZE			(BINARY_BITS * ARCH_SIZE)
#define SALT_SIZE			ARCH_SIZE

#define MIN_KEYS_PER_CRYPT		ARCH_BITS
#define MAX_KEYS_PER_CRYPT		ARCH_BITS

#else

#define ALGORITHM_NAME			DES_STD_ALGORITHM_NAME

#define BINARY_SIZE			ARCH_SIZE
#define SALT_SIZE			ARCH_SIZE

#define MIN_KEYS_PER_CRYPT		0x40
#if DES_128K
#define MAX_KEYS_PER_CRYPT		0x100
#else
#define MAX_KEYS_PER_CRYPT		0x80
#endif

#endif

static struct {
#if !DES_BS
	union {
		double dummy;
		struct {
			DES_KS KS;
			DES_binary binary;
		} data;
	} aligned;
#endif
	char key[PLAINTEXT_LENGTH];
} buffer[MAX_KEYS_PER_CRYPT];

#if DES_BS

struct fmt_main fmt_DES;
static int binary_bits = BINARY_BITS;
static int keys_changed;

static void init()
{
	if (mem_saving_level > 1) {
		DES_bs_mem_saving = 1;
		binary_bits = 32;
		fmt_DES.params.binary_size = ARCH_SIZE;
	}

	DES_bs_init();
}

#endif

static int valid(char *ciphertext)
{
	char *pos;

	if (!ciphertext[0] || !ciphertext[1]) return 0;

	for (pos = &ciphertext[2]; atoi64[(ARCH_INDEX)*pos] != 0x7F; pos++);
	if (*pos && *pos != ',') return 0;

	switch (pos - ciphertext) {
	case CIPHERTEXT_LENGTH_1:
		return 1;

	case CIPHERTEXT_LENGTH_2:
		return 2;

	default:
		return 0;
	}
}

static char *split(char *ciphertext, int index)
{
	static char out[14];

	if (index) {
		memcpy(out, &ciphertext[2], 2);
		memcpy(&out[2], &ciphertext[13], 11);
	} else
		memcpy(out, ciphertext, 13);

	out[13] = 0;
	return out;
}

static void *salt(char *ciphertext)
{
	static ARCH_WORD out;

#if DES_BS
	out = DES_raw_get_salt(ciphertext);
#else
	out = DES_std_get_salt(ciphertext);
#endif

	return &out;
}

#if DES_BS

static int binary_hash_0(void *binary)
{
	return DES_bs_binary_hash((ARCH_WORD *)binary, 4);
}

static int binary_hash_1(void *binary)
{
	return DES_bs_binary_hash((ARCH_WORD *)binary, 8);
}

static int binary_hash_2(void *binary)
{
	return DES_bs_binary_hash((ARCH_WORD *)binary, 12);
}

static int get_hash_0(int index)
{
	return DES_bs_get_hash(index, 4);
}

static int get_hash_1(int index)
{
	return DES_bs_get_hash(index, 8);
}

static int get_hash_2(int index)
{
	return DES_bs_get_hash(index, 12);
}

static int salt_hash(void *salt)
{
	return *(ARCH_WORD *)salt & 0x3FF;
}

static void set_salt(void *salt)
{
	DES_bs_set_salt(*(ARCH_WORD *)salt);
}

static void crypt_all(int count)
{
	int index;

	if (keys_changed) {
		DES_bs_clear_keys();
		for (index = 0; index < count; index++)
			DES_bs_set_key(buffer[index].key, index);
		keys_changed = 0;
	}

	DES_bs_crypt(25);
}

static int cmp_all(void *binary, int count)
{
	return DES_bs_cmp_all((ARCH_WORD *)binary, binary_bits);
}

static int cmp_one(void *binary, int index)
{
	return DES_bs_cmp_one((ARCH_WORD *)binary, binary_bits, index);
}

static int cmp_exact(char *source, int index)
{
	return DES_bs_cmp_one(DES_bs_get_binary(source), 64, index);
}

#else

static int binary_hash_0(void *binary)
{
	return DES_STD_HASH_0(*(ARCH_WORD *)binary);
}

static int binary_hash_1(void *binary)
{
	return DES_STD_HASH_1(*(ARCH_WORD *)binary);
}

static int binary_hash_2(void *binary)
{
	return DES_STD_HASH_2(*(ARCH_WORD *)binary);
}

static int get_hash_0(int index)
{
	return DES_STD_HASH_0(buffer[index].aligned.data.binary[0]);
}

static int get_hash_1(int index)
{
	ARCH_WORD binary;

	binary = buffer[index].aligned.data.binary[0];
	return DES_STD_HASH_1(binary);
}

static int get_hash_2(int index)
{
	ARCH_WORD binary;

	binary = buffer[index].aligned.data.binary[0];
	return DES_STD_HASH_2(binary);
}

static int salt_hash(void *salt)
{
	return DES_STD_HASH_2(*(ARCH_WORD *)salt) & 0x3FF;
}

static void set_salt(void *salt)
{
	DES_std_set_salt(*(ARCH_WORD *)salt);
}

static void crypt_all(int count)
{
	int index;

	for (index = 0; index < count; index++)
		DES_std_crypt(buffer[index].aligned.data.KS,
			buffer[index].aligned.data.binary);
}

static int cmp_all(void *binary, int count)
{
	int index;

	for (index = 0; index < count; index++)
	if (*(unsigned ARCH_WORD *)binary ==
	    (buffer[index].aligned.data.binary[0] & DES_BINARY_MASK))
		return 1;

	return 0;
}

static int cmp_one(void *binary, int index)
{
	return *(unsigned ARCH_WORD *)binary ==
		(buffer[index].aligned.data.binary[0] & DES_BINARY_MASK);
}

static int cmp_exact(char *source, int index)
{
	ARCH_WORD *binary;
	int word;

	binary = DES_std_get_binary(source);

	for (word = 0; word < 16 / DES_SIZE; word++)
	if ((unsigned ARCH_WORD)binary[word] !=
	    (buffer[index].aligned.data.binary[word] & DES_BINARY_MASK))
		return 0;

	return 1;
}

#endif

static void set_key(char *key, int index)
{
#if DES_BS
	keys_changed = 1;
#else
	DES_std_set_key(key);
	memcpy(buffer[index].aligned.data.KS, DES_KS_current, sizeof(DES_KS));
#endif

	memcpy(buffer[index].key, key, PLAINTEXT_LENGTH);
}

static char *get_key(int index)
{
	static char out[PLAINTEXT_LENGTH + 1];

	memcpy(out, buffer[index].key, PLAINTEXT_LENGTH);
	out[PLAINTEXT_LENGTH] = 0;

	return out;
}

struct fmt_main fmt_DES = {
	{
		FORMAT_LABEL,
		FORMAT_NAME,
		ALGORITHM_NAME,
		BENCHMARK_COMMENT,
		BENCHMARK_LENGTH,
		PLAINTEXT_LENGTH,
		BINARY_SIZE,
		SALT_SIZE,
		MIN_KEYS_PER_CRYPT,
		MAX_KEYS_PER_CRYPT,
		FMT_CASE,
		tests
	}, {
#if DES_BS
		init,
#else
		DES_std_init,
#endif
		valid,
		split,
		(void *(*)(char *))
#if DES_BS
			DES_bs_get_binary,
#else
			DES_std_get_binary,
#endif
		salt,
		{
			binary_hash_0,
			binary_hash_1,
			binary_hash_2
		},
		salt_hash,
		set_salt,
		set_key,
		get_key,
		crypt_all,
		{
			get_hash_0,
			get_hash_1,
			get_hash_2
		},
		cmp_all,
		cmp_one,
		cmp_exact
	}
};
