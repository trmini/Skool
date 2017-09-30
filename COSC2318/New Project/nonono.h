#include "a:\include.h"

struct rational
 {
  int num;
  char slash;
  int den;
 };

void get_fraction(rational&);
int get_GCD(int,int);
int get_LCM(int,int);

void get_fraction(rational&number)
 {
  cin>>number.num>>number.slash>>number.den;
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