#include "Poly.h"

Poly::Poly() {
  for (int i = 0; i <= MAX_DEGREE; i++) {
		  deg[i] = 0;
		  a[i] = 0;
  }
  max_deg = -1;
}

Poly::Poly(const Poly &OrgPoly) {
	max_deg = OrgPoly.max_deg;
	for (int i = MAX_DEGREE - max_deg; i <= MAX_DEGREE; i++) {
		deg[i] = OrgPoly.deg[i];
		a[i] = OrgPoly.a[i];
	}
}

Poly::~Poly() {
}

Poly& Poly::operator = (const Poly &Right) {
	if (this == &Right) {
		return *this;
	}
	max_deg = Right.max_deg;
	if (max_deg == -1) {
		for (int i = 0; i<MAX_DEGREE; i++) {
			deg[i] = 0;
			a[i] = 0;
			return *this;
		}
	}
	for (int i = 0; i < MAX_DEGREE - max_deg; i++) {
		deg[i] = 0;
		a[i] = 0;
	}
	for (int i = MAX_DEGREE - max_deg; i <= MAX_DEGREE; i++) {
		deg[i] = Right.deg[i];
		a[i] = Right.a[i];
	}
	return *this;
}

Poly& Poly::operator = (const int& number) {
	if (number == 0) {
		for (int i = 0; i <= MAX_DEGREE; i++) {
				a[i] = 0;
				deg[i] = 0;
		}
		max_deg = -1;
	}
	return *this;
}

int Poly::operator == (const int& number) {
	if (number == 0 && max_deg == -1) {
			return 1;
	}
	return 0;
}
			
void Poly::getval(int degree, char *string_num) {
		if (max_deg < degree) {
				max_deg = degree;
		}
		deg[MAX_DEGREE-degree] = 1;
		if (string_num[0] == '\0') {
			a[MAX_DEGREE-degree] = 1;;
		}
		else a[MAX_DEGREE-degree].atobig(string_num);
}

Poly operator + (Poly &Left, Poly &Right) {
		Poly result;
		if (Left == 0 && Right == 0) {
				return result;
		}
		if (Left == 0) {
				result = Right;
				return result;
		}
		if (Right == 0) {
				result = Left;
				return result;
		}
		for (int i = 0; i <= MAX_DEGREE; i++) {
				if (Left.deg[i] == 0 && Right.deg[i] == 0) {
						continue;
				}
				result.deg[i] = 1;
				if (Left.deg[i] == 0) {
						result.a[i] = Right.a[i];
						continue;
				}
				if (Right.deg[i] == 0) {
						result.a[i] = Left.a[i];
						continue;
				}
				
				result.a[i] = Left.a[i] + Right.a[i];
				if (result.a[i] == 0) {
						result.deg[i] = 0;
				}
		}
		result.setMaxDeg();
		return result;
}


Poly operator - (Poly &Left, Poly &Right) {
		Poly result;
		if (Left == 0 && Right == 0) {
				return result;
		}
		if (Left == 0) {
				Big_int ZERO = 1;
				result.max_deg = Right.max_deg;
				for (int i = MAX_DEGREE - Right.max_deg; i <= MAX_DEGREE; i++) {
					if (Right.deg[i]) {
						result.a[i] = ZERO - Right.a[i];
						result.deg[i] = 1;
					}
				}
				return result;
		}
		if (Right == 0) {
				result = Left;
				return result;
		}
		Big_int ZERO = 0;
		for (int i = 0; i <= MAX_DEGREE; i++) {
				if (Left.deg[i] == 0 && Right.deg[i] == 0) {
						continue;
				}
				result.deg[i] = 1;
				if (Left.deg[i] == 0) {
						result.a[i] = (ZERO - Right.a[i]);
						continue;
				}
				if (Right.deg[i] == 0) {
						result.a[i] = Left.a[i];
						continue;
				}
				result.a[i] = Left.a[i] - Right.a[i];
				if (result.a[i] == 0) {
						result.deg[i] = 0;
				}
		}
		result.setMaxDeg();
		return result;
}

Poly operator * (Poly &Left, Poly &Right) {
		Poly result;
		if (Left == 0 || Right == 0) {
				return result;
		}
		result.max_deg = Left.max_deg + Right.max_deg;
		for (int i = MAX_DEGREE-Left.max_deg; i <= MAX_DEGREE; i++) {
				if (Left.deg[i] == 0) {
						continue;
				}
				for (int j = MAX_DEGREE-Right.max_deg; j <= MAX_DEGREE; j++) {
						if (Right.deg[j] == 0) {
								continue;
						}
						int deg_tmp = i + j - MAX_DEGREE;
						if (result.deg[deg_tmp] == 0) {
								result.deg[deg_tmp] = 1;
						}
						Big_int temp = Left.a[i]*Right.a[j];
						result.a[deg_tmp] = result.a[deg_tmp] + temp;
				}
		}
		for (int i = MAX_DEGREE-result.max_deg; i <= MAX_DEGREE; i++) {
				if (result.deg[i] !=0 && result.a[i] == 0) {
						result.deg[i] = 0;
				}
		}
		result.setMaxDeg();
		return result;
		
}

void divide(Poly &A, Poly &B, Poly &Q, Poly &R) {
		// Assume B is monic
		if (B == 0) {
				cout <<"Division by 0\n";
				exit(1);
		}
		if (A == 0) {
				Q = 0;
				R = 0;
				return;
		}
		
		if (A.max_deg < B.max_deg) {
				Q = 0;
				R = A;
				return;
		}
		
		Q.max_deg = A.max_deg - B.max_deg;
		R = A;
		int count = 0;
		while (R.max_deg >= B.max_deg) {
				Poly temp, q;
				int i = MAX_DEGREE - (R.max_deg-B.max_deg);
				Q.deg[i] = 1;
				Q.a[i] = R.a[MAX_DEGREE-R.max_deg];
				q.max_deg = MAX_DEGREE - i;
				q.deg[i] = 1;
				q.a[i] = Q.a[i];
				temp = q*B;
				q = R - temp;
				R = q;
		}
		
}

Poly operator / (Poly &Left, Poly &Right) {
		Poly Q, R;
		divide(Left, Right, Q, R);
		return Q;
}

Poly operator % (Poly &Left, Poly &Right) {
		Poly Q, R;
		divide(Left, Right, Q, R);
		return R;
}

ostream& operator << (ostream &os, Poly& Right) {
		
		if (Right.max_deg == -1) {
				os<<"0";
				return os;
		}
		unsigned int i = MAX_DEGREE - Right.max_deg;
		if ( !(Right.a[i] == 1)) {
				os<<Right.a[i]<<" * ";
		}
		if (Right.max_deg > 1) {
			os<<"x^"<<Right.max_deg;
		}
		else {
			if (Right.max_deg == 1) {
				os<<"x";
			}
			else {
				return os;
			}
		}
		i++;
		for (; i < MAX_DEGREE-1; i++) {
				if (Right.deg[i]) {
						os<<" + ";
						if ( !(Right.a[i] == 1)) {
								os<<Right.a[i]<<" * ";
						}
						os<<"x^"<<MAX_DEGREE - i;
				}
		}
		if (Right.deg[MAX_DEGREE-1]) {
			os<<" + ";
			if ( !(Right.a[i] == 1)) {
				os<<Right.a[i]<<" * ";
			}
			os<<"x";
		}
		if (Right.deg[MAX_DEGREE]) {
			os<<" + "<<Right.a[MAX_DEGREE]<<";";
		}
		else {
			os<<";";
		}
		return os;
}

void Poly::setMaxDeg() {
  int i;
  for (i = 0; i < MAX_DEGREE; i++) {
	  if (deg[i] == 1 && !(a[i]==0)) {
	    break;
	  }
  }
  max_deg = MAX_DEGREE - i;
  if (max_deg == 0 && a[MAX_DEGREE] == 0) {
	max_deg = -1;
  }
}
