#include <stdio.h>
#include "pari.h"

int main() {
	GEN x = 5;
	GEN y = 7;
	GEN R;

	R = gpow(gmod(x,y), (GEN) -1, 1);
	
