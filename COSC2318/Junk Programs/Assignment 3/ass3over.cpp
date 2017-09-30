//Overloading version

#include <iostream.h>
#include <constrea.h>

void swap(char& first,char& second)
 { char temp;
   temp=first;
   first=second;
   second=temp;
 }

void swap(int& first,int& second)
 { int temp;
   temp=first;
   first=second;
   second=temp;
 }

void swap(float& first,float& second)
 { float temp;
   temp=first;
   first=second;
   second=temp;
 }


int main()
 {
   char c1='a', c2='b';
   int n1=5, n2=10;
   float f1=56.78, f2=98.23;
   cout.setf(ios::fixed);
   cout.setf(ios::showpoint);
   cout.precision(2);
   clrscr();

   cout<<" Original Order: "<<endl;
   cout<<" Characters   : "<<c1<<' '<<c2<<endl;
   cout<<" Integers     : "<<n1<<' '<<n2<<endl;
   cout<<" Real Numbers : "<<f1<<' '<<f2<<endl;

   swap(c1,c2);
   swap(n1,n2);
   swap(f1,f2);

   cout<<endl<<endl;

   cout<<" Swapped Order: "<<endl;
   cout<<" Characters   : "<<c1<<' '<<c2<<endl;
   cout<<" Integers     : "<<n1<<' '<<n2<<endl;
   cout<<" Real Numbers : "<<f1<<' '<<f2<<endl;

   cin.get();
   return 0;
 }
