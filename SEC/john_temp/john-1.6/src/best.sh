#!/bin/sh
#
# This file is part of John the Ripper password cracker,
# Copyright (c) 1996-98 by Solar Designer
#

[ $# -eq 4 ] || exit 1

MAKE=$1
DES_DEPEND=$2
MD5_DEPEND=$3
BF_DEPEND=$4

# Detect the best standard DES algorithm

MAX=0
DES_BEST=1

for MODE in 1 2 3 4 5; do
	if ./detect $MODE 1 0 0 0 > arch.h; then
		rm -f $DES_DEPEND bench
		echo "Compiling: DES benchmark (code version #$MODE)"
		$MAKE bench || exit 1
		RES=`./bench 1` || exit 1
		if [ $RES -gt $MAX ]; then
			MAX=$RES
			DES_BEST=$MODE
		fi
	fi
done

./detect $DES_BEST 0 0 0 0 > arch.h
rm -f $DES_DEPEND bench

echo "Compiling: DES benchmark (code version #$DES_BEST, no key copying)"
$MAKE bench || exit 1
RES=`./bench 1` || exit 1
if [ $RES -gt $MAX ]; then
	DES_COPY=0
else
	DES_COPY=1
fi

# Check if bitslice DES is faster

./detect $DES_BEST $DES_COPY 1 0 0 > arch.h
rm -f $DES_DEPEND bench

echo "Compiling: DES benchmark (bitslice)"
$MAKE bench || exit 1
RES=`./bench 1` || exit 1
if [ $RES -gt $MAX ]; then
	DES_BS=1
else
	DES_BS=0
fi

# Detect the best MD5 algorithm

MAX=`./bench 2` || exit 1

./detect $DES_BEST $DES_COPY $DES_BS 1 0 > arch.h
rm -f $MD5_DEPEND bench

echo "Compiling: MD5 benchmark (immediate values)"
$MAKE bench || exit 1
RES=`./bench 2` || exit 1
if [ $RES -gt $MAX ]; then
	MD5_IMM=1
else
	MD5_IMM=0
fi

# Detect the best Blowfish algorithm

MAX=`./bench 3` || exit 1

./detect $DES_BEST $DES_COPY $DES_BS $MD5_IMM 1 > arch.h
rm -f $BF_DEPEND bench

echo "Compiling: Blowfish benchmark (scale)"
$MAKE bench || exit 1
RES=`./bench 3` || exit 1
if [ $RES -gt $MAX ]; then
	BF_SCALE=1
else
	BF_SCALE=0
fi

# Produce generic.h, make sure everything is rebuilt with detected options,
# and do some cleanup

./detect $DES_BEST $DES_COPY $DES_BS $MD5_IMM $BF_SCALE > generic.h
rm -f $DES_DEPEND $MD5_DEPEND $BF_DEPEND bench detect best.o detect.o arch.h
