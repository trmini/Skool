#
# This file is part of John the Ripper password cracker,
# Copyright (c) 1996-98 by Solar Designer
#

CPP = gcc
CC = gcc
AS = gcc
LD = gcc
CP = cp
LN = ln -sf
RM = rm -f
SED = sed
NULL = /dev/null
CPPFLAGS = -E
CFLAGS = -c -Wall -O2 -fomit-frame-pointer
ASFLAGS = -c
LDFLAGS = -s
OPT_NORMAL = -funroll-loops
OPT_INLINE = -finline-functions

JOHN_VERSION = 1.6
JOHN_ARCHIVE = john-$(JOHN_VERSION)

JOHN_DIST = \
	$(JOHN_ARCHIVE)/src/*.{c,h,S,sh,asm,com,diff} \
	$(JOHN_ARCHIVE)/src/Makefile{,.dep} \
	$(JOHN_ARCHIVE)/run/{mailer,john.ini,*.chr,password.lst} \
	$(JOHN_ARCHIVE)/doc/* \
	$(JOHN_ARCHIVE)/README

JOHN_OBJS_ORIG = \
	DES_fmt.o DES_std.o \
	BSDI_fmt.o \
	MD5_fmt.o MD5_std.o \
	BF_fmt.o BF_std.o \
	AFS_fmt.o \
	LM_fmt.o \
	batch.o bench.o charset.o common.o compiler.o config.o cracker.o \
	external.o formats.o getopt.o idle.o inc.o john.o list.o loader.o \
	logger.o math.o memory.o misc.o options.o params.o path.o recovery.o \
	rpp.o rules.o signals.o single.o status.o tty.o wordlist.o \
	unshadow.o \
	unafs.o \
	unique.o

JOHN_OBJS = \
	$(JOHN_OBJS_ORIG)

BITSLICE_OBJS = \
	DES_bs.o DES_bs_b.o

BENCH_DES_OBJS_ORIG = \
	DES_fmt.o DES_std.o

BENCH_DES_OBJS_DEPEND = \
	$(BENCH_DES_OBJS_ORIG)

BENCH_MD5_OBJS_DEPEND = \
	MD5_std.o

BENCH_BF_OBJS_DEPEND = \
	BF_std.o

BENCH_OBJS = \
	$(BITSLICE_OBJS) \
	$(BENCH_DES_OBJS_DEPEND) \
	MD5_fmt.o $(BENCH_MD5_OBJS_DEPEND) \
	BF_fmt.o $(BENCH_BF_OBJS_DEPEND) \
	bench.o best.o common.o config.o formats.o memory.o misc.o params.o \
	path.o signals.o tty.o

PROJ = ../run/john ../run/unshadow ../run/unafs ../run/unique
PROJ_DOS = ../run/john.bin ../run/john.com \
	../run/unshadow.com ../run/unafs.com ../run/unique.com
PROJ_WIN32 = ../run/john.exe \
	../run/unshadow.exe ../run/unafs.exe ../run/unique.exe

default:
	@echo "To build John the Ripper, type:"
	@echo "	make SYSTEM"
	@echo "where SYSTEM can be one of the following:"
	@echo "linux-x86-any-elf        Linux, x86, ELF binaries"
	@echo "linux-x86-mmx-elf        Linux, x86 with MMX, ELF binaries"
	@echo "linux-x86-k6-elf         Linux, AMD K6, ELF binaries"
	@echo "linux-x86-any-a.out      Linux, x86, a.out binaries"
	@echo "linux-alpha              Linux, Alpha"
	@echo "linux-sparc              Linux, SPARC"
	@echo "freebsd-x86-any-a.out    FreeBSD, x86, a.out binaries"
	@echo "freebsd-x86-k6-a.out     FreeBSD, AMD K6, a.out binaries"
	@echo "freebsd-x86-any-elf      FreeBSD, x86, ELF binaries"
	@echo "freebsd-x86-mmx-elf      FreeBSD, x86 with MMX, ELF binaries"
	@echo "freebsd-x86-k6-elf       FreeBSD, AMD K6, ELF binaries"
	@echo "openbsd-x86-any          OpenBSD, x86"
	@echo "openbsd-x86-k6           OpenBSD, AMD K6"
	@echo "solaris-sparc-gcc        Solaris, SPARC, gcc"
	@echo "solaris-sparc-v8-cc      Solaris, SPARC V8, cc"
	@echo "solaris-sparc-v9-cc      Solaris, SPARC V9, cc"
	@echo "solaris-x86-any          Solaris, x86, gcc"
	@echo "solaris-x86-k6           Solaris, AMD K6, gcc"
	@echo "digital-alpha-cc         Digital UNIX, Alpha, cc"
	@echo "aix-ppc-cc               AIX, PowerPC, cc"
	@echo "hpux-pa-risc-gcc         HP-UX, PA-RISC, gcc"
	@echo "hpux-pa-risc-cc          HP-UX, PA-RISC, cc"
	@echo "irix-mips32-cc           IRIX, MIPS 32-bit, cc"
	@echo "irix-mips64-cc           IRIX, MIPS 64-bit, cc"
	@echo "dos-djgpp-x86-any        DOS, DJGPP 2.x, x86"
	@echo "dos-djgpp-x86-mmx        DOS, DJGPP 2.x, x86 with MMX"
	@echo "dos-djgpp-x86-k6         DOS, DJGPP 2.x, AMD K6"
	@echo "win32-cygwin-x86-any     Win32, Cygwin, x86"
	@echo "win32-cygwin-x86-mmx     Win32, Cygwin, x86 with MMX"
	@echo "win32-cygwin-x86-k6      Win32, Cygwin, AMD K6"
	@echo "generic                  Any other UNIX system with gcc"

linux-x86-any-elf:
	$(LN) x86-any.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(JOHN_OBJS) x86.o" \
		CFLAGS="$(CFLAGS) -m486"

linux-x86-mmx-elf:
	$(LN) x86-mmx.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(JOHN_OBJS) x86.o" \
		CFLAGS="$(CFLAGS) -m486"

linux-x86-k6-elf:
	$(LN) x86-k6.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS) x86.o"

linux-x86-any-a.out:
	$(LN) x86-any.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(JOHN_OBJS) x86.o" \
		CFLAGS="$(CFLAGS) -m486" \
		ASFLAGS="$(ASFLAGS) -DUNDERSCORES -DALIGN_LOG"

linux-alpha:
	$(LN) alpha.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS) alpha.o"

linux-alpha-very-slow-cache:
	$(LN) alpha-4k.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(JOHN_OBJS) alpha.o"

linux-sparc:
	$(MAKE) HAMMER=use-linux-sparc sparc.h
	ln -s sparc.h arch.h
	$(MAKE) use-linux-sparc NAIL="$(PROJ)"

use-linux-sparc:
	$(MAKE) $(NAIL) \
		BENCH_DES_OBJS_DEPEND="$(BENCH_DES_OBJS_ORIG) sparc.o" \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS_ORIG) sparc.o"

freebsd-x86-any-a.out:
	$(LN) x86-any.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(JOHN_OBJS) x86.o" \
		CFLAGS="$(CFLAGS) -m486" \
		ASFLAGS="$(ASFLAGS) -DUNDERSCORES -DALIGN_LOG -DBSD"

freebsd-x86-k6-a.out:
	$(LN) x86-k6.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS) x86.o" \
		ASFLAGS="$(ASFLAGS) -DUNDERSCORES -DALIGN_LOG -DBSD"

freebsd-x86-any-elf:
	$(LN) x86-any.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(JOHN_OBJS) x86.o" \
		CFLAGS="$(CFLAGS) -m486" \
		ASFLAGS="$(ASFLAGS) -DBSD"

freebsd-x86-mmx-elf:
	$(LN) x86-mmx.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(JOHN_OBJS) x86.o" \
		CFLAGS="$(CFLAGS) -m486" \
		ASFLAGS="$(ASFLAGS) -DBSD"

freebsd-x86-k6-elf:
	$(LN) x86-k6.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS) x86.o" \
		ASFLAGS="$(ASFLAGS) -DBSD"

openbsd-x86-any:
	$(LN) x86-any.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(JOHN_OBJS) x86.o" \
		CFLAGS="$(CFLAGS) -m486" \
		ASFLAGS="$(ASFLAGS) -DUNDERSCORES -DALIGN_LOG -DBSD"

openbsd-x86-k6:
	$(LN) x86-k6.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS) x86.o" \
		ASFLAGS="$(ASFLAGS) -DUNDERSCORES -DALIGN_LOG -DBSD"

solaris-sparc-any:
	$(MAKE) $(HAMMER) NAIL=sparc.h
	$(RM) arch.h
	ln -s sparc.h arch.h
	$(MAKE) $(HAMMER) NAIL="$(PROJ)"

solaris-sparc-gcc:
	$(MAKE) solaris-sparc-any HAMMER=use-solaris-sparc-gcc

solaris-sparc-v8-cc:
	$(MAKE) solaris-sparc-any HAMMER=use-solaris-sparc-v8-cc

solaris-sparc-v9-cc:
	$(MAKE) solaris-sparc-any HAMMER=use-solaris-sparc-v9-cc

use-solaris-sparc-gcc:
	$(MAKE) $(NAIL) \
		BENCH_DES_OBJS_DEPEND="$(BENCH_DES_OBJS_ORIG) sparc.o" \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS_ORIG) sparc.o"

use-solaris-sparc-v8-cc:
	$(MAKE) $(NAIL) \
		BENCH_DES_OBJS_DEPEND="$(BENCH_DES_OBJS_ORIG) spro-sparc.o" \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS_ORIG) spro-sparc.o" \
		CPP=cc CC=cc AS=cc LD=cc \
		CFLAGS="-c -xO4 -xarch=v8" \
		LDFLAGS="-s -lc" \
		OPT_NORMAL="" \
		OPT_INLINE="-xinline=S1,S2,S3,S4,S5,S6,S7,S8"

use-solaris-sparc-v9-cc:
	$(MAKE) $(NAIL) \
		BENCH_DES_OBJS_DEPEND="$(BENCH_DES_OBJS_ORIG) spro-sparc.o" \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS_ORIG) spro-sparc.o" \
		CPP=cc CC=cc AS=cc LD=cc \
		CFLAGS="-c -xO4 -xarch=v8plusa -xchip=ultra" \
		LDFLAGS="-s -lc" \
		OPT_NORMAL="" \
		OPT_INLINE="-xinline=S1,S2,S3,S4,S5,S6,S7,S8"

sparc.h:
	$(RM) arch.h
	$(CC) $(CFLAGS) -DOS_TIMER -DOS_FLOCK -DDES_ASM detect.c
	$(LD) $(LDFLAGS) detect.o -o detect
	./sparc.sh "$(MAKE)" "$(HAMMER)" "$(BENCH_DES_OBJS_DEPEND)"

# Older versions of spro cc didn't support .S files directly
spro-sparc.o: sparc.S
	$(CPP) $(CPPFLAGS) sparc.S > tmp.s
	$(AS) $(ASFLAGS) tmp.s -o spro-sparc.o
	$(RM) tmp.s

solaris-x86-any:
	$(RM) arch.h
	ln -s x86-any.h arch.h
	$(MAKE) $(PROJ) \
		SHELL=/bin/sh \
		JOHN_OBJS="$(JOHN_OBJS) solaris-x86.o" \
		CFLAGS="$(CFLAGS) -m486"

solaris-x86-k6:
	$(RM) arch.h
	ln -s x86-k6.h arch.h
	$(MAKE) $(PROJ) \
		SHELL=/bin/sh \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS) solaris-x86.o"

# Solaris x86 got a broken assembler, with line length restriction (and some
# other problems, that are worked around in x86.S)
solaris-x86.o: x86.S
	$(CPP) $(CPPFLAGS) -P -DSOLARIS x86.S | tr \; \\n > tmp.s
	$(AS) $(ASFLAGS) tmp.s -o solaris-x86.o
	$(RM) tmp.s

digital-alpha-cc:
	$(LN) alpha.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS) digital-alpha.o" \
		CPP=cc CC=cc AS=cc LD=cc \
		CFLAGS="-c -O5 -tune host" \
		OPT_NORMAL="" \
		OPT_INLINE="-inline all"

# DEC's cc and make use the .S suffix for a different purpose...
digital-alpha.o: alpha.S
	$(CPP) $(CPPFLAGS) alpha.S > tmp.s
	$(AS) $(ASFLAGS) tmp.s -o digital-alpha.o
	$(RM) tmp.s

aix-ppc-cc:
	$(LN) ppc.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS)" \
		CPP=cc CC=cc AS=cc LD=cc \
		CFLAGS="-c -qunroll -qarch=ppc -qtune=601 -qchars=signed" \
		LDFLAGS="-s -lbsd" \
		OPT_NORMAL="-O2" \
		OPT_INLINE="-O3 -Q=99 -w"

hpux-pa-risc-gcc:
	$(LN) pa-risc.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS)"

hpux-pa-risc-cc:
	$(LN) pa-risc.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS)" \
		CPP=cc CC=cc AS=cc LD=cc \
		CFLAGS="-c -Aa -D_HPUX_SOURCE -DANSI_CPP" \
		LDFLAGS="-s" \
		OPT_NORMAL="+O2" \
		OPT_INLINE="+O3 +Oinline"

irix-mips32-cc:
	$(LN) mips32.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS)" \
		CPP=cc CC=cc AS=cc LD=cc \
		CFLAGS="-c -O2 -32 -signed" \
		LDFLAGS="-s -32" \
		OPT_NORMAL="-LNO:opt=1 -OPT:Olimit=2194" \
		OPT_INLINE="-INLINE:all"

irix-mips64-cc:
	$(LN) mips64.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS)" \
		CPP=cc CC=cc AS=cc LD=cc \
		CFLAGS="-c -O2 -64 -mips4 -r10000 -signed" \
		LDFLAGS="-s -64 -mips4 -r10000" \
		OPT_NORMAL="-LNO:opt=1 -OPT:Olimit=2194" \
		OPT_INLINE="-INLINE:all"

dos-djgpp-x86-any:
	copy x86-any.h arch.h
	$(MAKE) $(PROJ_DOS) \
		JOHN_OBJS="$(JOHN_OBJS) x86.o" \
		CFLAGS="$(CFLAGS) -m486" \
		ASFLAGS="$(ASFLAGS) -DUNDERSCORES -DALIGN_LOG"

dos-djgpp-x86-mmx:
	copy x86-mmx.h arch.h
	$(MAKE) $(PROJ_DOS) \
		JOHN_OBJS="$(JOHN_OBJS) x86.o" \
		CFLAGS="$(CFLAGS) -m486" \
		ASFLAGS="$(ASFLAGS) -DUNDERSCORES -DALIGN_LOG"

dos-djgpp-x86-k6:
	copy x86-k6.h arch.h
	copy nonstd.c DES_bs_s.c
	$(MAKE) $(PROJ_DOS) \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS) x86.o" \
		ASFLAGS="$(ASFLAGS) -DUNDERSCORES -DALIGN_LOG"

win32-cygwin-x86-any:
	$(CP) x86-any.h arch.h
	$(MAKE) $(PROJ_WIN32) \
		JOHN_OBJS="$(JOHN_OBJS) x86.o" \
		CFLAGS="$(CFLAGS) -mpentium" \
		ASFLAGS="$(ASFLAGS) -DUNDERSCORES"

win32-cygwin-x86-mmx:
	$(CP) x86-mmx.h arch.h
	$(MAKE) $(PROJ_WIN32) \
		JOHN_OBJS="$(JOHN_OBJS) x86.o" \
		CFLAGS="$(CFLAGS) -mpentium" \
		ASFLAGS="$(ASFLAGS) -DUNDERSCORES"

win32-cygwin-x86-k6:
	$(CP) x86-k6.h arch.h
	$(CP) nonstd.c DES_bs_s.c
	$(MAKE) $(PROJ_WIN32) \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS) x86.o" \
		CFLAGS="-c -Wall -O -fomit-frame-pointer" \
		ASFLAGS="$(ASFLAGS) -DUNDERSCORES"

generic: generic.h
	$(RM) arch.h
	ln -s generic.h arch.h
	$(MAKE) $(PROJ) \
		JOHN_OBJS="$(BITSLICE_OBJS) $(JOHN_OBJS)"

generic.h:
	$(RM) arch.h
	$(CC) $(CFLAGS) detect.c
	$(LD) $(LDFLAGS) detect.o -o detect
	./best.sh "$(MAKE)" \
		"$(BENCH_DES_OBJS_DEPEND)" \
		"$(BENCH_MD5_OBJS_DEPEND)" \
		"$(BENCH_BF_OBJS_DEPEND)"

bench: $(BENCH_OBJS)
	$(LD) $(LDFLAGS) $(BENCH_OBJS) -o bench

../run/john: $(JOHN_OBJS)
	$(LD) $(LDFLAGS) $(JOHN_OBJS) -o ../run/john

../run/unshadow: ../run/john
	ln -s john ../run/unshadow

../run/unafs: ../run/john
	ln -s john ../run/unafs

../run/unique: ../run/john
	ln -s john ../run/unique

../run/john.bin: $(JOHN_OBJS)
	$(LD) $(LDFLAGS) $(JOHN_OBJS) -o ../run/john.exe
	if exist ..\run\john.bin del ..\run\john.bin
	ren ..\run\john.exe john.bin

../run/john.com: john.com
	copy john.com ..\run\john.com

../run/unshadow.com: john.com
	copy john.com ..\run\unshadow.com

../run/unafs.com: john.com
	copy john.com ..\run\unafs.com

../run/unique.com: john.com
	copy john.com ..\run\unique.com

john.com: john.asm
	@echo Use Borland TASM/TLINK to make JOHN.COM

../run/john.exe: $(JOHN_OBJS)
	$(LD) $(JOHN_OBJS) -lkernel32 -o ../run/john.exe
	strip ../run/john.exe

../run/unshadow.exe: symlink.c
	$(CC) symlink.c -o ../run/unshadow.exe
	strip ../run/unshadow.exe

../run/unafs.exe: symlink.c
	$(CC) symlink.c -o ../run/unafs.exe
	strip ../run/unafs.exe

../run/unique.exe: symlink.c
	$(CC) symlink.c -o ../run/unique.exe
	strip ../run/unique.exe

# Inlining the S-boxes produces faster code, as long as they fit in the cache
# (that is, on RISC with at least 8 Kb of L1 code cache).
DES_bs_b.o: DES_bs_b.c DES_bs_s.c
	$(CC) $(CFLAGS) $(OPT_INLINE) DES_bs_b.c

# I prefer to distribute Matthew Kwan's S-box file unmodified...
DES_bs_s.c: nonstd.c
	$(SED) "s/unsigned long/ARCH_WORD/" nonstd.c > DES_bs_s.c

.c.o:
	$(CC) $(CFLAGS) $(OPT_NORMAL) $*.c

.S.o:
	$(AS) $(ASFLAGS) $*.S

# We don't have any files with .s suffix, this is for compiling in DOS only
.s.o:
	$(AS) $(ASFLAGS) $*.S

depend:
	makedepend -fMakefile.dep -Y *.c 2>> /dev/null

clean:
	$(RM) $(PROJ) $(PROJ_DOS) $(PROJ_WIN32)
	$(RM) ../run/john.exe *.o *.bak core
	$(RM) detect bench generic.h arch.h sparc.h tmp.s DES_bs_s.c
	$(CP) $(NULL) Makefile.dep

dist:
	$(MAKE) clean
	cd ../..; \
	chmod 600 $(JOHN_DIST); \
	chmod 700 $(JOHN_ARCHIVE)/src/*.sh $(JOHN_ARCHIVE)/run/mailer; \
	touch $(JOHN_DIST); \
	tar czf $(JOHN_ARCHIVE)/$(JOHN_ARCHIVE).tar.gz $(JOHN_DIST)

include Makefile.dep
