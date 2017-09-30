#include <math.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <ctype.h>
#include "Big_int.h"
#include "Poly.h"
#include "calculator.h"

#define MAX_DECIMAL 400

Big_int PRIME;

int get_num(ifstream& infile, char *num_string);
void get_poly(ifstream& infile, Poly& f );

int main(int argc, char *argv[]) {
	ifstream infile;
	char num_string[MAX_DECIMAL];
	unsigned int cnt = 0;
	int sign_flag = 1;
	unsigned long time = 0;
	Big_int n;
	Poly f, g, result;
	char temp;
	if (argc != 2) {
		cout<<"Usage: "<<argv[0]<<" <filename>\n";
		return 0;
	}
	infile.open(argv[1]);
	if (infile.fail()) {
		cout<<"Error while opening file "<<argv[1]<<"!\n";
		return 0;
	}
	temp = infile.peek();
	while (!isdigit(temp)) {
		if (temp == EOF) {
		  cout<<"Syntax error: cannot find p\n";
		  return 0;
		}
		infile.get();
		temp = infile.peek();
	}
	get_num(infile, num_string);
	PRIME.atobig(num_string);
	// Finish getting PRIME
	 
	temp = infile.peek();
	while (!isdigit(temp)) {
		if (temp == EOF) {
		  cout<<"Syntax error: cannot find n\n";
		  return 0;
		}
		infile.get();
		temp = infile.peek();
	}
	get_num(infile, num_string);
	n.atobig(num_string);
	// Finish getting n
	// Start to get f(x)

	temp = infile.peek();
	while (!isdigit(temp) && temp != 'x') {
		if (temp == EOF) {
		  cout<<"Syntax error: cannot find f(x)\n";
		  return 0;
		}			
		infile.get();
		temp = infile.peek();
	}
	get_poly(infile, f);
	
	temp = infile.peek();
	while (!isdigit(temp) && temp != 'x') {
		if (temp == EOF) {
		  cout<<"Syntax error: cannot find g(x)\n";
		  return 0;
		}			
		infile.get();
		temp = infile.peek();
	}
	get_poly(infile, g);
	// End of get Data
	// Start main program
	cout<<"p = "<<p<<"\n";
	cout<<"n = "<<n<<"\n";
	cout<<"F[x] = "<<f<<"\n";
	cout<<"G[x] = "<<g<<"\n";
	calculator(f, g, n, result);
	cout<<result<<"\n";
	infile.close();
	return 1;
}

int get_num(ifstream &infile, char *num_string) {
	char temp;
	int cnt = 0;
	temp = infile.peek();
	while (temp != ';' && temp != EOF) {
		if (isdigit(temp)) {
			num_string[cnt] = temp;
			cnt++;
		}
		infile.get();
		temp = infile.peek();
	}
	num_string[cnt] = '\0';
	return cnt;
}

void get_poly(ifstream &infile, Poly &f) {
  char temp;
  char num_string[MAX_DECIMAL] = "";
  temp = infile.peek();
  unsigned int cnt = 0;
  while (temp != ';') {
    if (temp == EOF) {
      cout<<"Syntax Error for getting Polynomial\n";
      exit(0);
    }
    if (temp == 'x') {
      infile.get();
      infile.get();
      int degree;
      infile >> degree;
      f.getval(degree, num_string);
      cnt = 0;
      num_string[0] = '\0';
      infile.get(temp);
      continue;
    }
  
    if ( !isdigit(temp) && temp != 'x' ) {
      infile.get();
      temp = infile.peek();
    }
    
    if (isdigit(temp)) {
      num_string[cnt] = temp;
      cnt++;
      num_string[cnt] = '\0';
      infile.get();
      temp = infile.peek();
    }
  }
  if (num_string[0] != '\0') {
	  f.getval(0, num_string);
  }
  return;  
}
