/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

#include <stdio.h>
#include <string.h>

#include "params.h"
#include "formats.h"

struct fmt_main *fmt_list = NULL;
static struct fmt_main **fmt_tail = &fmt_list;

void fmt_register(struct fmt_main *format)
{
	format->private.initialized = 0;
	format->next = NULL;
	*fmt_tail = format;
	fmt_tail = &format->next;
}

void fmt_init(struct fmt_main *format)
{
	if (!format->private.initialized) {
		format->methods.init();
		format->private.initialized = 1;
	}
}

int fmt_self_test(struct fmt_main *format)
{
	struct fmt_tests *current;
	char *ciphertext, *plaintext;
	int size;
	void *binary, *salt;

	if (format->params.plaintext_length > PLAINTEXT_BUFFER_SIZE - 3)
		return 1;

	if (format->methods.valid("*")) return 1;

	fmt_init(format);

	if (!(current = format->params.tests)) return 0;

	do {
		if (format->methods.valid(current->ciphertext) != 1) break;
		ciphertext = format->methods.split(current->ciphertext, 0);
		plaintext = current->plaintext;

		binary = format->methods.binary(ciphertext);
		salt = format->methods.salt(ciphertext);

		if ((unsigned int)format->methods.salt_hash(salt) >=
			SALT_HASH_SIZE) break;

		format->methods.set_salt(salt);
		format->methods.set_key(current->plaintext, 0);

		format->methods.crypt_all(1);

		for (size = 0; size < 3; size++)
		if (format->methods.get_hash[size](0) !=
			format->methods.binary_hash[size](binary)) break;
		if (size < 3) break;

		if (!format->methods.cmp_all(binary, 1)) break;
		if (!format->methods.cmp_one(binary, 0)) break;
		if (!format->methods.cmp_exact(ciphertext, 0)) break;

		if (strncmp(format->methods.get_key(0), plaintext,
			format->params.plaintext_length)) break;
	} while ((++current)->ciphertext);

	return current->ciphertext != NULL;
}

void fmt_default_init()
{
}

int fmt_default_valid(char *ciphertext)
{
	return 0;
}

char *fmt_default_split(char *ciphertext, int index)
{
	return ciphertext;
}

void *fmt_default_binary(char *ciphertext)
{
	return ciphertext;
}

void *fmt_default_salt(char *ciphertext)
{
	return ciphertext;
}

int fmt_default_binary_hash(void *binary)
{
	return 0;
}

int fmt_default_salt_hash(void *salt)
{
	return 0;
}

int fmt_default_get_hash(int index)
{
	return 0;
}
