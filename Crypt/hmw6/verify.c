#include <stdio.h>
#include <stdlib.h>
#include "pari.h"

int main (int argc, char *argv[]) {
  GEN h;
  GEN q, p, g, y; 
  GEN R, S, s, u1, u2, x;
  FILE *filein;
  int i;
  int d = 1;

  if (argc != 2) {
	  printf("Usage: %s <filename>\n", argv[0]);
	  exit(1);
  }
  filein = fopen(argv[1], "r");
  if (filein == NULL) {
	  printf("Cannot open %s\n", argv[1]);
  }
  pari_init(100000,2);
  q = lisGEN(filein);
  p = lisGEN(filein);
  g = lisGEN(filein);
  g = gmodulo(g, p);
  y = lisGEN(filein);
  y = gmodulo(y, p);
  printf("Please enter your SSN: ");
  h = lisGEN(stdin);

  for (i = 0; i < 16; i++) {
    R = lisGEN(filein);
    S = lisGEN(filein);
    s = ginvmod(S, q);
    u1 = gmod(gmul(s,h),q);
    u2 = gmod(gmul(s,R),q);
    x = gmul(gmod(gpow(g,u1,d), p), gmod(gpow(y,u2,d),p));
    x = compo(x, 2);
    x = gmod(x, q);
    if (!gcmp(x, R)) {
      printf("Signature number %i is verified!\n", i+1);
      break;
    }
  }
  if (i == 16) {
    printf("Cannot verify any signature!\n");
  }
  return 1; 
}
