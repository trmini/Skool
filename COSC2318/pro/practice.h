#include "c:\pro\rational.h"

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

void warning()
 {
  char buffer[4096];
  gettext(1,1,80,25,buffer);
  sub_window(32,17,48,19,RED);
  gotoxy(3,1);
  cout<<"You did input";
  gotoxy(3,3);
  cout<<"improper data!";
  window(1,1,80,25);
  gotoxy(1,25);
  textcolor(WHITE);
  textbackground(RED);
  clreol();
  cout<<"ESC Return";
  while (getch()!=ESC);
  puttext(1,1,80,25,buffer);
  return;
 }

void self_test(int lesson,int mix_flag)
 {
  rational_number number1,number2,result_number,temp_result;
  char operator_sign;
  char temp[one_line];
  int option,total_count=0,correct_count=0;
  BOOLEAN again=TRUE;
  while (again==TRUE)
   {
    drawbackground();

  //get random values
  randomize();
  number1.num=random(LIMIT)+1;
  number1.den=random(LIMIT)+2;
  number2.num=random(LIMIT)+1;
  number2.den=random(LIMIT)+2;
  if (random(2)==0) //Random for a negative or positive number
   {
    number1.num=-number1.num;
   }
  if (lesson==-1)
   {option=random(4);}
  else
   option=lesson-1;
  switch (option) //Random for operator
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
     result_number=divise(number1,number2);
     operator_sign='ö';
     break;
    case 3:
     result_number=multiply(number1,number2);
     operator_sign='x';
     break;
   }
  //For Scored Board
  sub_window(4,4,26,8,GREEN);
  gotoxy(2,1);
  cout<<"SCORED BOARD";
  gotoxy(2,3);
  cout<<"Correct answer(s): "<<correct_count;
  gotoxy(2,4);
  cout<<"Total  problem(s): "<<total_count;
  total_count++;

  //Mix or normal Condition
  sub_window(58,4,71,4,GREEN);
  gotoxy(1,1);
  if (mix_flag==TRUE)
   {
    cout<<"Show as mixed";
   }
  else cout<<"Show as normal";

  //Prompt for input data
  window(1,1,80,25);
  gotoxy(1,25);
  textcolor(WHITE);
  textbackground(RED);
  clreol();
  cout<<" Enter your result in term of numerator/denominator (Ex. 2/3)";

  sub_window(20,10,60,16);
  gotoxy(3,3);
  show_fraction(number1,mix_flag);
  cout<<" "<<operator_sign<<" ";
  show_fraction(number2,mix_flag);
  cout<<" "<<'='<<" ";
  cin.getline(temp,one_line);
  while(!get_fraction(temp,temp_result))
   {
    warning();
    sub_window(20,10,60,16);
    gotoxy(3,3);
    show_fraction(number1,mix_flag);
    cout<<" "<<operator_sign<<" ";
    show_fraction(number2,mix_flag);
    cout<<" "<<'='<<" ";
    cin.getline(temp,one_line);
   }
  while (!check())
   {
    sub_window(20,10,60,16);
    gotoxy(3,3);
    show_fraction(number1,mix_flag);
    cout<<" "<<operator_sign<<" ";
    show_fraction(number2,mix_flag);
    cout<<" "<<'='<<" ";
    cin.getline(temp,one_line);
    while(!get_fraction(temp,temp_result))
     {
      warning();
      sub_window(20,10,60,16);
      gotoxy(3,3);
      show_fraction(number1,mix_flag);
      cout<<" "<<operator_sign<<" ";
      show_fraction(number2,mix_flag);
      cout<<" "<<'='<<" ";
      cin.getline(temp,one_line);
     }
   }
   temp_result=reduce(temp_result);
  if (temp_result.num==result_number.num&&temp_result.den==result_number.den)
   {
    correct_count++;
    sub_window(30,17,50,19,GREEN);
    gotoxy(4,2);
    cout<<"You've got it!";
    again=prompt();
   }
  else
   {
    sub_window(25,17,55,20,BROWN);
    gotoxy(10,1);
    cout<<"** Sorry! **";
    gotoxy(4,3);
    cout<<"The result is ";
    show_fraction(result_number,mix_flag);
    again=prompt();
   }
  }
  return;
 }