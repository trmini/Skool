/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

#include <string.h>

#include "arch.h"
#include "DES_std.h"
#include "common.h"
#include "formats.h"

#define FORMAT_LABEL			"lm"
#define FORMAT_NAME			"NT LM DES"

#define BENCHMARK_COMMENT		""
#define BENCHMARK_LENGTH		-1

#define PLAINTEXT_LENGTH		7
#define CIPHERTEXT_LENGTH		32

#define LM_EMPTY			"AAD3B435B51404EE"

static struct fmt_tests tests[] = {
	{"$LM$1C3A2B6D939A1021", "AAA"},
	{"$LM$CBC501A4D2227783", "AAAAAAA"},
	{"$LM$CE045FF77F9D89E2", "ZZZZ"},
	{LM_EMPTY LM_EMPTY, ""},
	{"$LM$7584248B8D2C9F9E", "A"},
	{NULL}
};

#define ALGORITHM_NAME			DES_STD_ALGORITHM_NAME

#define BINARY_SIZE			ARCH_SIZE
#define SALT_SIZE			0

#define MIN_KEYS_PER_CRYPT		0x40
#define MAX_KEYS_PER_CRYPT		0x40

#define LM_IV_0				0x2153474B
#define LM_IV_1				0x25242340

static struct {
	union {
		double dummy;
		DES_binary binary;
	} aligned;
	char key[PLAINTEXT_LENGTH + 1];
} buffer[MAX_KEYS_PER_CRYPT];

static DES_binary LM_IV;

static void init()
{
	DES_std_init();
	DES_std_set_salt(0);
	DES_count = 1;

	DES_std_set_block(LM_IV_0, LM_IV_1);
	memcpy(LM_IV, DES_IV, sizeof(DES_binary));
}

static int valid(char *ciphertext)
{
	char *pos;

	for (pos = ciphertext; atoi16[(ARCH_INDEX)*pos] != 0x7F; pos++);
	if (!*pos && pos - ciphertext == CIPHERTEXT_LENGTH) {
		if (strcmp(&ciphertext[16], LM_EMPTY))
			return 2;
		else
			return 1;
	}

	if (strncmp(ciphertext, "$LM$", 4)) return 0;

	for (pos = &ciphertext[4]; atoi16[(ARCH_INDEX)*pos] != 0x7F; pos++);
	if (*pos || pos - ciphertext != 20) return 0;

	return 1;
}

static char *split(char *ciphertext, int index)
{
	static char out[21];

	if (!strncmp(ciphertext, "$LM$", 4)) return ciphertext;

	out[0] = '$';
	out[1] = 'L';
	out[2] = 'M';
	out[3] = '$';

	if (index)
		memcpy(&out[4], &ciphertext[16], 16);
	else
		memcpy(&out[4], ciphertext, 16);

	out[20] = 0;
	return out;
}

static void *get_binary(char *ciphertext)
{
	static DES_binary out;
	ARCH_WORD_32 block[2];
	int index, value;

	block[0] = block[1] = 0;
	for (index = 0; index < 16; index += 2) {
		value = (int)atoi16[(ARCH_INDEX)ciphertext[index + 4]] << 4;
		value |= atoi16[(ARCH_INDEX)ciphertext[index + 5]];

		block[index >> 3] |= value << ((index << 2) & 0x18);
	}

	DES_std_set_block(block[0], block[1]);
	memcpy(out, DES_IV, sizeof(DES_binary));

	return out;
}

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
	return DES_STD_HASH_0(buffer[index].aligned.binary[0]);
}

static int get_hash_1(int index)
{
	ARCH_WORD binary;

	binary = buffer[index].aligned.binary[0];
	return DES_STD_HASH_1(binary);
}

static int get_hash_2(int index)
{
	ARCH_WORD binary;

	binary = buffer[index].aligned.binary[0];
	return DES_STD_HASH_2(binary);
}

static void set_salt(void *salt)
{
}

static void crypt_all(int count)
{
	int index;
	unsigned char *key7;
	union {
		char b[8];
		ARCH_WORD_32 w[2];
	} key8;

	for (index = 0; index < count; index++) {
		key7 = (unsigned char *)buffer[index].key;
		key8.b[0] = key7[0] >> 1;
		key8.b[1] = ((key7[0] & 0x01) << 6) | (key7[1] >> 2);
		key8.b[2] = ((key7[1] & 0x03) << 5) | (key7[2] >> 3);
		key8.b[3] = ((key7[2] & 0x07) << 4) | (key7[3] >> 4);
		key8.b[4] = ((key7[3] & 0x0F) << 3) | (key7[4] >> 5);
		key8.b[5] = ((key7[4] & 0x1F) << 2) | (key7[5] >> 6);
		key8.b[6] = ((key7[5] & 0x3F) << 1) | (key7[6] >> 7);
		key8.b[7] = key7[6];
		key8.w[0] |= 0x80808080;
		key8.w[1] |= 0x80808080;

		DES_std_set_key(key8.b);

		DES_IV[0] = LM_IV[0];
		DES_IV[1] = LM_IV[1];
#if ARCH_BITS < 64
		DES_IV[2] = LM_IV[2];
		DES_IV[3] = LM_IV[3];
#endif

		DES_std_crypt(DES_KS_current, buffer[index].aligned.binary);
	}
}

static int cmp_all(void *binary, int count)
{
	int index;

	for (index = 0; index < count; index++)
	if (*(unsigned ARCH_WORD *)binary == buffer[index].aligned.binary[0])
		return 1;

	return 0;
}

static int cmp_one(void *binary, int index)
{
	return *(unsigned ARCH_WORD *)binary ==
		buffer[index].aligned.binary[0];
}

static int cmp_exact(char *source, int index)
{
	ARCH_WORD *binary;
	int word;

	binary = (ARCH_WORD *)get_binary(source);

	for (word = 0; word < 16 / DES_SIZE; word++)
	if ((unsigned ARCH_WORD)binary[word] !=
	    buffer[index].aligned.binary[word])
		return 0;

	return 1;
}

static void set_key(char *key, int index)
{
	char *dst = buffer[index].key;
	char *src = key;
	int count = PLAINTEXT_LENGTH;

	do {
		if (*src >= 'a' && *src <= 'z')
			*dst++ = *src++ & ~0x20;
		else
			if ((*dst++ = *src)) src++;
	} while (--count);
	*dst = 0;
}

static char *get_key(int index)
{
	return buffer[index].key;
}

struct fmt_main fmt_LM = {
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
		FMT_8_BIT,
		tests
	}, {
		init,
		valid,
		split,
		get_binary,
		fmt_default_salt,
		{
			binary_hash_0,
			binary_hash_1,
			binary_hash_2
		},
		fmt_default_salt_hash,
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
