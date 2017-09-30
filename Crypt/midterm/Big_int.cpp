#include "Big_int.h"
extern Big_int PRIME;

Big_int::Big_int() {
  num_p = new TWOBYTES[1];
  num_p[0] = (TWOBYTES) 0;
  length = 1;
  sign = 0;
}

Big_int::Big_int(const Big_int &OrgBig_num) {
  length = OrgBig_num.length;
  sign = OrgBig_num.sign;
  num_p = new TWOBYTES[length];
  memcpy(num_p, OrgBig_num.num_p, length*sizeof(TWOBYTES));
}

Big_int::Big_int(const TWOBYTES number) {
  length = 1;
  num_p = new TWOBYTES[length];
  num_p[0] = number;
  if (number == 0) {
    sign = 0;
  }
  else {
    sign = 1;
  }
}

Big_int::~Big_int() {
  delete num_p;
  num_p = NULL;
}

Big_int& Big_int::operator = (const Big_int &Right) {
  if (this == &Right) {
	return *this;
  }
  
  if (num_p != NULL) {
	delete num_p;
  }

  length = Right.length;
  sign = Right.sign;
  num_p = new TWOBYTES[length];
  memcpy(num_p, Right.num_p, length*sizeof(TWOBYTES));
  return *this;
}

Big_int& Big_int::operator = (TWOBYTES o_num) {
  if (num_p != NULL) {
    delete num_p;
  }
  length = 1;
  num_p = new TWOBYTES[1];
  num_p[0] = o_num;
  if (o_num == 0) {
    sign = 0;
  }
  else {
    sign = 1;
  }
  return *this;
}

int Big_int::operator == (const Big_int &Right) {
  if (length != Right.length) { return 0;}
  if (memcmp(num_p, Right.num_p, length*sizeof(TWOBYTES))) {
	  return 0;
  }
  return 1;
}

int Big_int::operator == (const TWOBYTES number) {
	if (length == 1 && num_p[0] == number) {
		return 1;
	}
	return 0;
}

int Big_int::operator > (const Big_int &Right) {
  if (length < Right.length) { return 0;}
  if (length > Right.length) { return 1;}
  long i = (long) length-1;
  while (i > 0) {
	if (num_p[i] != Right.num_p[i]) {
		break;
	}
	i--;
  }
  if (num_p[i] > Right.num_p[i]) {
	  return 1;
  }
  return 0;
}

// C = (A+B) mod P
// Both 0 < A, B < P
Big_int operator + (Big_int &Left, Big_int &Right) {
	  
  if (Left.length < Right.length) {
	return (Right + Left);
  }
  /* Right is smaller than or equal to Left */
  Big_int result;
  if (Left == 0) {
	  result = Right;
	  return result;
  }

  if (Right == 0) {
	  result = Left;
	  return result;
  }
  result.getSpace(Left.length);
  unsigned int i;
  TWOBYTES sum = 0;
  TWOBYTES carry = 0;
  for ( i = 0; i < Left.length; i++) {
	if ( i < Right.length ) {
	  sum = carry + Left.num_p[i] + Right.num_p[i];
	}
	else {
	  if (carry == 0) {
	   memcpy(result.num_p+i, Left.num_p+i, (result.length-i)*sizeof(TWOBYTES));
	   break;
	  }
	  sum = carry + Left.num_p[i];
	}
	carry = sum / BASE;
	sum = sum % BASE;
	result.num_p[i] = sum;
  }
  if (carry > 0 ) {
	TWOBYTES *temp_p = result.num_p;
	result.num_p = new TWOBYTES[result.length+1];
	memcpy(result.num_p, temp_p, result.length*sizeof(TWOBYTES));
	result.num_p[result.length] = carry;
	result.length++;
	delete temp_p;
  }
  if (result == PRIME) {
	  result = 0;
	  return result;
  }
  if (result > PRIME) {
    result = result - PRIME;
  }
  result.optimize();
  return result;
}

// C = (A-B) mod P
// 0 <= A, B <= P
Big_int operator - (Big_int &Left, Big_int &Right) {
  Big_int result;
  if (Right == Left) {
	  result = 0;
	  return result;
  }
  if (Right > Left) {
	  Big_int temp = PRIME - Right;
	  result = Left + temp;
	  return result;
  }
  if (Right == 0) {
	  result = Left;
	  return result;
  }
  
  /* Left is greater than or equal to Right */
  result.getSpace(Left.length);
  unsigned int i;
  TWOBYTES diff = 0;
  TWOBYTES carry = 0;
  for( i = 0; i < result.length; i++) {
	diff = carry;
	if (i < Right.length) {
	  diff += Right.num_p[i];
	}
	if (Left.num_p[i] < diff) {
	  carry = 1;
	  diff = BASE + Left.num_p[i] - diff;
	}
	else {
	  carry = 0;
	  diff = Left.num_p[i] - diff;
	}
	result.num_p[i] = diff;
  }
  if (result == PRIME) {
    result = 0;
  }
  result.optimize();
  return result;
}

Big_int operator * (Big_int &Left, Big_int &Right) {

  Big_int result;
  if (Left == 0 || Right == 0) {
	  return result;
  }
  if (Left == 1) {
	  result = Right;
	  return result;
  }

  if (Right == 1) {
	  result = Left;
	  return result;
  }
  
  /* Assume that Right is shoter than Left; */
  if (Left.length < Right.length) {
	result = Right*Left;
	return result;
  }
  
  result.getSpace(Left.length + Right.length + 1);
  unsigned int i = 0;
  for (i = 0; i < Right.length; i++) {
	if (Right.num_p[i] == 0) {
		continue;
	}
	unsigned int j;
	TWOBYTES carry = 0;
	TWOBYTES num = 0;
	for (j = 0; j < Left.length; j++) {
	  num = Left.num_p[j]*Right.num_p[i];
	  num += result.num_p[i+j] + carry;
	  carry = num/BASE;
	  result.num_p[i+j] = num%BASE;
	}
	result.num_p[i+j] += carry;
  }
  result.sign = Left.sign*Right.sign;
  while (result.num_p[result.length - 1] == 0) {
	  result.length--;
  }
  if (result.length < Left.length + Right.length + 1) {
	  TWOBYTES *temp_p = result.num_p;
	  result.num_p = new TWOBYTES[result.length];
	  memcpy(result.num_p, temp_p, result.length*sizeof(TWOBYTES));
	  delete temp_p;
	  temp_p = NULL;
  }

  result = result % PRIME;
  return result;
}


void divide(Big_int &A, Big_int &B, Big_int &Q, Big_int &R) {
  /* Q = A/B; R = A%B; */
  /* Implementation of Knuth's Long Division Alg. */
  if (B == 0) {
	  cout<<"Division by 0\n";
	  exit(0);
  }
  if (A == 0) {
	 Q = 0;
	 R = 0;
	 return;
  }
  if (B == 1) {
	  Q = A;
	  R = 0;
  }
  if (A == B) {
	  Q = 1;
	  R = 0;
  }
  
  if (B > A) {
	  Q = 0;
	  R = A;
	  return;
  }
  //End of trivia cases
  // A > B 
  
  if (B.length == 1) {
  	  Big_int q;
	  q.getSpace(A.length);
	  TWOBYTES r = 0;
	  long i = (long) A.length - 1;
	  for (; i >= 0; i--) {
		r = r*BASE + A.num_p[i];
		q.num_p[i] = r / B.num_p[0];
		r = r % B.num_p[0];
	  }
	  if (q.num_p[A.length - 1] == 0) {
		 q.length--;
	  }
	  Q = q;
	  R = r;
	  return;
  }
  /* Step D1: Normalization */
  TWOBYTES d = (TWOBYTES) BASE/(B.num_p[B.length-1]+1);
  TWOBYTES *u = new TWOBYTES[A.length+1];
  TWOBYTES *v = new TWOBYTES[B.length];
  TWOBYTES *q = new TWOBYTES[A.length - B.length + 1];
  if (d == 1) {
	  u[A.length] = 0;
	  memcpy(u, A.num_p, A.length*sizeof(TWOBYTES));
	  
  }
  else {
	  TWOBYTES u_carry = 0;
	  for (unsigned int i = 0; i < A.length; i++) {
		  u_carry = (A.num_p[i]*d + u_carry);
		  u[i] = u_carry % BASE;
		  u_carry = u_carry / BASE;
	  }
	  u[A.length] = u_carry;
  }
  TWOBYTES v_carry = 0;
  for (unsigned int i = 0; i < B.length; i++) {
	  v_carry = (B.num_p[i]*d + v_carry);
	  v[i] = v_carry % BASE;
	  v_carry = v_carry / BASE;
  }
  /* Implementation of the Knuth's Long Division Algorithm */
  /* Step D2: Initialization */
  for ( long i = (long) A.length; i >= B.length ; i--) {
	  TWOBYTES q_tmp, r_tmp;
	  int overflow = 0;
	  if (u[i] >= v[B.length-1]) {
		  q_tmp = BASE - 1;
		  if (u[i-1] + v[B.length-1] >= BASE) {
			  overflow = 1;
		  }
		  else {
			  r_tmp = u[i-1] + v[B.length-1];
		  }
	  }
	  else {
		  q_tmp = (TWOBYTES) (u[i]*BASE + u[i-1])/v[B.length-1];
		  r_tmp = (TWOBYTES) (u[i]*BASE + u[i-1])%v[B.length-1];
	  }
	  if (!overflow) {
		  while (v[B.length-2]*q_tmp > (r_tmp*BASE+u[i-2])) {
			  q_tmp--;
			  if (r_tmp + v[B.length-1] >= BASE) {
				  break;
			  }
			  r_tmp += v[B.length-1];
		  }
	  }
	  //D4 Multiply and Subtract

	  TWOBYTES carry = 0;
	  for (int j = 0; j < B.length; j++) {
		  TWOBYTES h, l;
		  h = (v[j]*q_tmp + carry) / BASE;
		  l = (v[j]*q_tmp + carry) % BASE;
		  
		  if (u[i+j-B.length] >= l) {
			  u[i+j-B.length] -= l;
		  }
		  else {
			  h++;
			  u[i+j-B.length] = (BASE - l + u[i+j-B.length]);
		  }
		  carry = h;
	  }

	  int borrow;
	  if (u[i] >= carry) {
		  borrow = 0;
		  u[i] -= carry;
	  }
	  else {
		  borrow = 1;
		  u[i] = BASE - carry + u[i];
	  }
	  if (borrow) {
		  q_tmp--;
		  TWOBYTES carry = 0;
		  for (unsigned int j = 0; j < B.length; j++) {
			  TWOBYTES temp = u[i+j-B.length] + v[i] + carry;
			  u[i+j-B.length] = temp /BASE;
			  carry = temp % BASE;
		  }
	  }
	  q[i-B.length] = q_tmp;	  
  }
  delete v;
  v = NULL;
  TWOBYTES u_carry = 0;
  for (unsigned int i = B.length; i > 0; i--) {
	  TWOBYTES temp = u_carry*BASE + u[i-1];
	  u[i-1] = temp / d;
	  u_carry = temp % d;
  }
  unsigned int u_len = B.length;
  for (unsigned int i = B.length; i > 0; i--) {
	  if (u[i-1]) break;
	  else u_len--;
  }
  if (u_len == 0) {
	  delete u;
	  u = NULL;
	  R = 0;
  }
  else {
	  R.getSpace(u_len);
	  memcpy(R.num_p, u, u_len*sizeof(TWOBYTES));
	  R.length = u_len;
	  delete u;
	  u = NULL;
  }
  unsigned int q_len = A.length - B.length;
  if (q[q_len]) {
	  q_len += 1;
  }
  if (q_len) {
	  Q.getSpace(q_len);
	  memcpy(Q.num_p, q, q_len*sizeof(TWOBYTES));
	  Q.length = q_len;
	  delete q;
	  q = NULL;
  }
  else {
	  delete q;
	  q = NULL;
	  Q = 0;
  }
  return;
}  	  

Big_int operator / (Big_int& Left, Big_int& Right) {
	Big_int Q, R;
	divide(Left, Right, Q, R);
	Q.optimize();
	return Q;
  }

Big_int operator % (Big_int& Left, Big_int& Right) {
	Big_int Q, R;
	divide(Left, Right, Q, R);
	R.optimize();
	return R;
}

ostream& operator << (ostream& os, Big_int& Right) {
  long i = (long) Right.length - 1;
  os << Right.num_p[i];
  i--;
  for (; i >= 0; i--) {
	  TWOBYTES temp = Right.num_p[i];
	  if (temp < 10) {
		  os <<"000"<<temp;
		  continue;
	  }
	  if (temp < 100) {
		  os <<"00"<<temp;
		  continue;
	  }
	  if (temp < 1000) {
		  os <<"0"<<temp;
		  continue;
	  }
	  os<<temp;

  }
  return os;
}

void Big_int::getSpace(unsigned int o_length) {
  length = o_length;
  if (num_p != NULL) {
    delete num_p;
  }
  if (length == 0) {
	  num_p = new TWOBYTES[length];
	  num_p[0] = 0;
	  length = 1;
	  return;
  }
  num_p = new TWOBYTES[length];
  memset(num_p, 0, length*sizeof(TWOBYTES));
}

int Big_int::isOdd() {
	if (num_p[0] % 2) {
		return 1;
	}
	return 0;
}

void Big_int::atobig(char *string_num) { 
	long i;
	unsigned int str_len = strlen(string_num);
	if (str_len == 0) {
		if (num_p != NULL) {
			delete num_p;
		}
		num_p = new TWOBYTES[1];
		length = 1;
		num_p[0] = 0;
		sign = 0;
		return;
	}				

	if (str_len % 4) {
		length = str_len/4 + 1;
	}
	else length = str_len/4;
	if (num_p != NULL) {
		delete num_p;
	}
	num_p = new TWOBYTES[length];
        memset(num_p, 0, length*sizeof(TWOBYTES));
	for (i = 0; i < str_len; i++) {
		int j = i/4;
		num_p[j] = num_p[j] + ((int) pow(10, i%4) * (int) (string_num[str_len - i - 1] - '0'));
	}
	sign = 1;
	return;
}

void Big_int::optimize() {
	long i = (long) length;
	while ( i > 0 && num_p[i-1] == 0) {
		i--;
	}
	if (i == length) return;
	if (i == 0) {
		delete num_p;
		num_p = new TWOBYTES[1];
		num_p[0] = 0;
		length = 1;
		return;
	}
	length = i;
	TWOBYTES *temp_p = num_p;
	num_p = new TWOBYTES[i];
	memcpy(num_p, temp_p, i*sizeof(TWOBYTES));
	delete temp_p;
	temp_p = NULL;
	return;
}

void Big_int::half() { 
  TWOBYTES r = 0;
  long i = (long) length - 1;
  for (; i >= 0; i--) {
	r = r*BASE + num_p[i];
	num_p[i] = r / 2;
	r = r % 2;
  }
  optimize();
  return;
}

void Big_int::sub1() {
  if (num_p[0] >= 1) {
	  num_p[0] -= 1;
	  return;
  }
  unsigned int i = 0;
  unsigned carry = 0;
  while (num_p[i] <= 1) {
	 if (num_p[i] == 1) {
		 if (!carry) {
			 break;
		 }
		 num_p[i] = BASE - 1;
		 i++;
		 continue;
	 }
	 carry = 1;
	 num_p[i] = BASE - carry;
	 i++;
  }
  num_p[i] -= carry;
  optimize();
  return;
}
