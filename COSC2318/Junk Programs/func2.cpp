#include <iostream.h>
#include <constrea.h>
#include <math.h>

int main()
 {
  float a,b,fa,fb,fm,mean;
  char more='Y';
  a=1;
  b=2;
  fa=a*a*a-a-2;
  fb=b*b*b-b-2;
  while (more=='Y'||more=='y')
   {
    if (fa*fb<0)
     {
      mean=(a+b)/2;
      fm=mean*mean*mean-mean-2;
      if (fa*fm<0)
       {
        cout<<a<<"<= x="<<mean<<" <="<<b<<endl;
        cout<<"Accuracy is "<<fabs(mean-a)<<endl;
        b=mean;
       }
       else
        {
          cout<<a<<"<= x="<<mean<<" <="<<b<<endl;
          cout<<"Accuracy is "<<fabs(mean-b)<<endl;
          a=mean;
        }
      }
     cout<<"Do you want more accuracy? (Y/N)";
     cin>>more;
    }
   cin.get();
   return 0;
  }