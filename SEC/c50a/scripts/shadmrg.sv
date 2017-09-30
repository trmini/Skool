#!/bin/sh
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

SHADOW=/etc/shadow
PASSWD=/etc/passwd

(
	sed -e 's/^/STAG:/' < $SHADOW
	sed -e 's/^/PTAG:/' < $PASSWD
) |
awk -F: '
BEGIN {
	OFS=":";
}
$1 == "STAG" {
	pw[$2] = $3;
	next;
}
$1 == "PTAG"{
	$3 = pw[$2];
	print $0;	

}' |
sed -e 's/^PTAG://'
