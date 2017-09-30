// This is a library for dealing with rational numbers
// Including operations and some other basic functios

struct rational_number
 {
  int num;
  int den;
 };

/****Prototype section****/

BOOLEAN get_fraction(char[],rational_number&);
// Get value for a rational number

BOOLEAN  check_fraction(rational_number);
//Check if data is correct
//Return TRUE if data is correct or FALSE if it is not

void show_fraction(rational_number,BOOLEAN=FALSE);
//Display a rational number

void show_mixed(rational_number);
//Express a rational number as a mixed number
//The numerator must be bigger than the denominator

//Operators
// Addition,subtraction,multiplication,division for two rational numbers
// Result will be return in term of a rational number
rational_number add(rational_number,rational_number);
rational_number subtract(rational_number,rational_number);
rational_number multiply(rational_number,rational_number);
rational_number divise(rational_number,rational_number);
// Find a reciprocal and reduce a rational number
rational_number inverse(rational_number);
rational_number reduce(rational_number);
//rational_number operate(char, rational_number,rational_number);

int get_GCD(int,int);
// Get the greatest common divisor for a pair of integers

int get_LCM(int,int);
// Get the smallest common denominator



/****Definition section****/

BOOLEAN get_fraction(char data[],rational_number&fraction)
 {
  char temp1[one_line],temp2[one_line];
  int index;
  fraction.den=0;
  if (!isdigit(data[0])&&data[0]!='-'&&data[0]!='+')
   {return FALSE;}
  else
   {
    temp1[0]=data[0];
    for (index=1;isdigit(data[index]);index++)
     {
      temp1[index]=data[index];
     }
    if ((data[index]=='/'&&isdigit(data[index+1]))||data[index]=='\0')
     {
      fraction.num=atoi(temp1);
      if (data[index]=='\0')
       {
	fraction.den=1;
	return TRUE;
       }
      else
       {
	int i=0;
	index++;
	for (;isdigit(data[index]);index++)
	 {
	  temp2[i]=data[index];
	  i++;
	 }
	fraction.den=atoi(temp2);
	if (fraction.den==0) return FALSE;
	else return TRUE;
       }
     }
    else return FALSE;
   }
 }

BOOLEAN check_fraction(rational_number fraction)
 {
  if (fraction.den==0) return FALSE;
  else return TRUE;
 }

void show_fraction(rational_number fraction, BOOLEAN mix)
 {
  if (fraction.num==fraction.den)
   {
    cout<<'1';
    return;
   }
  if (fraction.den==1)
   {
    cout<<fraction.num;
    return;
   }
  if (fraction.num==0)
   {
    cout<<'0';
    return;
   }
  if (abs(fraction.num)>abs(fraction.den)&&mix==TRUE)
   {
    show_mixed(fraction);
    return;
   }
  else
   {
    cout<<fraction.num<<'/'<<fraction.den;
    return;
   }
 }

void show_mixed(rational_number fraction)
 {
  int number;
  number=(fraction.num)/(fraction.den);
  cout<<number<<" "<<(abs(fraction.num))%(fraction.den)<<'/'<<fraction.den;
  return;
 }

rational_number reduce(rational_number original)
 {
  rational_number reduced_number;
  reduced_number.num=original.num/get_GCD(original.num,original.den);
  reduced_number.den=original.den/get_GCD(original.num,original.den);
  return reduced_number;
 }

int get_GCD(int a, int b)
 {
  int reminder;
  a=abs(a);
  b=abs(b);
  if (a<b)
   {
    reminder=b; //treat reminder as temp
    b=a;
    a=reminder;
   } //Make sure that a>=b
  while (b!=0)
   {
    reminder=a%b;
    a=b;
    b=reminder;
   }
  return a;
 }

int get_LCM(int a,int b)
 {
  return (abs(a*b)/get_GCD(a,b));
 }


//Operators Definition
// "+"
rational_number add(rational_number fraction1,rational_number fraction2)
 {
  rational_number result;
  result.num=fraction1.num*fraction2.den+fraction2.num*fraction1.den;
  result.den=fraction1.den*fraction2.den;
  return reduce(result);
 }
// "-"
rational_number subtract(rational_number fraction1,rational_number fraction2)
 {
  rational_number result;
  result.num=fraction1.num*fraction2.den-fraction2.num*fraction1.den;
  result.den=fraction1.den*fraction2.den;
  return reduce(result);
 }
// "*"
rational_number multiply(rational_number fraction1,rational_number fraction2)
 {
  rational_number result;
  result.num=fraction1.num*fraction2.num;
  result.den=fraction1.den*fraction2.den;
  return reduce(result);
 }
// "/"
rational_number divise(rational_number fraction1,rational_number fraction2)
  {
   return multiply(fraction1,inverse(fraction2));
  }


rational_number inverse(rational_number fraction)
 {
  rational_number result;
  result.num=fraction.den;
  result.den=fraction.num;
  return result;
 }