#include <iostream.h>
#include <constrea.h>
#include <string.h>
#include <fstream.h>
#include <stdlib.h>

const char file_name[]="PEOPLE.DAT";

const int maxsize=81;

int main()
 {
  fstream people;
  int age, count=0, total=0;
  char name[maxsize];
  clrscr();
  cout.setf(ios::fixed);
  cout.setf(ios::showpoint);
  cout.precision(2);
  people.open(file_name,ios::out);
  if (people.fail())
   {
    cout<<"Error when creating output file!"<<endl;
    exit(1);
   }
  cout<<"Begin inputing data. Enter 0 to finish."<<endl;
  cout<<"Enter name: ";
  cin.getline(name,81);
  do
   {
    cout<<"Enter age: ";
    cin>>age;
    cin.get();
    count++;
    total=total+age;
    people<<name<<"  "<<age<<endl;
    cout<<"Enter name: ";
    cin.getline(name,81);
   }
  while (name[0]!='0');
  people.close();
  people.open(file_name,ios::in);
  people.getline(name,81);
  cout<<"NAME    AGE"<<endl;
  while (!people.eof())
   {
    cout<<name<<endl;
    people.getline(name,81);
   }
  cout<<"Average age: "<<(total/double(count))<<endl;
  people.close();
  cout<<"Hit enter to get out!";
  cin.get();
  return 0;
 }