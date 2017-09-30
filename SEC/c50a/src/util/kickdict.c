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

#undef DEBUG
#undef DEBUG2

static char dictconf[] = "conf/dictrun.conf";
static char dictgrps[] = "run/dict";
static char xprogram[] = "dictfilt | crack-sort | uniq";

int
main(argc, argv)
int argc;
char *argv[];
{
    int status;
    FILE *fpin;
    FILE *fpout;
    int dictnum;
    int counter;
    char *this_dictgrp = (char *) 0;
    char *this_genset = (char *) 0;
    char this_rule[STRINGSIZE];
    char buffer[STRINGSIZE];
    char program[16384];	/* BIG */

    if (argc < 2)
    {
	printf("/error/ Usage:\tkickdict [-list] [number]\n");
	exit(1);
    }

    if (!strcmp(argv[1], "-list"))
    {
	dictnum = 0;
    }
    else if (!(dictnum = atoi(argv[1])))
    {
	printf("/error/ kickdict: number must be gt 0\n");
	exit(1);
    }

    if (!(fpin = fopen(dictconf, "r")))
    {
	printf("/error/ kickdict: cannot read '%s'\n", dictconf);
	exit(1);
    }

    counter = 0;
    status = 0;

#ifdef DEBUG
    fprintf(stderr, "kickdict: *** searching for rule %d\n", (dictnum - 1));
#endif

    while ((!status) && fgets(buffer, STRINGSIZE, fpin))
    {
	char **pp;
	FILE *rufp;

	/* tidy up */
	Trim(buffer);

	/* check for comments */
	if ((!buffer[0]) || (buffer[0] == '#'))
	{
	    continue;
	}

#ifdef DEBUG
	fprintf(stderr, "kickdict: buffer=%s counter=%d\n", buffer, counter);
#endif

	pp = SplitOn(':', buffer, 2);
	this_dictgrp = pp[0];	/* == buffer */
	this_genset = pp[1];

#ifdef DEBUG
	fprintf(stderr, "kickdict: dictgrp=%s\n", pp[0]);
	fprintf(stderr, "kickdict: genset=%s\n", pp[1]);
#endif

	/* if it's a standalone generator, break out now */

	if (this_genset[0] == '|')
	{
	    counter++;

	    if (dictnum && (counter != dictnum))
	    {
		continue;
	    }
	    else if (!dictnum)
	    {
		printf("%-5d %-10s %s\n",
		       counter, this_dictgrp, this_genset);
		continue;
	    }
	    status = 2;
	    break;
	}

	/* else iterate thru the relevant genset */

	if (!(rufp = fopen(this_genset, "r")))
	{
	    printf("/error/ kickdict: cannot read '%s'\n", this_genset);
	    exit(1);
	}

/* strangest things ever said, #6, to alan cox: "...and remember, alan
 * - no monkeybusiness.  remember, i sleep nude and we dont want to
 * give rachel the shock of her life..." */

	while ((!status) && fgets(this_rule, STRINGSIZE, rufp))
	{
	    Trim(this_rule);

	    if ((!this_rule[0]) || (this_rule[0] == '#'))
	    {
		continue;
	    }

#ifdef DEBUG
	    fprintf(stderr, "kickdict: counter=%d rule=%s\n",
		    counter, this_rule);
#endif

	    counter++;

	    if (dictnum && (counter == dictnum))
	    {
		status = 1;
		break;
	    }
	    else if (!dictnum)
	    {
		printf("%-5d %-10s %s\n",
		       counter, this_dictgrp, this_rule);
	    }
	}

	fclose(rufp);
    }

    fclose(fpin);

    if (!dictnum)		/* -list */
    {
	return (0);
    }

#ifdef DEBUG
    fprintf(stderr, "kickdict: commited rule '%s'\n", this_rule);
    fprintf(stderr, "kickdict: commited dictgrp '%s'\n", this_dictgrp);
#endif

    program[0] = '\0';		/* just in case */

    if (status == 2)		/* read from a pipe */
    {
	if (this_dictgrp && this_dictgrp[0])	/* filter a dictgrp */
	{
	    strcpy(this_rule, ":");
	    sprintf(program,
		    "smartcat %s/%s.* %s",
		    dictgrps, this_dictgrp, this_genset);
	}
	else
	{
	    status = 3;		/* dictionary stream */
	    strcpy(this_rule, "<stream>");
	    sprintf(program, "%s", this_genset + 1);
	}
    }
    else if (status == 1)	/* standard dictionary rule */
    {
	if (this_dictgrp[0])
	{
	    sprintf(program,
		    "smartcat %s/%s.*",
		    dictgrps, this_dictgrp);
	}
	else
	{
	    status = 0;		/* have rule, but no dictgrp to apply it to */
	}
    }

    /* write status on stdout, NOT to be picked up by dictfilt,etc */

    printf("/%s/ stat=%d look=%d find=%d genset='%s' rule='%s' dgrp='%s' prog='%s'\n",
	   (status ? (status == 3 ? "stream" : "ok") : "error"),
	   status, dictnum, counter,
	   this_genset, this_rule, this_dictgrp,
	   program);

    fflush(stdout);		/* force write on stdout */

    if (!status)		/* final check before commit */
    {
	exit(1);
    }

#ifdef DEBUG
    fprintf(stderr, "kickdict: reading from: %s\n", program);
#endif

    /* now, kick off the subprocesses and copy from one to the other */
    if (!(fpin = popen(program, "r")))
    {
	perror(program);
	exit(1);
    }

    if (status != 3)		/* if not streaming, fire off dictfilter */
    {
#ifdef DEBUG
	fprintf(stderr, "kickdict: writing to: %s\n", xprogram);
#endif
	if (!(fpout = popen(xprogram, "w")))
	{
	    perror(xprogram);
	    exit(1);
	}

	fprintf(fpout, "%s\n", this_rule);	/* feed rule to dictfilt */
    }
    else
    {
#ifdef DEBUG
	fprintf(stderr, "kickdict: writing to <stdout>\n");
#endif
	fpout = stdout;
    }

    /* copy from one to the other, removing comment lines */

    for (buffer[0] = '\0'; fgets(buffer, STRINGSIZE, fpin); buffer[0] = '\0')
    {
	/* check for comments */
	if ((!buffer[0]) || (buffer[0] == '#'))
	{
	    continue;
	}

#ifdef DEBUG2
	fprintf(stderr, "kickdict: sending %s", buffer);
#endif

	/* write to child process */
	fputs(buffer, fpout);
    }

    pclose(fpin);

    if (status != 3)
    {
	pclose(fpout);
    }

    return (0);
}
