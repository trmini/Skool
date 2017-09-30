#include <time.h>
#include "calculator.h"

void calculator(Poly& f, Poly& g, Big_int& n, Poly& result) {
  //Using the repeated squaring method
  Poly b = g % f;
  Big_int ONE, TWO, N;
  ONE = 1;
  TWO = 2;
  N = n;
  int first = 1;
  Poly temp;
  if (N == 1) {
	  result = b;
	  return;
  }
  while ( !(N == 1)) {
	  if (N.isOdd()) {
		  N.sub1();
		  if (first) {
			  result = b;
			  first = 0;
		  }
		  temp = result*b;
		  result = temp % f;
	  }
	  temp = b * b;
	  b = temp % f;
	  //cout<<"b="<<b<<"\n";
	  N.half();
  }
  if (first) {
	  result = b;
  }
  else {
	  temp = result * b;
	  result = temp % f;
  }
  cout<<result<<"\n";
  return;
}
