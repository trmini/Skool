CC = gcc 
INCDIR = /usr/local/include/pari
LIBDIR = /usr/local/lib
CFLAGS = -I$(INCDIR) -L$(LIBDIR)

all: verify

verify: verify.c
	$(CC) $(CFLAGS) -o verify verify.c /usr/local/lib/libpari.a -lm
