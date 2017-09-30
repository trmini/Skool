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

CC= gcc
CFLAGS= -O2 -g
LIB= ../src/libdes/libdes.a
#LIB= -lcrypt

all: brute brute-gen

brute: brute.c
	$(CC) -O2 -o $@ brute.c $(LIB)

brute-gen: brute.c
	$(CC) -DBRUTEGEN=1 -O2 -o $@ brute.c

fips: fips.o
	$(CC) -O2 -o $@ fips.o

clean:
	rm -f *.o *~ brute brute-gen core

test: all
	./brute-gen A-C a-b 0-2

