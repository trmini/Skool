// Exercise 3 page 138

#include <iostream.h>
#include <constrea.h>
#include <ctype.h>

int main()
 {
  int hours, minutes, data_OK=1;
  char answer='Y', suffix, dummy;

  clrscr();
  while (toupper(answer)=='Y')
   {
    cout<<"Enter time (Hours:MinutesSuffix) : ";
    cin>>hours;
    cin>>dummy;
    cin>>minutes;
    cin>>suffix;
    while (cin.get()!='\n') { cin.get(); } //Clean buffer
    suffix=toupper(suffix);

    if (suffix=='A' && hours<12) hours=hours;
    else if (suffix=='P' && hours<12) hours=(hours+12);
    else if (suffix=='N' && hours==12) hours=12;
    else if (suffix=='M' && hours==12) hours=0;
    else data_OK=0;

    if (data_OK==1)
     {
      if (minutes>=10 && hours>=10) cout<<hours<<":"<<minutes<<" hours"<<endl;
      else
       if (minutes>=10) cout<<"0"<<hours<<":"<<minutes<<" hours"<<endl;
       else if (hours>=10)   cout<<hours<<":0"<<minutes<<" hours"<<endl;
	    else cout<<"0"<<hours<<":0"<<minutes<< " hours"<<endl;
     }
    else
     {
      cout<<" Invalid data format!"<<endl;
      data_OK=1;
     }
    cout<<endl<<"Again? Y/N ";
    cin>>answer;
    cout<<endl;
   }
  clrscr();
  return 0;
 }

