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

static int use_grule;
static char prefix_grule[STRINGSIZE] = ":";
static char suffix_grule[STRINGSIZE] = ":";
static char grulefile[] = "conf/globrule.conf";

void
DoMangle(fp, rule)
FILE *fp;
char *rule;
{
    char *ptr;
    char buff1[STRINGSIZE];
    char buff2[STRINGSIZE];
    char buff3[STRINGSIZE];

    for (*buff1 = '\0'; fgets(buff1, STRINGSIZE, fp); *buff1 = '\0')
    {
#ifdef DEBUG
	fprintf(stderr, "dictfilt: read: %s", buff1);
#endif

	if (use_grule)
	{
	    Chop(buff1);

	    if ((ptr = Mangle(buff1, prefix_grule)))
	    {
		strcpy(buff2, ptr);

		if ((ptr = Mangle(buff2, rule)))
		{
		    strcpy(buff3, ptr);

		    if ((ptr = Mangle(buff3, suffix_grule)))
		    {
#ifdef DEBUG
			fprintf(stderr, "dictfilt: wrote: %s\n", ptr);
#endif
			puts(ptr);
		    }
		}
	    }
	} else
	{
	    if ((ptr = Mangle(buff1, rule)))
	    {
#ifdef DEBUG
		fprintf(stderr, "dictfilt: wrote: %s\n", ptr);
#endif
		puts(ptr);
	    }
	}
    }

    return;
}

int
main(argc, argv)
int argc;
char *argv[];
{
    FILE *fp;
    char rbuff[STRINGSIZE];

    if ((fp = fopen(grulefile, "r")))
    {
	for (rbuff[0] = '\0'; fgets(rbuff, STRINGSIZE, fp); rbuff[0] = '\0')
	{
	    Trim(rbuff);

	    if ((!rbuff[0]) || (rbuff[0] == '#'))
	    {
		continue;
	    }
	    use_grule++;

	    if (use_grule == 1)
	    {
		strcpy(prefix_grule, rbuff);
	    } else if (use_grule == 2)
	    {
		strcpy(suffix_grule, rbuff);
		break;		/* ha ha! */
	    } else
	    {
		fprintf(stderr, "%s: ignoring '%s' in file %s (TCH)\n",
			argv[0], rbuff, grulefile);
		break;
	    }
	}

	fclose(fp);
    }
    /* ---- */

#ifdef DEBUG
    fprintf(stderr, "dictfilt: prefix %s\n", prefix_grule);
    fprintf(stderr, "dictfilt: suffix %s\n", suffix_grule);
#endif

    if (argc < 2)
    {
	if (!fgets(rbuff, STRINGSIZE, stdin))
	{
	    perror("dictfile: fgets couldnt read rule from stdin");
	    exit(1);
	}

	Trim(rbuff);

#ifdef DEBUG
	fprintf(stderr, "dictfilt: stdin rule: %s\n", rbuff);
	fprintf(stderr, "dictfilt: processing stdin\n");
#endif

	DoMangle(stdin, rbuff);
    } else
    {
	strcpy(rbuff, argv[1]);

#ifdef DEBUG
	fprintf(stderr, "dictfilt: argument rule: %s\n", rbuff);
#endif

	if (argc < 3)
	{
	    DoMangle(stdin, rbuff);
#ifdef DEBUG
	    fprintf(stderr, "dictfilt: processing stdin\n");
#endif
	} else
	{
	    int i;

	    for (i = 2; i < argc; i++)
	    {
		if (!(fp = fopen(argv[i], "r")))
		{
		    perror(argv[i]);
		    continue;
		}
#ifdef DEBUG
		fprintf(stderr, "dictfilt: processing %s\n", argv[i]);
#endif

		DoMangle(fp, rbuff);

		fclose(fp);
	    }
	}
    }

    return (0);
}
