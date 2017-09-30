//Course    : CS 5973 Cryptography
//Big_int class
//Assume that type int is 4 bytes

#include <iostream.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#ifndef BIG_INT_CLASS
#define BIG_INT_CLASS
typedef unsigned int TWOBYTES;
const TWOBYTES BASE = 10000;

class Big_int {
  private:

	int sign; // 1 positive, -1 negative, 0 zero
	TWOBYTES *num_p;
	unsigned int length; //Number of TWOBYTES blocks used

	void getSpace(unsigned int o_length);
	void optimize();

  public:
	/* Construction and Destruction */
	Big_int();
	Big_int(const Big_int &OrgBig_num);
	Big_int(const TWOBYTES number);
	~Big_int();

	Big_int &operator = (const Big_int &Right);
	Big_int &operator = (const TWOBYTES o_num);
	int isOdd();
	void atobig(char *string_num);
	
	int operator == (const Big_int &Right);
	//friend int operator != (const Big_int &Right) const;
	int operator == (const TWOBYTES number);
	//friend int operator < (const Big_int &Right) const;
	int operator > (const Big_int &Right);
	//friend int operator <= (const Big_int &Right) const;
	//friend int operator >= (const Big_int &Right) const;


	friend Big_int operator + (Big_int &Left, Big_int &Right);
	friend Big_int operator - (Big_int &Left, Big_int &Right);
	friend Big_int operator * (Big_int &Left, Big_int &Right);
	friend Big_int operator / (Big_int &Left, Big_int &Right);
	friend Big_int operator % (Big_int &Left, Big_int &Right);

	friend ostream& operator << (ostream &os, Big_int &Rigth);
	friend void divide(Big_int &A, Big_int &B, Big_int &Q, Big_int &R);
	void half();
	void sub1();


	inline unsigned int Length();
	inline int Sign();
	inline void Set_sign(int o_sign);
};

inline unsigned int Big_int::Length() {
  return length;
}
inline int Big_int::Sign() {
  return sign;
}

inline void Big_int::Set_sign(int o_sign) {
  if (sign != 0) {
	  sign = o_sign;
  }
}
#endif
