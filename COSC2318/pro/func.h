struct fraction
 {
  int num;
  int den;
 };

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

void get_fract(fraction&number)
 {
  char slash;
  cin>>number.num>>slash>>number.den;
  cin.get();
  return;
 }

void show_fract(fraction&number)
 {
  cout<<number.num<<'/'<<number.den;
  return;
 }



