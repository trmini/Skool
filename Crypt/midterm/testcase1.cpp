#include <math.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <ctype.h>
#include "Big_int.h"
#include "Poly.h"
#include "calculator.h"

#define MAX_DECIMAL 300

Big_int PRIME = 0;

int main() {
	Big_int a, b, c, r;
	Big_int ONE = 1;
	Big_int TWO = 2;
	PRIME.atobig("813583753573957395839573993");
	a.atobig("22222222222");
	b.atobig("11111111110");
	cout<<"PRIME = "<<PRIME<<"\n";
	cout<<"a = " <<a<<"\n";
	cout<<"b = " <<b<<"\n";
	c = a/TWO;
	//c = b*a;
	cout<<"a/2 = " <<c<<"\n";
	b.sub1();
	cout<<"b-1 = " <<b<<"\n";
	b = c/TWO;
	cout<<"c/2 = " <<b<<"\n";
	return 1;
	a = c / b;
	r = c % b;
	cout<<"a = " <<a<<"\n";
	cout<<"r = " <<r<<"\n";
	b = c / a;
	r = c % a;
	cout<<"b = " <<b<<"\n";
	cout<<"r = " <<r<<"\n";
	return 1;
}
