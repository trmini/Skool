// Trang Luu
// COSC 2318.001
// Library for Program 3

#include <iostream.h>
#include <iomanip.h>
#include <constrea.h>
#include <ctype.h>
#include <conio.h>

double de_amount(double,int);
// Calculate the depreceation amount from the cost and estimated year this
// item will last.
// de_amount(begining_cost,estimated_life); will return depreciation amount.
// The begining_cost is real, and estimated_life is an positive integer.

void decimal(int);
// Real number will be displayed with any digits after the decimal point
// For example, decimal(2); will display 3.141 in term 3.14

void get_data(char[], double&, int&, int&, double&);
// Collect data for this program
// Values received are in the oder that follow:
// name, original cost, year of perchase, estimated life, and salvage value.
// The type of each parameters is listed in protocol

void print_data(char[],double,int,int,double);
// This function simply print the header for the table and include data
// collected from user.

void print_data(int,double,double);
// Print year, cost at the beginning and depreciation amount for that year
// print_data(1998,10000.0,4000.0);
// This function also does some more works to give specific outputs for
// the program

void welcome();
//Display instructions for this program


/* ~z~z~z~ Defenition part: ~z~z~z~*/

double de_amount(double begining_cost, int life)
 {
  return  ((2*begining_cost)/life);
 }

void decimal(int Precision)
 {
  cout.setf(ios::fixed);
  cout.setf(ios::showpoint);
  cout.precision(Precision);
  return;
 }

//-----
void get_data(char name[],double&cost,int&year,int&life,double&salvage)
 {
  char check=' ';
  int count=0;
  while (toupper(check)!='Y')
   {
    cout<<"Please discribe your item (less than 20 letters):  ";
    cin.get(name[count]);
    while (name[count]!='\n')
     {
      count++;
      cin.get(name[count]);
     }
    name[count]='\0';
    cout<<endl<<"What is the original cost?  $";
    cin>>cost;
    cout<<endl<<"What is the year of purchase (4 numbers) ? ";
    cin>>year;
    cout<<endl<<"Estimated life in years? ";
    cin>>life;
    cout<<endl<<"What is the salvage value? $";
    cin>>salvage;
    cout<<endl<<"Is everything correct? Y/N  ";
    cin>>check;
    cin.get();
    clrscr();
   }
 }

//-----
void print_data(char name[],double cost,int year,int life,double salvage)
 {
  cout<<setw(35)<<"Description:"<<setw(20)<<name<<endl;
  cout<<setw(35)<<"Original Cost:"<<setw(7)<<"$"<<setw(13)<<cost<<endl;
  cout<<setw(35)<<"Year of Purchase:"<<setw(20)<<year<<endl;
  cout<<setw(35)<<"Estimated Life:"<<setw(14)<<life<<setw(6)<<"years"<<endl;
  cout<<setw(35)<<"Salvage Value:"<<setw(7)<<"$"<<setw(13)<<salvage<<endl;
  cout<<endl<<endl;
  cout<<setw(16)<<"Year"<<setw(28)<<"Value at Beginning"<<setw(20)<<"Amount of"<<endl;
  cout<<setw(41)<<"of The Year"<<setw(24)<<"Depreciation"<<endl;
  cout<<setw(16)<<"===="<<setw(28)<<"=================="<<setw(21)<<"============"<<endl;
  cout<<endl;
  return;
 }

//-----
void print_data(int year,double cost,double depreciation)
 {
  cout<<setw(16)<<year<<setw(11)<<"$"<<setw(15)<<cost<<setw(12)<<"$"<<setw(12)<<depreciation;
  cout<<endl;
  return;
 }

//-----
void welcome()
 {
  clrscr();
  gotoxy(30,2);
  cout<<"Welcome!"<<endl<<endl;
  cout<<"     This program will help you prepare well information on your company's"<<endl;
  cout<<"assets. For best result table, use this program for item which cost is less"<<endl;
  cout<<"than one hundred millions dollars.";
  gotoxy(50,18);
  cout<<"Hit enter to start!";
  cin.get();
  clrscr();
 }