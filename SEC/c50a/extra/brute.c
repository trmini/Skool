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

#include <stdio.h>
#include <stdlib.h>

#define MAXLEN 64

char *cipher;
char *pwset[MAXLEN];
char pwbuffer[MAXLEN];

void
brute(int charno, int length)
{
    char *p;
    int doit;

    if (charno == (length - 1))
    {
	doit = 1;
    }
    else
    {
	doit = 0;
    }

    pwbuffer[charno + 1] = '\0';

    if (!pwset[charno])
    {
	fprintf(stderr, "wtf?\n");
	exit(1);
    }

    for (p = pwset[charno]; *p; p++)
    {
	pwbuffer[charno] = *p;

	if (doit)
	{
#if BRUTEGEN
	    printf("%s\n", pwbuffer);
#else
	    if (!strcmp(cipher, (char *) crypt(pwbuffer, cipher)))
	    {
		printf("brute: guessed %s == '%s'\n", cipher, pwbuffer);
		exit(0);
	    }
#endif
	}
	else
	{
	    brute(charno + 1, length);
	}
    }
}

char *
expand(char *string)
{
    int i;
    int j;
    char *c;
    char *retval;
    int a;
    int b;
    char block[256];            /* std crypt() is restricted to 128, bwtf.  md5 aint. */

    for (i = 0; i < 256; i++)
    {
	block[i] = '\0';
    }

    c = string;

    while (*c)
    {
	i = 0;
	j = -1;

	if ((*c == '\\') && c[1])
	{
	    c++;
	    i = *c & 0xff;
	    c++;
	} else
	{
	    i = *c & 0xff;
	    c++;
	}

	if (*c == '-' && c[1])
	{
	    c++;

	    if ((*c == '\\') && c[1])
	    {
		c++;
		j = *c & 0xff;
		c++;
	    } else
	    {
		j = *c & 0xff;
		c++;
	    }
	}

	if (i < j)
	{
	    while (i <= j)
	    {
		block[i++] = 1;
	    }
	}
	else
	{
	    block[i] = 1;	    
	}
    }

    j = 0;

    for (i = 0; i < 256; i++)
    {
	if (block[i])
	{
	    j++;
	}
    }

    if (!j)
    {
	fprintf(stderr, "brute: cannot permit empty set: %s\n", string);
	exit(1);
    }

    c = retval = malloc(j + 1);

    for (i = 0; i < 256; i++)
    {
	if (block[i])
	{
	    *(c++) = i;
	}
    }

    *c = '\0';

    return retval;
}

/* brute ciphertext '[set]' ... */

main(int argc, char **argv)
{
    int i;
    int j;

#if BRUTEGEN
    if (argc < 2)
    {
	fprintf(stderr, "Usage:\t%s charset ...\n", argv[0]);
	exit(1);
    }

    i = 1;
#else
    if (argc < 3)
    {
	fprintf(stderr, "Usage:\t%s ciphertext charset ...\n", argv[0]);
	exit(1);
    }

    cipher = argv[1];
    fprintf(stderr, "brute: ciphertext: %s\n", cipher);

    i = 2;
#endif

    j = 0;
    while ((i < argc) && (i < (MAXLEN - 1)))
    {
	fprintf(stderr, "brute: set %d: %s -> ", j, argv[i]);
	pwset[j] = expand(argv[i]);
	fprintf(stderr, "'%s'\n", pwset[j]);
	i++;
	j++;
    }

    pwset[j] = (char *) 0;

    brute(0, j);

#if BRUTEGEN
    exit(0);
#else
    exit(1); /* if cracking, exit with error unless cracked */
#endif
}
