//Dien Trang Luu
#include <iostream.h>
#include "Big_int.h"

#ifndef POLY_CLASS
#define POLY_CLASS
#define MAX_DEGREE 20

class Poly {
	private:
		Big_int a[MAX_DEGREE+1];
		unsigned int deg[MAX_DEGREE+1];
		int max_deg; // -1 indicates  Zero Polynomial

		void setMaxDeg();
		friend void divide(Poly &A, Poly &B, Poly &Q, Poly &R);
		
	public:
		/* Construction and Destruction */
		Poly();
		Poly(const Poly& OrgPoly);
		~Poly();

 	        Poly& operator = (const Poly& Right);
		Poly& operator = (const int& number); //Used for F[x] = 0
		
		int operator == (const int& number); //Used for F[x] == 0
		void getval(int degree, char *num_string);

		friend Poly operator + (Poly &Left, Poly &Right);
	 	friend Poly operator - (Poly &Left, Poly &Right);
	    	friend Poly operator * (Poly &Left, Poly &Right);
		friend Poly operator / (Poly &Left, Poly &Right);
		friend Poly operator % (Poly &Left, Poly &Right);
		
		friend ostream &operator << (ostream &os, Poly &Right);
};
#endif
