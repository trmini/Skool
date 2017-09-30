// Trang Luu
// COSC 2318.001
// Program 3: When will the IRS visit?
// Due date: 10/05/98
// Display a table of values indicating the asset's current value and current
// depreciation amount.

#include <iostream.h>
#include <constrea.h>
#include <ctype.h>

void get_data(char[]&,double&,int&,int&,double&);
void welcome();


int main()
 {
  double original_cost, salvage_value;
  int year_of_purchase, estimated_life;
  char answer='Y', item_name[80]='Unknown';
  welcome();
  while (toupper(answer)='Y')
   {

   }
 }

void get_data(char&name[],double&cost,int&year,int&life,double&salvage)
 {
  char check=' ';
  while (check!='Y')
   {
    cout<<"Please discribe your item:  ";
    cin>>name;
    cout<<"What is the original cost?  $";
    cin>>cost;
    cout<<"What is the year of purchase (4 numbers) ? ";
    cin>>year;
    cout<<"Estimated life ?";
    cin>>life;
    cout<<"What is the salvage value? $";
    cin>>salvage;
    cout<<" Is everything correct? Y/N";
    cin>>check;
   }
  cout
 }

void welcome()
 {
  clrscr();
  goto(2,31);
  cout<<"Welcome!"<<endl<<endl;
  cout<<" This program will help you prepare well information on your";
  cout<<" company's asset. Don't worry about the IRS's visiting."<<endl;
  cout<<" Hit enter to start!";
  cout.get();
  clrscr();
 }
