# makefile was written by Dien Trang Luu
CC=gcc
OPS=-g
LIBS=-lnsl -lsocket
LINT=lint
LOPS=-Xarch=v9 -m -x -u
LLIBS=

all:            part1 part2 part 3 part4

part1:          syncdir1 syncdaemon1

part2:          syncdir1 syncdaemon2

part3:          syncdir2 syncdaemon3

part4:          syncdir2 syncdaemon4 syncstatus

 
syncdir1:       syncdir1.c uici.o uici.h
		$(CC) $(OPS) -o syncdir1 syncdir1.c uici.o $(LIBS)

syncdir2:       syncdir2.c uici.o uici.h
		$(CC) $(OPS) -o syncdir2 syncdir2.c uici.o $(LIBS)

syncdaemon1:    syncdaemon1.c uici.o uici.h
		$(CC) $(OPS) -o syncdaemon1 syncdaemon1.c uici.o $(LIBS)

syncdaemon2:    syncdaemon2.c uici.o uici.h
		$(CC) $(OPS) -o syncdaemon2 syncdaemon2.c uici.o $(LIBS)

syncdaemon3:    syncdaemon3.c uici.o uici.h
		$(CC) $(OPS) -o syncdaemon3 syncdaemon3.c uici.o $(LIBS)

syncdaemon4:    syncdaemon4.c uici.o uici.h
		$(CC) $(OPS) -o syncdaemon4 syncdaemon4.c uici.o $(LIBS)

syncfix:        syncfix.c uici.o uici.h
		$(CC) $(OPS) -o syncfix syncfix.c uici.o $(LIBS)

syncstatus:     syncstatus.c uici.o uici.h
		$(CC) $(OPS) -o syncstatus syncstatus.c uici.o $(LIBS)

uici.o:         uici.c uici.h
		$(CC) $(OPS) -c uici.c $(LIBS)

lint1_1:
		$(LINT) $(LOPS) syncdir1.c uici.c

lint1_2:
		$(LINT) $(LOPS) syncdaemon1.c uici.c

lint2_1:
		$(LINT) $(LOPS) syncdir1.c uici.c

lint2_2:
		$(LINT) $(LOPS) syncdaemon2.c uici.c

lint3_1:
		$(LINT) $(LOPS) syncdir2.c uici.c

lint3_2:
		$(LINT) $(LOPS) syncdaemon3.c uici.c

lint4_1:
		$(LINT) $(LOPS) syncdir2.c uici.c

lint4_2:
		$(LINT) $(LOPS) syncdaemon4.c uici.c

lint4_3:
		$(LINT) $(LOPS) syncstatus.c uici.c

clean:
		rm -f core *.o client server
