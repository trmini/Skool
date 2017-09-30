// This is a library for dealing with rational numbers
// Including operations and some basic functios
#include "a:\include.h"

struct rational_number
 {
  int num;
  char slash;
  int den;
 };

/****Prototype section****/

void get_fraction(rational_number&);
// Get value for a rational number

int  check_fraction(rational_number);
//Check if data is correct
//Return 1 if data is correct or 0 if it is not

void show_fraction(rational_number);
//Display a rational number

void show_mixed(rational_number);
//Express a rational number as a mixed number
//The numerator must be bigger than the denominator

//Operators
// Addition,subtraction,multiplication,division two rational numbers
// Result will be return in term of a rational number
rational_number add(rational_number,rational_number);
rational_number subtract(rational_number,rational_number);
rational_number multiply(rational_number,rational_number);
rational_number divise_fraction(rational_number,rational_number);
// Find a reciprocal and reduce a rational number
rational_number inverse(rational_number);
rational_number reduce(rational_number);

int get_GCD(int,int);
// Get the greatest common divisor for a pair of integers

int get_LCM(int,int);
// Get the smallest common denominator



/****Definition section****/

void get_fraction(rational_number&fraction)
 {
  cin>>fraction.num>>fraction.slash>>fraction.den;
  cin.get();
  return;
 }

int check_fraction(rational_number fraction)
 {
  if (fraction.slash!='/'||fraction.den==0) return 0;
  return 1;
 }

void show_fraction(rational_number fraction)
 {
  cout<<fraction.num<<" "<<fraction.slash<<" "<<fraction.den;
  return;
 }

void show_mixed(fractional_number fraction)
 {
  int number;
  number=fraction.num/fraction.den;
  fraction.num=fraction.num%fraction.den;
  cout<<number<<" ";
  show_fraction(fraction);
  return;
 }

rational_number reduce(rational_number original)
 {
  rational_number reduced;
  reduced.num=original.num/get_GDC(original.num,original.den);
  reduced.den=original.den/get_GDC(original.num,original.den);
  return reduced;
 }

int get_GCD(int a, int b)
 {
  int reminder;
  a=abs(a);
  b=abs(b);
  if (a<b)
   {
    reminder=b;
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
rational_number subtract(rational_number,rational_number)
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
rational_number divise_fraction(rational_number fraction1,rational_number fraction2)
  return multiply(fraction1,inverse(fraction2));


rational_number inverse(rational_number fraction)
 {
  rational_number result;
  result.num=fraction.den;
  result.den=fraction.num;
  return result;
 }
