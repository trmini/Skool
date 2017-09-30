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
#include "elcid.h"

#define CRACK_MEMSMALL          0x0001
#define CRACK_DISKSMALL         0x0002
#define CRACK_STREAMING         0x0004
#define CRACK_MAIL              0x0010
static int crack_flags;

static int from_rule = 1;	/* where kickdict starts from */

static char nullstr[] = "(null)";	/* just in case */
static char pauser[] = "pauser";	/* run by 'system()' */
static char kickdict[] = "kickdict";	/* run by 'system()' */
static char nastygram[] = "nastygram";	/* run by 'system()' */

static long int cstat_numciphers;
static long int cstat_numgroups;
static long int cstat_numrejects;
static long int cstat_numerrors;

static int epws;		/* ELCID - max valid password length */
static int ebs;			/* ELCID - max no. of guesses per iteration */

/* ------------------------------------------------------------------ */

/* structure maintained for cracking */

struct CookieGroup
{
    struct CookieGroup *cg_next;	/* pointer to next in list */
    long int cg_count;		/* number of entries in a group */
    long int cg_unguessed;	/* number of entries unguessed */
    char **cg_ciphers;		/* vector of cg_count pointers to ciphers */
    void **cg_cookies;		/* vector of cg_count pointers to cookie */
    char **cg_passwords;	/* vector of cg_count (NULL?) pointers to cracked pws */
    char **cg_usernames;	/* vector of cg_count (NULL?) pointers to usernames */
    char **cg_comments;		/* vector of cg_count (NULL?) pointers to 'comment' */
    char **cg_wordgroups;	/* vector of cg_count (NULL?) pointers to wordgroups */
};

static struct CookieGroup *cgr_crackem;

/* ------------------------------------------------------------------ */

/* hook for loading the above */

struct DataHook
{
    struct DataHook *dh_next;
    char *dh_cipher;
    char *dh_username;
    char *dh_comment;
    char *dh_wordgroup;
};

static struct DataHook *dhr_cache;
static struct DataHook *dhr_used;
static struct DataHook *dhr_reject;

/* ------------------------------------------------------------------ */

/* structure for dictionary storage */

struct DictBlock
{
    struct DictBlock *db_next;
    char *db_guessvec[1];	/* vector of 'ebs' pointers to char */
    /* and a melange of malloc()ed bytes follow, undeclared */
};

static struct DictBlock *dbr_cache;

/* ------------------------------------------------------------------ */

/* structure for storing cipher/password combos */

struct PasswordStore
{
    struct PasswordStore *ps_next;
    char *ps_cipher;
    char *ps_plain;
};

static struct PasswordStore *psr_guessed;

/* ------------------------------------------------------------------ */

void
Logger(key, fmt, a, b, c, d, e, f)
char key;
char *fmt;
char *a, *b, *c, *d, *e, *f;
{
    long t;

    time(&t);

    fprintf(stdout, "%c:%ld:", key, t);
    fprintf(stdout, fmt, a, b, c, d, e, f);

    fflush(stdout);
    fflush(stderr);		/* and why not ? */
}

/* ------------------------------------------------------------------ */

void
CatchSig(i)
int i;
{
    Logger('T', "CatchSig: Caught a signal! (%d)\n", i);
    exit(0);
}

/* ================================================================== */

struct CookieGroup *
GetCookie()
{
    struct CookieGroup *retval;

    retval = (struct CookieGroup *) malloc(sizeof (struct CookieGroup));
    ClrMem(retval, sizeof (struct CookieGroup));

    return (retval);
}

/* ================================================================== */

struct DictBlock *
GetDictBlock()
{
    int size;
    struct DictBlock *retval;

    /* size = dictblock + */
    /* pointers to 'ebs' guesses, less 1 already allocated + */
    /* space for 'ebs' guesses of length 'epws' + NULL byte */

    size = sizeof (struct DictBlock) +
     ((ebs - 1) * sizeof (char *)) +
     (ebs * (epws + 1));

    if (dbr_cache)
    {
	retval = dbr_cache;
	dbr_cache = dbr_cache->db_next;
    }
    else
    {
	retval = (struct DictBlock *) malloc(size);
    }

    ClrMem(retval, size);
    return (retval);
}

void
PutDictBlock(db)
struct DictBlock *db;
{
    db->db_next = dbr_cache;
    dbr_cache = db;
}

/* ------------------------------------------------------------------ *

   "What is appropriate for the master, is not appropriate for the novice"

   C++.  Why?

   Admittedly, having to write your own storage allocators is a pain; you
   (and your team?) must be disciplined enough to utilise ONLY the code
   that you have written, without replication of functionality - which
   would otherwise (eventually) lead to code stagnation.

   However; in my particular happenstance, working on my own, with speed
   and portability an essential trait of my code, what *I* write is more
   likely to DWIW than what some package would provide me with.

   There will be bugs, of course, but that's life.

   The purpose of most OOPS seems to be to foist much of the
   responsibility for ensuring the overall correctness of a large
   software package onto the language designers and compiler writers.
   Primitives are cast in stone and are ubiquitous; modules only interact
   through well-defined channels of linkage.

   Assuming that nobody seriously screws up betwixt the heaven of linkage
   and the earth of primitives, the sun will shine, the earth will be
   productive, the package will function, and the users will be "happy".

   Until the planet's resources are exhausted.

   The "buffering" provided by supplying our inexperienced programmers
   with top-end development machines and cushy programming environments
   has far reaching effects; noviciates are protected from experiencing
   how to incant some of the deep (but boring) magic of programming.

   They are never dropped into the shit, and therefore don't know (never
   learn) how to swim out of it.

   The best thing that ever happened to Crack (and me, in my development
   as a programmer) was being forced to write the cracker code on my
   Amiga A500, in 2.5Mb of unprotected memory, with a 20Mb harddrive for
   storage.  Quickly I learned the limitations of the underlying machine
   (which, in practice, were few) and how my coding style was impacting
   the machine (which was overbearingly).

   This was the nub of my problems.

   After many months of trial, I learned how to make much better (near
   optimal, given my portability constraint) use of those resources at my
   disposal without crashing the F#@*$^%!! thing.

   The time was frustrating, but the experience was good for me.
   Calvin's father would probably call it "character building".

   Even today, I don't *always* check the return value from malloc(), but
   I respect that fact that there are people out there with "wimpier"
   machines than my own.  I try to make allowances and balanced
   decisions.

   In pursuit of such experience, I see a positive (but, in software
   development houses, probably unviable) argument for giving the
   "programmers" the worst possible machines for development platforms.

   The ironclad test should be to have to make their code perform
   responsively on at least 3 different architectures, one of which is
   diskless with 4Mb or less, of memory.  Preferably another one should
   have VM with the bottom 8Kb unmapped, or even have no MMU at all, with
   a particularly delicate O/S running the shebang.

   - and every single programmer should read both "The Mythical
   Man-Month" and "Structure and Interpretation of Computer Programs".

   * ------------------------------------------------------------------ */

struct DataHook *
GetDataHook()
{
    struct DataHook *retval;

    if (dhr_cache)
    {
	retval = dhr_cache;
	dhr_cache = retval->dh_next;
    }
    else
    {
	retval = (struct DataHook *) malloc(sizeof (struct DataHook));
    }

    ClrMem(retval, sizeof (struct DataHook));

    return (retval);
}

void
PutDataHook(dh)
struct DataHook *dh;
{
    dh->dh_next = dhr_cache;
    dhr_cache = dh;
}

/* ================================================================== */

struct PasswordStore *
FindPW(cipher, plain)
char *cipher;
char *plain;
{
    struct PasswordStore *retval;

    if ((!cipher) && (!plain))
    {
	return (struct PasswordStore *) 0;
    }

    for (retval = psr_guessed; retval; retval = retval->ps_next)
    {
	if (cipher && strcmp(cipher, retval->ps_cipher))
	{
	    continue;
	}
	if (plain && strcmp(plain, retval->ps_plain))
	{
	    continue;
	}
	break;			/* perverse, huh ? */
    }

    return (retval);
}

struct PasswordStore *
PushPW(cipher, plain)
char *cipher;
char *plain;
{
    struct PasswordStore *retval;

    retval = (struct PasswordStore *) malloc(sizeof (struct PasswordStore));

    retval->ps_cipher = Clone(cipher);
    retval->ps_plain = Clone(plain);
    retval->ps_next = psr_guessed;
    psr_guessed = retval;

    return (retval);
}

struct PasswordStore *
StorePW(cipher, plain)
char *cipher;
char *plain;
{
    struct PasswordStore *retval;

    if ((!cipher) || (!plain))
    {
	Logger('T', "StorePW: told to store a NULL pointer!\n");
	exit(1);
    }

    if ((retval = FindPW(cipher, plain)))
    {
	return (retval);
    }

    return (PushPW(cipher, plain));
}

/* ================================================================== */

void
FlushDataHooks()
{
    int i;
    struct DataHook *dh;
    struct CookieGroup *cg;

    for (dh = dhr_used, i = 0; dh; dh = dh->dh_next, i++)
    {
	cstat_numciphers++;
    }

    if (!i)
    {
	return;
    }

    cstat_numgroups++;

    cg = GetCookie();

    cg->cg_unguessed = cg->cg_count = i;

    cg->cg_ciphers = (char **) malloc(i * sizeof (char *));
    cg->cg_cookies = (void **) malloc(i * sizeof (void *));
    cg->cg_passwords = (char **) malloc(i * sizeof (char *));
    cg->cg_usernames = (char **) malloc(i * sizeof (char *));
    cg->cg_comments = (char **) malloc(i * sizeof (char *));
    cg->cg_wordgroups = (char **) malloc(i * sizeof (char *));

    for (dh = dhr_used, i = 0; dh; dh = dh->dh_next, i++)
    {
	cg->cg_ciphers[i] = dh->dh_cipher;
	cg->cg_passwords[i] = (char *) 0;	/* mark as unguessed */
	cg->cg_usernames[i] = dh->dh_username;
	cg->cg_comments[i] = dh->dh_comment;
	cg->cg_wordgroups[i] = dh->dh_wordgroup;
    }

    /* call library to make cookies */
    elcid_mkcookies(cg->cg_ciphers,
		    cg->cg_usernames,
		    cg->cg_cookies,
		    cg->cg_count);

    /* push onto list */
    cg->cg_next = cgr_crackem;
    cgr_crackem = cg;

    /* restore the data hooks for reuse */
    for (dh = dhr_used; dh; /* nothing */ )
    {
	struct DataHook *dh2;

	dh2 = dh->dh_next;
	PutDataHook(dh);
	dh = dh2;
    }
    dhr_used = (struct DataHook *) 0;
}

/* ------------------------------------------------------------------ */

void
StoreDataHook(atoms)
char **atoms;
{
    int booboo;
    char *sortkey;
    char *cipher;
    char *comment;
    char *username;
    char *wordgroup;
    struct DataHook *dh;
    static char lastsortkey[STRINGSIZE];

    booboo = 0;
    sortkey = atoms[1];
    cipher = atoms[2];
    username = atoms[3];
    comment = atoms[4];
    wordgroup = atoms[5];

    if (!wordgroup || !username || !comment || !cipher || !sortkey)
    {
	Logger('E', "StoreDataHook: short input on a 'D' line\n");
	cstat_numerrors++;
	return;
    }

    if (!cipher[0])
    {
	Logger('E', "StoreDataHook: null ciphertext: %s\n", username);
	StorePW(cipher, "<null-ciphertext>");
	cstat_numrejects++;
	booboo++;
    }
    else if (elcid_invalid(cipher))
    {
	Logger('E', "StoreDataHook: invalid ciphertext: %s %s\n", username, cipher);
	StorePW(cipher, "<invalid-ciphertext>");
	cstat_numrejects++;
	booboo++;
    }

    if (strcmp(lastsortkey, sortkey))	/* IS THIS STILL OK ??? */
    {
	FlushDataHooks();
	strcpy(lastsortkey, sortkey);
    }

    dh = GetDataHook();

    dh->dh_cipher = Clone(cipher);
    dh->dh_username = username[0] ? Clone(username) : (char *) 0;
    dh->dh_comment = comment[0] ? Clone(comment) : (char *) 0;
    dh->dh_wordgroup = wordgroup[0] ? Clone(wordgroup) : (char *) 0;

    if (booboo)
    {
	Logger('E',
	       "StoreDataHook: wg='%s' un='%s' cm='%s' ct='%s' sk='%s'\n",
	       (wordgroup ? wordgroup : nullstr),
	       (username ? username : nullstr),
	       (comment ? comment : nullstr),
	       (cipher ? cipher : nullstr),
	       (sortkey ? sortkey : nullstr));

	dh->dh_next = dhr_reject;
	dhr_reject = dh;
    }
    else
    {
	dh->dh_next = dhr_used;
	dhr_used = dh;
    }

    return;			/* done */
}

/* ------------------------------------------------------------------ */

void
LoadInput(fp)
FILE *fp;
{
    char **atoms;
    char buffer[STRINGSIZE];

    for (*buffer = '\0'; fgets(buffer, STRINGSIZE, fp); *buffer = '\0')
    {
	int i;

	/* skip blanks/comments */
	if ((!buffer[0]) || (buffer[0] == '#'))
	{
	    continue;
	}

	/* copy input to output */
	fputs(buffer, stdout);

	/* remove newline */
	Chop(buffer);

	/* switch on record type */
	switch (buffer[0])
	{
	case 'E':		/* error E:time:comment */
	case 'I':		/* info I:time:comment */
	case 'S':		/* startup S:time:comment */
	case 'T':		/* termination T:time:comment */
	    break;		/* - skip 'em */

	case 'O':		/* opendictstream O:time:integer */
	    atoms = SplitOn(':', buffer, 3);
	    from_rule = atoi(atoms[2]);
	    break;

	case 'F':		/* feedback F:ct:pt */
	    atoms = SplitOn(':', buffer, 3);
	    if (!atoms[1] || !atoms[2])
	    {
		Logger('E',
		       "LoadInput: Malformed FeedBack Message: '%s' '%s'\n",
		       (atoms[1] ? atoms[1] : nullstr),
		       (atoms[2] ? atoms[2] : nullstr));
		break;
	    }
	    StorePW(atoms[1], atoms[2]);
	    break;

	case 'G':		/* guess G:time:ct:pt */
	    atoms = SplitOn(':', buffer, 4);
	    if (!atoms[1] || !atoms[2] || !atoms[3])
	    {
		Logger('E',
		     "LoadInput: Malformed Guess Message: '%s' '%s' '%s'\n",
		       (atoms[1] ? atoms[1] : nullstr),
		       (atoms[2] ? atoms[2] : nullstr),
		       (atoms[3] ? atoms[3] : nullstr));
		break;
	    }
	    StorePW(atoms[2], atoms[3]);
	    break;

	case 'D':		/* data D:sort:ct:user:gecos:comment */
	    atoms = SplitOn(':', buffer, 6);
	    StoreDataHook(atoms);
	    break;

	default:
	    atoms = SplitOn(':', buffer, STRINGSIZE);
	    for (i = 0; atoms[i]; i++)
	    {
		Logger('E', "LoadInput: [%d] '%s'\n", i, atoms[i]);
	    }

	    Logger('T', "LoadInput: not recognised\n");
	    exit(1);
	    break;
	}
    }

    FlushDataHooks();		/* for good measure */
}

/* ------------------------------------------------------------------ */

int
LoadDictBlock(fp, db)
FILE *fp;
struct DictBlock *db;
{
    int i;
    char *optr;
    int retval;			/* count of words loaded */

    retval = 0;
    optr = (char *) &db->db_guessvec[ebs];	/* HACK HACK BLETCH HACK */

    for (i = 0; i < ebs; i++)
    {
	char *iptr;
	char tbuf[STRINGSIZE];

	if (!fgets(tbuf, STRINGSIZE, fp))	/* hit EOF and quit */
	{
	    break;
	}
	Chop(tbuf);		/* remove newline */
	tbuf[epws] = '\0';	/* enforce maximum size of a PW anyway */
	retval++;		/* bump the returned word count */

	/* point at location for word and copy out */
	db->db_guessvec[i] = optr;

	for (iptr = tbuf; *iptr; /* nothing */ )
	{
	    *(optr++) = *(iptr++);
	}

	*(optr++) = '\0';	/* terminate stored word */
    }

    return (retval);
}

/* ------------------------------------------------------------------ */

struct DictBlock *
LoadDictionary(fp)
FILE *fp;
{
    long int wordcount;
    struct DictBlock *retval;

    wordcount = 0;

    retval = (struct DictBlock *) 0;

    while (!feof(fp))
    {
	int i;
	struct DictBlock *this;

	this = GetDictBlock();
	i = LoadDictBlock(fp, this);

	this->db_next = retval;
	retval = this;

	wordcount += i;

	if (i < ebs)		/* will still work for i == 0 */
	{
	    break;
	}
    }

    Logger('I', "LoadDictionary: loaded %ld words into memory\n", wordcount);

    return (retval);
}

void
DropDictionary(db)
struct DictBlock *db;
{
    struct DictBlock *db2;

    while (db)
    {
	db2 = db->db_next;
	PutDictBlock(db);
	db = db2;
    }
}

/* ------------------------------------------------------------------ */

FILE *
OpenDictStream(number)
{
    FILE *fp;
    char tbuf[STRINGSIZE];
    char dictcall[STRINGSIZE];
    static char dictok[] = "/ok/";
    static char dictstream[] = "/stream/";

    sprintf(dictcall, "%s %d", kickdict, number);

    Logger('I', "OpenDictStream: trying: %s\n", dictcall);

    /* open the relevant dictionary */
    if (!(fp = popen(dictcall, "r")))
    {
	perror("popen");
	Logger('T', "OpenDictStream: popen failed\n");
	exit(1);
    }

    /* read the status line here and check end of dictionaries */
    if (!fgets(tbuf, STRINGSIZE, fp))
    {
	Logger('T', "OpenDictStream: initial fgets failed\n");
	perror("fgets");
	exit(1);
    }

    Chop(tbuf);

    /* check for stream flags here and act accordingly if disksmall &&
       memsmall */

    Logger('I', "OpenDictStream: status: %s\n", tbuf);	/* for people */
    Logger('O', "%d\n", number);	/* for computer */

    if (!strncmp(tbuf, dictok, strlen(dictok)))
    {
	crack_flags &= ~CRACK_STREAMING;	/* switch off streaming */
	return (fp);
    }
    else if (!strncmp(tbuf, dictstream, strlen(dictstream)))
    {
	crack_flags |= CRACK_STREAMING;		/* switch it back on */
	return (fp);
    }

    /* you've hit a error or end of dictionaries */
    Logger('I', "OpenDictStream: end of dictionaries: %s\n", tbuf);
    pclose(fp);

    return ((FILE *) 0);
}

/* ================================================================== */

void
Pauser()
{
    time_t now;
    static time_t then;

    time(&now);

    if ((now - then) > 60)
    {
	system(pauser);
	time(&then);
    }
}

/* optimise this to hell and back */

int
DoIt(cg, guessvec)
struct CookieGroup *cg;
char **guessvec;
{
    int i;
    int hits;

    if ((hits = elcid_test(guessvec,
			   cg->cg_cookies,
			   cg->cg_passwords,
			   cg->cg_count)))
    {
	/* erase the cookies of anyone we hit (kinky!) */
	for (i = 0; i < cg->cg_count; i++)
	{
	    if (cg->cg_passwords[i] && cg->cg_cookies[i])
	    {
		Logger('G', "%s:%s\n",
		       cg->cg_ciphers[i], cg->cg_passwords[i]);

		/* waste a little memory, so? */
		cg->cg_cookies[i] = (void *) 0;

		/* send mail if desired & possible */
		if ((crack_flags & CRACK_MAIL) && (cg->cg_usernames[i]))
		{
		    char tbuf[8192];

		    sprintf(tbuf, "%s %s", nastygram, cg->cg_usernames[i]);
		    system(tbuf);
		}
	    }
	}

	/* break out if no-one left to hit */
	cg->cg_unguessed -= hits;
    }
    return (cg->cg_unguessed);
}

/* ================================================================== */

void
Cracker()
{
    int i;
    FILE *fp;
    int dictloop;
    struct CookieGroup *cg;

    /* boot the elcid library */
    elcid_init();

    /* load max #guesses/iteration into global value */
    ebs = elcid_bsize();

    /* load max valid pw length into global value */
    epws = elcid_pwsize();

    /* clean up after a recovery */

    /* okay, i know this is a n**2 algorithm, but there's no guarantee
     * some spod won't write a SPF rotine which presents the cracker
     * with unsorted input, else I could make it (n**2)/2 by breaking
     * after a successful erasure; mostly this should be academic as
     * the fbfilt should have removed all such guessed records from
     * our input, but why not put it in anyway in case it is needed?
     * */

    for (cg = cgr_crackem; cg; cg = cg->cg_next)
    {
	for (i = 0; i < cg->cg_count; i++)
	{
	    if (cg->cg_cookies[i] && FindPW(cg->cg_ciphers[i], (char *) 0))
	    {
		Logger('I', "Cracker: erasing '%s' as already cracked\n",
		       cg->cg_ciphers[i]);
		cg->cg_cookies[i] = (void *) 0;
	    }
	}
    }

    /* loop over all dictionaries */
    for (dictloop = from_rule;; dictloop++)
    {
	if (!(fp = OpenDictStream(dictloop)))
	{
	    return;
	}

	if ((crack_flags & CRACK_STREAMING) ||
	    ((crack_flags & CRACK_MEMSMALL) &&
	     (crack_flags & CRACK_DISKSMALL)))
	{
	    struct DictBlock *db;

	    db = GetDictBlock();

	    /* have to do this particular mode arse-backwards because
	       of streaming/memory constraints; this is less efficient
	       than the "do an entire dictionary under one salt"
	       scheme which originally inspired me to write Crack, but
	       the impact is reduced by keeping EBS high */

	    while (LoadDictBlock(fp, db))
	    {
		for (cg = cgr_crackem; cg; cg = cg->cg_next)
		{
		    if (cg->cg_unguessed)
		    {
			elcid_setup(cg->cg_ciphers);
			DoIt(cg, db->db_guessvec);
		    }
		}

		Pauser();
	    }

	    PutDictBlock(db);
	}
	else if (crack_flags & CRACK_MEMSMALL)
	{
	    FILE *cfp;
	    char tbuf[8192];
	    struct DictBlock *db;
	    static char dictcache[] = "/tmp/.C50cache";

	    /* cache dictionary to disk here */
	    if (!(cfp = fopen(dictcache, "w+")))
	    {
		Logger('T', "Cracker: cannot open dictcache %s\n",
		       dictcache);
		perror(dictcache);
		exit(1);

	    }

	    while ((i = fread(tbuf, 1, sizeof (tbuf), fp)))
	    {
		if (fwrite(tbuf, 1, i, cfp) != i)
		{
		    Logger('T', "Cracker: cannot write to dictcache %s\n",
			   dictcache);
		    perror(dictcache);
		    exit(1);
		}
	    }

	    /* close the dictionary here */
	    pclose(fp);

	    /* flush the cache */
	    fflush(cfp);

	    /* i tawt i taw a diwctionawy bwock */
	    db = GetDictBlock();

	    /* crack */
	    for (cg = cgr_crackem; cg; cg = cg->cg_next)
	    {
		if (cg->cg_unguessed)
		{
		    /* setup for this pass */
		    elcid_setup(cg->cg_ciphers);

		    /* rewind cache */
		    rewind(cfp);

		    while (LoadDictBlock(cfp, db))
		    {
			if (!DoIt(cg, db->db_guessvec))
			{
			    break;
			}
		    }
		}

		Pauser();
	    }

	    /* replace block */
	    PutDictBlock(db);

	    /* close cache */
	    fclose(cfp);
	}
	else
	    /* big memory model - probably the finest password cracker
	       in the world, if you have the (modest) resources
	       necessary to run it */
	{
	    struct DictBlock *dictroot;

	    dictroot = LoadDictionary(fp);

	    pclose(fp);

	    /* iterate over the users */
	    for (cg = cgr_crackem; cg; cg = cg->cg_next)
	    {
		struct DictBlock *db;

		/* crack */
		if (cg->cg_unguessed)
		{
		    /* setup for this pass */
		    elcid_setup(cg->cg_ciphers);

		    /* iterate over the dictionary */
		    for (db = dictroot; db; db = db->db_next)
		    {
			if (!DoIt(cg, db->db_guessvec))
			{
			    break;
			}
		    }
		}

		Pauser();
	    }

	    /* Free up the dictionary */
	    DropDictionary(dictroot);
	}
    }

    return;
}

/* ================================================================== */

int
main(argc, argv)
int argc;
char *argv[];
{
    int i;
    int errs;
    int user_from_rule;

    errs = 0;
    crack_flags = 0;
    user_from_rule = 0;

    for (i = 3; i < 256; i++)
    {
	close(i);		/* cheat */
    }

    for (i = 1; i < argc; i++)
    {
	if (argv[i][0] == '-')
	{
	    if (!strcmp(argv[i], "-mail"))
	    {
		crack_flags |= CRACK_MAIL;
	    }
	    else if (!strcmp(argv[i], "-disksmall"))
	    {
		crack_flags |= CRACK_MEMSMALL | CRACK_DISKSMALL;
	    }
	    else if (!strcmp(argv[i], "-memsmall"))
	    {
		crack_flags |= CRACK_MEMSMALL;

	    }
	    else if (!strcmp(argv[i], "-from"))
	    {
		if (++i >= argc)
		{
		    fprintf(stderr, "%s: -from needs arg\n", argv[0]);
		    exit(1);
		}

		if ((user_from_rule = atoi(argv[i])) < 1)
		{
		    fprintf(stderr, "%s: bad rule-start number '%s'\n",
			    argv[0], argv[i]);
		    exit(1);
		}
	    }
	    else if (!strcmp(argv[i], "-nice"))
	    {
		int niceval;

		if (++i >= argc)
		{
		    fprintf(stderr, "%s: -nice needs arg\n", argv[0]);
		    exit(1);
		}

		niceval = atoi(argv[i]);

		if (nice(niceval) < 0)
		{
		    perror("nice");
		    exit(1);
		}
	    }
	    else if (!strcmp(argv[i], "-kill"))
	    {
		FILE *kfp;

		if (++i >= argc)
		{
		    fprintf(stderr, "%s: -kill needs arg\n", argv[0]);
		    exit(1);
		}

		if (!(kfp = fopen(argv[i], "w")))
		{
		    perror(argv[i]);
		    exit(1);
		}

		fprintf(kfp, "#!/bin/sh\n");
		fprintf(kfp, "kill -TERM %d\n", (int) getpid());
		fprintf(kfp, "rm -f $0\n");
		fprintf(kfp, "exit 0\n");
		fclose(kfp);
	    }
	    else
	    {
		fprintf(stderr,
			"%s: bad argument '%s'\n", argv[0], argv[i]);
		errs++;
	    }
	}
    }

    if (errs)
    {
	fprintf(stderr,
		"%s: errors encountered - bailing out before I freak\n",
		argv[0]);
	exit(1);
    }
    /* ------------------------------------------------------------------ */

    /* signal catching */
    signal(SIGHUP, CatchSig);
    signal(SIGINT, CatchSig);
    signal(SIGQUIT, CatchSig);
    signal(SIGTERM, CatchSig);

    /* load the input - may reset from_rule */
    LoadInput(stdin);

    /* override from_rule if needs be */
    if (user_from_rule)
    {
	from_rule = user_from_rule;
    }

    /* do not close stdin, as it fucks over GNU popen() */
    /* fclose(stdin); */

    /* write a startup message */
    Logger('S', "pid=%d ciphers=%ld saltgroups=%ld rejects=%ld errors=%ld srule=%d\n",
	   getpid(),
	   cstat_numciphers,
	   cstat_numgroups,
	   cstat_numrejects,
	   cstat_numerrors,
	   from_rule);

    /* one final check... */
    if (cstat_numciphers <= 0)
    {
	Logger('T', "Crack: nothing 'uncracked' in my input. Done.\n");
	exit(0);
    }

    /* 3.. 2.. 1.. Thunderbirds are go! */
    Cracker();

    /* Fall down go BOOM! */
    Logger('T', "Crack: Done.\n");

    return (0);
}
