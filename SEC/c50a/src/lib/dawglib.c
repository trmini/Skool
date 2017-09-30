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

/*
 * *** Directed Acyclic Word Graphs ****
 */

#if 0
 dawg file:	<magic> <elements> [EOF];
 magic: 	[string "#!xdawg\n"];
 elements:	<element> <elements>;
 element:	<offset> <string>;
 offset:	[value from 0..63 encoded as ASCII '0' .. 'o'];
 string:	[vector of bytes in the file, newline terminated];
#endif

/* Files are meant to be read sorted serially (once the magic number
 * has been identified) and each word is stored as a delta of the
 * previous one, the offset at which the new word differs from the
 * previous one is encoded as the first char of the line.
 */

#define LBND '0'
#define UBND 'z'

char dawgmagic[] = "#!xdawg\n";

static char gbuffer[STRINGSIZE];
static char pbuffer[STRINGSIZE];

static char *
PutDAWG (fp, buffer)
    FILE *fp;
    char *buffer;
{
    int i;

    ChopNL(buffer);

    for (i = 0; buffer[i] && pbuffer[i] && (buffer[i] == pbuffer[i]); i++);

    if (i > (UBND - LBND))
    {
	i = 0; /* can't cope, so pass verbatim */
    }

    putc ((unsigned char) i + LBND, fp);
    fputs(buffer + i, fp);
    fputc('\n', fp);
    strcpy(pbuffer, buffer);
    return (buffer);
}

static char *
GetDAWG (fp, buffer)
    FILE *fp;
    char *buffer;
{
    unsigned int off;

    if ((off = (unsigned int) getc (fp)) == (unsigned) EOF)
    {
	return ((char *) 0);
    } else if (off < LBND || off > UBND)
    {
	Debug (1, "GetDAWG: read weird offset: %d\n", off);
	return ((char *) 0);
    }
    off -= LBND;

    fgets (gbuffer + off, STRINGSIZE - off, fp); /* includes NL */
    gbuffer[STRINGSIZE - 1] = '\0';              /* to be certain */

    strcpy (buffer, gbuffer);		         /* when in doubt, BFI */

    return (buffer);
}

int
ResetDAWG ()
{
    ClrMem (gbuffer, sizeof (gbuffer));
    ClrMem (pbuffer, sizeof (pbuffer));

    return (0);
}

int
PackDAWG (infp, outfp)
    FILE *infp;
    FILE *outfp;
{
    char buffer[STRINGSIZE];

    ResetDAWG ();

    fputs (dawgmagic, outfp);

    while (fgets (buffer, STRINGSIZE, infp))
    {
	PutDAWG (outfp, buffer);
    }

    return (0);
}

int
UnPackDAWG (infp, outfp)
    FILE *infp;
    FILE *outfp;
{
    char buffer[STRINGSIZE];

    ResetDAWG ();

    while (fgets (buffer, STRINGSIZE, infp))
    {
	if (!strcmp (buffer, dawgmagic))
	{
	    break;
	}
    }

    while (GetDAWG (infp, buffer))
    {
	fputs(buffer, outfp);
    }

    return (0);
}
