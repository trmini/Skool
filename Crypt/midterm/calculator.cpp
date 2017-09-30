#include <time.h>
#include "calculator.h"

void calculator(Poly& f, Poly& g, Big_int& n, Poly& result) {
  //Using the repeated squaring method
  Poly b = g % f;
  Big_int N = n;
  int first = 1;
  if (N == 1) {
	  result = b;
	  return;
  }
  while ( !(N == 0)) {
	  if (N.isOdd()) {
		  if (first) {
			  cout<<N<<"\n";
			  result = b;
			  first = 0;
		  }
		  else {
	  		Poly temp;
			temp = b * result;
		  	result = temp % f;
		  }
		  N.sub1();
		  cout<<N<<"\n";
		  
	  }
	  else {
		  Poly temp;
		  temp = b*b;
		  b = temp % f;
		  N.half();
	  }
  }
  return;
}
