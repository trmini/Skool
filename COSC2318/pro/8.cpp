#include "c:\pro\include.h"
#include "c:\pro\rational.h"
#include "c:\pro\display.h"
const int LIMIT=99;

BOOLEAN prompt()
 {
  char key;
  window(1,1,80,25);
  gotoxy(1,25);
  textcolor(WHITE);
  textbackground(RED);
  clreol();
  cout<<" Use ESC to exit or hit any key to continue";
  key=getch();
  if (key!=ESC)
   {
    if (key==0) {getch();}
    return TRUE;
   }
  else
   {
    return FALSE;
   }
 }

BOOLEAN check()
 {
  char answer;
  char buffer[4096];
  gettext(1,1,80,25,buffer);
  sub_window(27,15,53,17);
  gotoxy(2,2);
  cout<<"Is that your answer? Y/N";
  answer=getch();
  puttext(1,1,80,25,buffer);
  if (toupper(answer)=='Y') return TRUE;
  else return FALSE;
 }

int main()
 {
  rational_number number1,number2,result_number,temp;
  char operator_sign;
  int total_count=0,correct_count=0;
  BOOLEAN again=TRUE;
  while (again==TRUE)
   {
  //get random values
  randomize();
  drawbackground();
  number1.num=random(LIMIT)+1;
  number1.den=random(LIMIT)+2;
  number2.num=random(LIMIT)+1;
  number2.den=random(LIMIT)+2;
  if (random(2)==0) //Random for a negative or positive number
   {
    number1.num=-number1.num;
   }
  switch (random(4)) //Random for operator
   {
    case 0:
     result_number=add(number1,number2);
     operator_sign='+';
     break;
    case 1:
     result_number=subtract(number1,number2);
     operator_sign='-';
     break;
    case 2:
     result_number=multiply(number1,number2);
     operator_sign='x';
     break;
    case 3:
     result_number=divise(number1,number2);
     operator_sign='ö';
     break;
   }
  sub_window(4,4,26,8);
  gotoxy(2,1);
  cout<<"SCORED BOARD";
  gotoxy(2,3);
  cout<<"Correct answer(s): "<<correct_count;
  gotoxy(2,4);
  cout<<"Total question(s): "<<total_count;
  total_count++;
  //Prompt for input data
  window(1,1,80,25);
  gotoxy(1,25);
  textcolor(WHITE);
  textbackground(RED);
  clreol();
  cout<<" Enter your result in term of numerator/denominator (Ex. 2/3)";

  sub_window(20,10,60,16);
  gotoxy(3,3);
  show_fraction(number1);
  cout<<" "<<operator_sign<<" ";
  show_fraction(number2);
  cout<<" "<<'='<<" ";
  get_fraction(temp);
  while (!check())
   {
    sub_window(20,10,60,16);
    gotoxy(3,3);
    show_fraction(number1);
    cout<<" "<<operator_sign<<" ";
    show_fraction(number2);
    cout<<" "<<'='<<" ";
    get_fraction(temp);
   }
  if (temp.num==result_number.num&&temp.den==result_number.den)
   {
    correct_count++;
    sub_window(30,17,50,19);
    gotoxy(4,2);
    cout<<"You've got it!";
    again=prompt();
   }
  else
   {
    sub_window(25,17,55,20);
    gotoxy(10,1);
    cout<<"** Sorry! **";
    gotoxy(4,3);
    cout<<"The result is ";
    show_fraction(result_number);
    again=prompt();
   }
   }
  return 0;
 }