// Trang Luu
// COSC 2318.001
// Program 2: Where did I leave my toga?
// Due date: 09/16/98
// Roman adding/subtracting machine.

#include <iostream.h>
#include <constrea.h>
#include <ctype.h>

int convert(char);
// This function converts a Roman numerical character into its integer value.
// When you give a Roman numerical character, the function will return integer value.
// Otherwise, it will return zero.
// Example:
// convert(L); equals to convert(l); and the returned value is 50
// convert(Q); will return 0

void convert(int);
// This function converts an integer to a Roman number
// It will receive an integer parameter and display a correlative Roman number
// All you need is give it a positive integer; it will display the result right
// at the current cursor's position.
// Example:
// convert(15); XV will be displayed


int main()
 {
  int number1, number2, result;
  char answer='Y', symbol;
  clrscr();
  cout<<"Welcome!"<<endl;
  cout<<"This program will help you add or subtract two Roman numbers. You can"<<endl;
  cout<<"also see your values in both forms, Roman and Integer."<<endl<<endl;
  cout<<"Hit enter to begin!";
  cin.get();
  while (answer=='y' || answer=='Y')
   {
    symbol=' ';     // Clear variables before using
    number1=0;
    number2=0;

    clrscr();
    gotoxy(1,25);
    cout<<"Any character that is not a Roman numerical character will be assignted to zero!";
    gotoxy(1,3);
    cout<<"Enter the first number:	";
    while (symbol!='\n')
     {
      cin.get(symbol);
      symbol=toupper(symbol);
      number1=number1+convert(symbol);
     }
    cout<<"The first number is: "<<number1<<" ( ";
    convert(number1);
    cout<<" ) "<<endl<<endl;
    cout<<"Enter the second number: ";
    symbol=' ';     //Clear symbol's value
    while (symbol!='\n')
     {
      cin.get(symbol);
      symbol=toupper(symbol);
      number2=number2+convert(symbol);
     }
    cout<<"The second number is: "<<number2<<" ( ";
    convert(number2);
    cout<<" ) "<<endl<<endl;
    cout<<"Enter the desired operation (+ or -): ";
    cin>>symbol;
    cout<<endl;
    if (symbol=='+')
     {
      result=number1+number2;
      cout<<"The sum of two numbers is: ";
      convert(result);
      cout<<"    or    "<<result<<endl;
     }
    else
     if (symbol=='-')
      {
       result=number1-number2;
       cout<<"The difference between two numbers is: ";
       if (result==0) cout<<"zero"<<endl;
       else
	{
	 convert(result);
	 cout<<"    or    "<<result<<endl;
	}
      }
     else cout<<"Invalid operation!"<<endl;

    cout<<endl;
    cout<<"Do you want to continue? Y/N: ";
    cin>>answer;
    cin.get();
   }
  clrscr();
  return 0;
 }

int convert(char Roman_number)
 {
  if (Roman_number=='I') return 1;
  else if (Roman_number=='V') return 5;
  else if (Roman_number=='X') return 10;
  else if (Roman_number=='L') return 50;
  else if (Roman_number=='C') return 100;
  else if (Roman_number=='D') return 500;
  else if (Roman_number=='M') return 1000;
  else return 0;
 }

void convert(int Int_number)
 {

  while (Int_number>=1000)
   {
    cout<<"M";                           //1000
    Int_number=Int_number-1000;
   }

  while (Int_number>=500)
   {
    cout<<"D";                           //500
    Int_number=Int_number-500;
   }

  while (Int_number>=100)
   {
    cout<<"C";                           //100
    Int_number=Int_number-100;
   }

  while (Int_number>=50)
   {
    cout<<"L";                           //50
    Int_number=Int_number-50;
   }

  while (Int_number>=10)
   {
    cout<<"X";                           //10
    Int_number=Int_number-10;
   }

  while (Int_number>=5)
   {
    cout<<"V";                           //5
    Int_number=Int_number-5;
   }

  while (Int_number>=1)
   {
    cout<<"I";                           //1
    Int_number=Int_number-1;
   }
  return;
 }