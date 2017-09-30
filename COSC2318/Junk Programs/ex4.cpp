#include <iostream.h>
#include <ctype.h>
#include <constrea.h>

int main()
  {
   float total, needed, inter_rate, monthly, duration;
   char again='Y';
   cout.setf(ios::fixed);
   cout.setf(ios::showpoint);
   cout.precision(2);
   while (again=='Y'|| again=='y')
    {
      clrscr();
      cout<<'\t'<<"Calculate the face value according to your needed amount\n";
      cout<<endl;
      cout<<"Please enter the amount needed: $";
      cin>>needed;
      cout<<"What is the interest rate? %";
      cin>>inter_rate;
      cout<<"How many months you want to make this loan? ";
      cin>>duration;
      cout<<endl;
      // End of input data
      inter_rate=inter_rate/100; // change from percent to number
      total=(needed/(1-(inter_rate*duration/12)));
      monthly=(total/duration);
      cout<<"The face amount of this loan is: $"<<total<<endl;
      cout<<"Your monthly payment will be: $"<<monthly<<endl;
      cout<<"Want again? (Y/N) ";
      cin>>again;
    }
   return(0);
  }
