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

CFLAGS= $(XCFLAGS) -I../lib
CC=     $(XCC)

XLIB=$(XDIR)/libc5.a $(XLIBS)

EXE=    $(XDIR)/dawg \
	$(XDIR)/dictfilt \
	$(XDIR)/kickdict \
	$(XDIR)/cracker

#------------------------------------------------------------------

all:    $(EXE)
	@echo all made in util

clean:
	rm -f *.o *~

clobber: clean
	rm -f $(EXE)

$(XLIB):

#------------------------------------------------------------------

$(XDIR)/cracker:        cracker.c $(XLIB)
	mkcracker $(XDIR) "XDIR=$(XDIR)" "XCFLAGS=$(XCFLAGS)" "XCC=$(XCC)" "XLIB=$(XLIB)"

# ...targets which are known to mkcracker...

$(XDIR)/stdlib-cracker: cracker.c $(XLIB)
	$(CC) $(CFLAGS) -c elcid.c
	$(CC) $(CFLAGS) -o $(XDIR)/cracker cracker.c elcid.o $(XLIB)
	date > $@

$(XDIR)/libdes-cracker: cracker.c $(XLIB)
	$(CC) $(CFLAGS) -c elcid.c
	$(CC) $(CFLAGS) -o $(XDIR)/cracker cracker.c elcid.o $(XLIB) ../libdes/libdes.a
	date > $@

$(XDIR)/ufc-cracker: cracker.c $(XLIB)
	$(CC) $(CFLAGS) -DINITDES -DFCRYPT -c elcid.c
	$(CC) $(CFLAGS) -o $(XDIR)/cracker cracker.c elcid.o $(XLIB) ../ufc-crypt/libufc.a
	date > $@

$(XDIR)/gnu-cracker: cracker.c $(XLIB)
	$(CC) $(CFLAGS) -c elcid.c
	$(CC) $(CFLAGS) -o $(XDIR)/cracker cracker.c elcid.o $(XLIB) ../crypt/libufc.a
	date > $@

#------------------------------------------------------------------

$(XDIR)/dawg: dawg.c $(XLIB)
	$(CC) $(CFLAGS) -o $(XDIR)/dawg dawg.c $(XLIB)
	rm -f $(XDIR)/xdawg
	ln $(XDIR)/dawg $(XDIR)/xdawg

#------------------------------------------------------------------

# don't you just love dependency graphs?
$(XDIR)/dictfilt: dictfilt.c $(XLIB) $(XDIR)/cracker
	$(CC) $(CFLAGS) -o $(XDIR)/dictfilt dictfilt.c elcid.c $(XLIB)

#------------------------------------------------------------------

$(XDIR)/kickdict: kickdict.c $(XLIB)
	$(CC) $(CFLAGS) -o $(XDIR)/kickdict kickdict.c $(XLIB)

