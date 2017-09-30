#include <stdio.h>
#include <stdlib.h>
#include "pari.h"

int main (int argc, char *argv[]) {
  GEN p = 29579385439865947694694684968492467204765762471;
  GEN a = gmod(9817492759375943, p);
  GEN b = 7358469577957;
  GEN k = 30;
  GEN m = 630607954;
  GEN xb = gmod(15747883366601598527268816398234122460533203151, p);
  GEN yb = gmod(20436308842811998600725239736561666084747562678, p);
  GEN X, Y, tmp;
  GEN Pm;

  int i = 1;
  
  pdari_init(100000,2);

  m = k*m;

  for (i = 1; i <= k; i++) {
	  X = gmod(m + i, p);
	  tmp = gmod((gpow(X, 3, 1) + gmul(a,X) + gmod(b,p)), p);
	  Y = sqrt(tmp);
	  if (!gcmp(tmp, gmul(Y,Y))) {
		  break;
	  }
  }
  Pm = [X,Y];
  return 1;
}
	  
/*y2 = x3 + 9817492759375943  x + 7358469577957 over nite
eld Fp with p = 29579385439865947694694684968492467204765762471. Use  = 30.
The base point is B = (5; 22628679036034363552380188982145515602413799729).
My public key is [15747883366601598527268816398234122460533203151;
20436308842811998600725239736561666084747562678). */
