#include "a:\include.h"

struct rational
 {
  int num;
  char slash;
  int den;
 };

void get_fraction(rational&);
void show_fraction(rational);
void show_mixed(rational);

int get_GCD(int,int);
int get_LCM(int,int);

rational reduce(rational);
rational inverse(rational);
rational add(rational,rational);
rational subtract(rational,rational);
rational multiply(rational,rational);
rational divise(rational,rational);

//++Definition++

void get_fraction(rational&number)
 {
  cin>>number.num>>number.slash>>number.den;
  cin.get(); //Clear the eol character
  return;
 }

void show_fraction(rational number)
 {
  if (number.den==1)
   {
    cout<<number.num;
  else cout<<number.slash<<number.den<<" ";
  if (number.num==0) cout<<number.num;
  return;
 }

void show_mixed(rational number)
 {
  int int_number;
  int_number=number.num/number.den;
  number.num=number.num%number.den;
  if (int_number==0)
   {
    show_fraction(number);
    return;
   }
  else  cout<<int_number<<" ";
  if (number.num!=0) show_fraction(number);
  return;
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

//++Fun with rational number++


rational reduce(rational original)
 {
  rational reduced;
  reduced.slash='/';
  reduced.num=original.num/get_GCD(original.num,original.den);
  reduced.den=original.den/get_GCD(original.num,original.den);
  return reduced;
 }

rational inverse(rational fraction)
 {
  rational result;
  result.slash='/';
  result.num=fraction.den;
  result.den=fraction.num;
  return result;
 }

//Operators Definition
// "+"
rational add(rational fraction1,rational fraction2)
 {
  rational result;
  result.slash='/';
  result.num=fraction1.num*fraction2.den+fraction2.num*fraction1.den;
  result.den=fraction1.den*fraction2.den;
  return reduce(result);
 }
// "-"
rational subtract(rational fraction1,rational fraction2)
 {
  rational result;
  result.slash='/';
  result.num=fraction1.num*fraction2.den-fraction2.num*fraction1.den;
  result.den=fraction1.den*fraction2.den;
  return reduce(result);
 }
// "*"
rational multiply(rational fraction1,rational fraction2)
 {
  rational result;
  result.slash='/';
  result.num=fraction1.num*fraction2.num;
  result.den=fraction1.den*fraction2.den;
  return reduce(result);
 }
// "/"
rational divise(rational fraction1,rational fraction2)
  {
   return multiply(fraction1,inverse(fraction2));
  }