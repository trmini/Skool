/*
# This program was written by and is copyright Alec Muffett 1991,
# 1992, 1993, 1994, 1995, and 1996, and is provided as part of the
# Crack v5.0 Password Cracking package.
#
# The copyright holder disclaims all responsibility or liability with
# respect to its usage or its effect upon hardware or computer
# systems, and maintains copyright as set out in the "LICENCE"
# document which accompanies distributions of Crack v5.0 and upwards.
*/

#include "libcrack.h"

/* this file implements a traditional crypt() backend for the
   (hopefully) platform-independent API for Crack version 5.0+ */

#define CIPHERTEXTSIZE  13
#define PLAINTEXTSIZE   8       /* must be <= MAXWORDLEN in libcrack.h */
#define SALTSIZE 2

static char private_salt[SALTSIZE + 1];

#undef CRYPT16

#undef CRYPTALG
#ifdef FCRYPT
#define CRYPTALG fcrypt
#endif /* FCRYPT */
#ifdef CRYPT16
#define CRYPTALG fcrypt
#endif /* CRYPT16 */
#ifndef CRYPTALG /* fall thry default */
#define CRYPTALG crypt
#endif /* CRYPTALG */

#define EBS             1024    /* BIG number - and why not ? */

/* boot hook */

int
elcid_init()
{
#ifdef INITDES
    init_des();
#endif /* INITDES */
    return (0);
}

/* optimum/max number of words 'N' for elcid_test */

int
elcid_bsize()
{
    return (EBS);               /* arbitrary value reducing memfrag */
}

/* maximum valid length of a guess */

int
elcid_pwsize()
{
    return (PLAINTEXTSIZE);
}

int
elcid_mkcookies(ciphertexts, usernames, cookies, m)
char **ciphertexts;             /* vector of M ciphertexts */
char **usernames;               /* vector of M usernames */
void **cookies;                 /* vector to contain M cookies */
int m;                          /* M */
{
    while (m--)
    {
	cookies[m] = (void *) Clone(ciphertexts[m]);
    }
    return (0);
}

int
elcid_setup(ciphertexts, m)
char **ciphertexts;             /* vector of M ciphertexts */
int m;                          /* not used in this implementation */
{
    if (ciphertexts[0])
    {
	strncpy(private_salt, ciphertexts[0], SALTSIZE);
	private_salt[SALTSIZE] = '\0';
	return (0);
    }

    fprintf(stderr, "elcid: salt not set (this shouldn't happen)\n");

    exit(1);

    return (-1);                /* swtf? */
}

int
elcid_invalid(ciphertext)       /* check validity of a ciphertext */
char *ciphertext;
{
    int i;
    int retval;
    static char sixtyfour[] =
    "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    /* I *have* seen user-generated salts outside the 'legal' range,
     * (hence the test below starts at '2') however an asterisk is
     * usually fatal; pray to god that there're no hash collisions,
     * since in the eyes of Morris' salt routines, '*'=='w' and hence
     * a 13-char ciphertext prefixed with a '*' is equivalent to the
     * same ciphertext shifted right one, prefixed with a 'w'. */

    /* to date, I know of no such instances - alecm */

    if (ciphertext[0] == '*')
    {
	return (-1);
    }

    if (strlen(ciphertext) < CIPHERTEXTSIZE)
    {
	return (-1);
    }

    retval = 0;

    for (i = 2; i < CIPHERTEXTSIZE && ciphertext[i]; i++)
    {
	if (!strchr(sixtyfour, (int) ciphertext[i]))
	{
	    retval++;
	}
    }

    return (retval);
}

int
elcid_test(guesses, cookies, results, cookiecount)
char **guesses;                 /* vector of N guesses */
void **cookies;                 /* vector of M cookies */
char **results;                 /* vector of M results (guessed passwords) */
int cookiecount;                /* M */
{
    int i;
    int retval;

    /* clear the log */
    retval = 0;

    /* for each element in the "guesses" array */
    for (i = 0; (i < EBS) && guesses[i]; i++)
    {
	int j;
	char *cipher;

	/* encrypt it */
	cipher = (char *) CRYPTALG(guesses[i], private_salt);

	/* and see if the encryption matches any of the cookies */
	for (j = 0; j < cookiecount; j++)
	{
	    /* skip, if there's no cookie, or it's been guessed */
	    if ((!cookies[j]) || results[j])
	    {
		continue;
	    }

	    /* compare, and clone the guessed word if correct */
	    if (!strncmp(cipher, (char *) cookies[j], CIPHERTEXTSIZE))
	    {
		results[j] = (char *) Clone(guesses[i]);
		retval++;
	    }
	}
    }

    return (retval);
}
