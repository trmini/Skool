//Small Project

//Global

#include <iostream.h>
#include <constrea.h>
struct corner
 {
  int x;
  int y;
 };

struct rational_number
 {
  int numerator;
  char slash;
  int denominator;
 };

void getfraction(rational_number&);
void showfraction(rational_number);
void background(int);
void make_window(constream&,int,int,int,int,int=BLUE,int=LIGHTGRAY);
void fill_window(constream,char);

void getfraction(rational_number&fraction)
 {
  cin>>fraction.numerator>>fraction.slash>>fraction.denominator;
  cin.get();
  while (fraction.slash!='/'||fraction.denominator==0)
   {
    //int line;
    //line=wherey(); more to control the monitor
    cout<<"Invalid Data! Please re-enter another number: ";
    cin>>fraction.numerator>>fraction.slash>>fraction.denominator;
   }
  return;
 }

void showfraction(rational_number fraction)
 {
  cout<<fraction.numerator<<fraction.slash<<fraction.denominator;
  return;
 }

void background(constream&full_window,int color)
 {
   constream full_window;
   int row,column;

   full_window.window(1,1,80,25);
   full_window<<setattr((BLUE<<4)|color);
   full_window.clrscr();
   full_window<<setcrsrtype(0);
   for (row=1;row<=25;row++)
    for (column=1;column<=80;column++)
     {
      full_window<<setxy(column,row)<<"±";
     }
   return;
 }

void make_window(constream& windows,int upleft_x,int upleft_y,int downright_x,int downright_y, int background_color, int foreground_color)
 {
  windows.window(upleft_x,upleft_y,downright_x,downright_y);
  windows.clrscr();
  windows<<setcrsrtype(0)<<setattr((background_color<<4)|foreground);
  return;
 }

void outline_window(constream window)
 {
  int column,row;
  for (row=1;row<=
