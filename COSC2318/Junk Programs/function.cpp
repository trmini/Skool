#include <iostream.h>
#include <math.h>

int main()
 {
   double f,x;
   int count=0;
   x=-0.5;
   while (x<1)
    {
     f=exp(x)-1-x-x*x/2;
     count++;
     x=x+0.01;
     cout<<x<<"__"<<f<<endl;
     if ((count%15)==0)
      {
       cout<<"Hit enter to continue!";
       cin.get();
      }
     }
   cout<<"Did you see something?"<<endl;
   return 0;
 }


