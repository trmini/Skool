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

int
Debug (level, fmt, arg1, arg2, arg3, arg4, arg5, arg6)
    int level;
    char *fmt;
    char *arg1, *arg2, *arg3, *arg4, *arg5, *arg6;
{
    fprintf (stderr, fmt, arg1, arg2, arg3, arg4, arg5, arg6);

    if (level < 0)
    {
	exit (0);
    }
    return (0);
}
