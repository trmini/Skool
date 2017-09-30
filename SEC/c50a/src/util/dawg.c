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
main (argc, argv)
    int argc;
    char *argv[];
{
    int i;
    int errcnt;
    int packp;
    int (*fn) ();

    errcnt = 0;
    i = strlen (argv[0]);

    if (i > 4 &&
	((argv[0][i - 5] == 'x') || (argv[0][i - 5] == 'X')))
    {
	fn = UnPackDAWG;
	packp = 0;
    } else
    {
	fn = PackDAWG;
	packp = 1;
    }

    if (argc < 2)
    {
	(*fn) (stdin, stdout);
    } else
    {
	int i;
	int verbose;

	i = 1;
	verbose = 0;

	if (!strcmp (argv[i], "-v"))
	{
	    verbose = 1;
	    i++;
	}

	for ( /* nothing */ ; i < argc; i++)
	{
	    FILE *ifp;
	    FILE *ofp;
	    char iname[STRINGSIZE];
	    char oname[STRINGSIZE];
	    static char extn[] = ".dwg";

	    if (packp)
	    {
		/* filename -> filename.dwg */
		strcpy (iname, argv[i]);
		strcpy (oname, argv[i]);
		strcat (oname, extn);
	    } else
	    {
		int j;
		int e;

		/* filename(.dwg)? -> filename */
		strcpy (iname, argv[i]);
		j = strlen (iname);
		e = strlen (extn);

		if (j > e && !strcmp (extn, iname + j - e))
		{
		    strcpy (oname, iname);
		    oname[j - e] = '\0';
		} else
		{
		    strcpy (oname, iname);
		    strcat (iname, extn);
		}
	    }

	    if (!(ifp = fopen (iname, "r")))
	    {
		perror (argv[i]);
		continue;
	    }

	    if (!(ofp = fopen (oname, "w")))
	    {
		fclose (ifp);
		perror (argv[i]);
		continue;
	    }

	    if (verbose)
	    {
		printf("%sdawg: %s -> %s\n", packp ? "" : "x", iname, oname);
	    }

	    if ((*fn) (ifp, ofp))
	    {
		errcnt++;
		fprintf(stderr, "Error: failed while %spacking '%s'\n", packp ? "" : "un", iname);
	    }

	    fclose (ifp);
	    fclose (ofp);
	}
    }

    exit(errcnt);
}
