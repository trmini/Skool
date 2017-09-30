CFLAGS=-g -I.
LFLAGS=

all : facetrain facetrain1 facetrain2 facetrain3 hidtopgm

facetrain: facetrain.o imagenet.o backprop_initr.o pgmimage.o
	$(CC) ${LFLAGS} facetrain.o imagenet.o backprop_initr.o pgmimage.o \
	      -o facetrain -lm

facetrain1: facetrain.o imagenet1.o backprop_initr.o pgmimage.o
	$(CC) ${LFLAGS} facetrain.o imagenet1.o backprop_initr.o pgmimage.o \
	      -o sun_glasses -lm
	      
facetrain2: facetrain2.o imagenet2.o backprop_initr.o pgmimage.o
	$(CC) ${LFLAGS} facetrain2.o imagenet2.o backprop_initr.o pgmimage.o \
	      -o people -lm
	      
facetrain3: facetrain3.o imagenet3.o backprop_initr.o pgmimage.o
	$(CC) ${LFLAGS} facetrain3.o imagenet3.o backprop_initr.o pgmimage.o \
	      -o face_direction -lm
	      
hidtopgm: hidtopgm.o pgmimage.o backprop_initr.o
	$(CC) ${LFLAGS} hidtopgm.o pgmimage.o backprop_initr.o \
	      -o hidtopgm -lm

outtopgm: outtopgm.o pgmimage.o backprop_initr.o
	$(CC) ${LFLAGS} outtopgm.o pgmimage.o backprop_initr.o \
	      -o outtopgm -lm

facetrain_init0: facetrain.o imagenet.o backprop_init0.o pgmimage.o
	$(CC) ${LFLAGS} facetrain.o imagenet.o backprop_init0.o pgmimage.o \
	      -o facetrain_init0 -lm

backprop_initr.o: backprop.c backprop.h
	$(CC) ${CFLAGS} -c backprop.c
	mv backprop.o backprop_initr.o

backprop_init0.o: backprop.c backprop.h
	$(CC) ${CFLAGS} -c -DINITZERO backprop.c
	mv backprop.o backprop_init0.o
