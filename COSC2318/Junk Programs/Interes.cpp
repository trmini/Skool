#include <iostream.h>
#include <constrea.h>
#include <ctype.h>

int main()
  {
    float total=1000;
    const float inter_rate=0.015;
    const int payment=50;
    float paid;
    int count=0;
    clrscr();
    cout.setf(ios::fixed);
    cout.setf(ios::showpoint);
    cout.precision(2);
    cout<<"This is a list of your monthly amount of interest paid and remaining debt\n";
    cout<<"Month"<<'\t'<<"Interest paid"<<'\t'<<"Remaining debt\n";
    while (total>50)
      {
        count++;
        paid=inter_rate*total;
        total=(total-payment+paid);
        cout<<"  "<<count<<"\t  "<<"$"<<paid<<"        "<<"$"<<total<<"\n";
        if (count==17)
         { cout<<"Hit enter to see the rest....\n";
           cin.get();
         }
      }
    paid=inter_rate*total;
    cout<<"\t  "<<"$"<<paid<<'\t'<<"You are finished\n";
    cout<<"The amount you will have to pay for the last month is: $"<<(total+paid)<<"\n";
    cout<<"You will pay in "<<(count+1)<<" months\n";
    cin.get();
    return 0;
  }




