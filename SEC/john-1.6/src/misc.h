/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

/*
 * Miscellaneous routines.
 */

#ifndef _JOHN_MISC_H
#define _JOHN_MISC_H

#include <stdio.h>

/*
 * Exit on error. Terminates the process with non-zero exit status.
 */
extern void error();

/*
 * Similar to perror(), but supports formatted output, and calls error().
 */
extern void pexit(char *format, ...)
#ifdef __GNUC__
	__attribute__ ((format (printf, 1, 2)));
#else
	;
#endif

/*
 * Attempts to write all the supplied data. Returns the number of bytes
 * written, or -1 on error.
 */
extern int write_loop(int fd, char *buffer, int count);

/*
 * Similar to fgets(), but doesn't leave the newline character in the buffer,
 * and skips to the end of long lines. Handles both UNIX and DOS style text
 * files correctly.
 */
extern char *fgetl(char *s, int size, FILE *stream);

/*
 * Similar to strncpy(), but terminates with only one null if there's room
 * instead of null padding to the supplied size like strncpy() does.
 */
extern char *strnfcpy(char *dst, char *src, int size);

/*
 * Similar to the above, but always null terminates the string.
 */
extern char *strnzcpy(char *dst, char *src, int size);

/*
 * Similar to strncat(), but total buffer size is supplied, and always null
 * terminates the string.
 */
extern char *strnzcat(char *dst, char *src, int size);

/*
 * Converts a string to lowercase.
 */
extern char *strlwr(char *s);

#endif
