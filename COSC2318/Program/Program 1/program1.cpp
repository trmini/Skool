// Trang Luu
// COSC 2318.001
// Program 1: Which way did they go?
// Due date: 09/09/98
// To transform compass headings to compass bearings

#include <iostream.h>
#include <constrea.h>

int main()
 {
  float angle;
  char answer='Y';

  clrscr();
  cout<<" Welcome!"<<endl;
  cout<<" This program will help you transfrom compass headings to compass bearings."<<endl;
  cout<<" Please hit Enter to begin!";
  cin.get();

  while (answer=='Y' || answer=='y')
   {
    clrscr();
    cout<<"Please input your compass heading value, between 0"<<char(248)<<" and 360"<<char(248)<<": ";
    cin>>angle;
    cout<<endl;
    if (angle>=0 && angle<=360)
     {

      // Face North
      if (angle<90 || angle>270)
       {
        if (angle==0 || angle==360)  cout<<"Keep going North!"<<endl;
        else
         {
          cout<<"Face North and then turn to";
          if (angle<90) cout<<"East "<<angle<<char(248)<<".\n";
          else cout<<"West "<<(360-angle)<<char(248)<<".\n";
         }
       }

      // Face South
      if (angle>90 && angle<270)
       {
        if (angle==180)cout<<"Keep going South!"<<endl;
        else
         {
          cout<<"Face South and then turn to ";
          if (angle<180) cout<<"East "<<(180-angle)<<char(248)<<".\n";
          else cout<<"West "<<(angle-180)<<char(248)<<".\n";
         }
       }

      // Special angles
      if (angle==90) cout<<"Face North or South and then turn to East 90"<<char(248)<<endl;
      if (angle==270)cout<<"Face North or South and then turn to West 90"<<char(248)<<endl;
     }
    else       // angle<0° or >360°
     {
      cout<<"Invalid data!"<<endl;
     }
    cout<<endl;
    cout<<"Do you want to continue? (Y/N) ";
    cin>>answer;
   }
  clrscr();
  return 0;
 }
