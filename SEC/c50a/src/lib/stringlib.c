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

char
Chop(string)
char *string;
{
    char c;
    char *ptr;

    c = '\0';

    for (ptr = string; *ptr; ptr++);

    if (ptr != string)
    {
	c = *(--ptr);
	*ptr = '\0';
    }

    return (c);
}

char
ChopNL(string)
char *string;
{
    char c;
    char *ptr;

    c = '\0';

    for (ptr = string; *ptr; ptr++);

    if (ptr != string)
    {
	c = *(--ptr);

	if (c == '\n')
	{
	    *ptr = '\0';
	}
    }

    return (c);
}

char *
Trim(string)
char *string;
{
    char *ptr;

    for (ptr = string; *ptr; ptr++);

    while ((--ptr >= string) && isspace(*ptr));

    *(++ptr) = '\0';

    return (ptr);
}

/*
 * The one positive thing that comes out of three years of studying
 * astronomy at UCL is that the graduate leaves knowing 14 different
 * ways of writing the letter 'D'
 */

char *
Clone(string)
char *string;
{
    char *retval;

    retval = (char *) malloc(strlen(string) + 1);

    if (retval)
    {
	strcpy(retval, string);
    }

    return (retval);
}

char **
SplitOn(c, ptr, maxf)
char c;
char *ptr;
int maxf;
{
    int i;
    static char *retval[STRINGSIZE];	/* worst case scenario */

    for (i = 0; i < STRINGSIZE; i++)
    {
	retval[i] = ptr;

	if ((--maxf) <= 0)
	{
	    break;
	}

	while (*ptr && (*ptr != c))
	{
	    ptr++;
	}

	if (*ptr)
	{
	    *(ptr++) = '\0';
	} else
	{
	    break;
	}
    }

    /* so I can only have 255 element records. Fine by me. */

    if (++i < STRINGSIZE)
    {
	retval[i] = (char *) 0;
    } else
    {
	retval[STRINGSIZE - 1] = (char *) 0;
    }

    return (retval);
}
