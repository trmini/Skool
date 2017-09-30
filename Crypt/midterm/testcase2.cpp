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
	Poly p, q, s, t, u;
	Big_int ONE = 1;
	Big_int TWO = 2;
	PRIME.atobig("1979");
	
	p.getval(6,"1");
	p.getval(4,"1800");
	p.getval(0,"1");
	
	
	q.getval(4,"1");
	q.getval(2,"1800");
	
	cout<<"p = "<<p<<" q = "<<q<<"\n";
	s = p/q;
	u = s*q;
	s = p%q;
	t = u + s;
	cout<<t<<"\n";
	cout<<s<<"\n";
	a.atobig("22222222222");
	calculator(q, p, a, t);
	cout<<"after cal "<<t<<"\n";
        return 1;

	b.atobig("22222222222");
	b.half();
	cout<<"b = " <<b<<"\n";
	b.sub1();
	cout<<"b = " <<b<<"\n";
	return 1;
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
