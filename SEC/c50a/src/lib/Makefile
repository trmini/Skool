###
# This program was written by and is copyright Alec Muffett 1991,
# 1992, 1993, 1994, 1995, and 1996, and is provided as part of the
# Crack v5.0 Password Cracking package.
#
# The copyright holder disclaims all responsibility or liability with
# respect to its usage or its effect upon hardware or computer
# systems, and maintains copyright as set out in the "LICENCE"
# document which accompanies distributions of Crack v5.0 and upwards.
###

XLIB=	$(XDIR)/libc5.a
CFLAGS= $(XCFLAGS)
CC=	$(XCC)

SRC=	dawglib.c debug.c rules.c stringlib.c
OBJ=	dawglib.o debug.o rules.o stringlib.o

$(XLIB): $(SRC) ../lib/libcrack.h
	( make "XDIR=$(XDIR)" "XCFLAGS=$(XCFLAGS)" $(OBJ) || exit 1 )	
	ar rv $(XLIB) $(OBJ)
	-ranlib $(XLIB)
	@echo all made in lib

clean:
	rm -f $(OBJ) *~

clobber: clean
	rm -f $(XLIB)

../lib/libcrack.h:

