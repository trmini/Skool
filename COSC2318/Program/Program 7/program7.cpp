//Trang Luu
//COSC 2318
//Program 7: What is in a name?
//Process a file of name. In the original data file may contain garbage, so this
//program will save only names into a new file.

#include <fstream.h>
#include <iostream.h>
#include <constrea.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

const int max_size=80;

struct name
 {
  char last_name[max_size];
  char first_name[max_size];
  char middle_name[max_size];
 };

void process(char s[],name&data_name);
//Process data so that we will have a name with clear first name, last name and middle name
//String s contain original data, the structure data_name have three fields for three elements
//in a normal name.
//After calling this function, each field of data_name will store exact value of last,first, and
//middle.
//The original data must have last name comes firts, and then last name and middle name. Each element
//should be seperate by non_alphabetical characters.
//middle name can be an option.

void print(ofstream&output,int order,name data_name);
//Print data which is organized into a file
//output is already opened, order is an integer, and struct data_name

void kleen_data(name&data_name);
//Kleen data_name

void kleen_data(char[]);
//Kleen temporary data

int main()
 {
  ifstream input;
  ofstream output;
  int order;
  char file_name[max_size];
  char data[max_size];
  name data_name;
  char answer;
  do
   {
    order=1;
    clrscr();
    cout<<"Welcome to the program 7: What is in a name?"<<endl;
    cout<<endl<<"Please enter your data file with full direction: "<<endl;
    cin>>file_name;
    cin.get();
    input.open(file_name);
    while (input.fail())
     {
      input.clear();
      cout<<"Error when open input file!"<<endl;
      cout<<"Re-enter your file name: ";
      cin>>file_name;
      cin.get();
      input.open(file_name);
     }
    do
     {
      cout<<"Please enter your desired output file with full direction:"<<endl;
      cin>>file_name;
      cin.get();
      cout<<"Are you sure there is no files with this name? (Y/N) ";
      cin>>answer;
      cout<<endl;
     }
    while (toupper(answer)!='Y');
    output.open(file_name);
    if (output.fail())
     {
      cout<<"Cannot create the output file!"<<endl;
      cout<<"Check and run program again."<<endl;
      exit(1);
     }
    output<<"Created by program 7: What is in a name?"<<endl;
    output<<"========================================"<<endl;
    do
     {
      kleen_data(data);
      input.getline(data,max_size-1);
      kleen_data(data_name);
      process(data,data_name);
      print(output,order,data_name);
      order++;
     }
    while (!input.eof());
    input.close();
    output.close();
    cout<<"Done!"<<endl;
    cout<<"Do you want to process one more time? Y/N ";
    cin>>answer;
    cin.get();
   }
  while (toupper(answer)=='Y');
  clrscr();
  return 0;
 }

void print(ofstream&output,int order,name data_name)
 {
  output.setf(ios::left);
  output<<setw(3)<<order<<":    "<<data_name.last_name<<" "<<data_name.first_name<<" "<<data_name.middle_name<<endl;
 }

void process(char data[],name&data_name)
 {
  int index,i;
  for (index=0;index<max_size&&!isalpha(data[index]);index++);
  i=0;
  while (isalpha(data[index]))
   {
    data_name.last_name[i]=data[index];
    data_name.last_name[i+1]='\0';
    index++;
    i++;
   }
  for (;index<max_size&&!isalpha(data[index]);index++);
  i=0;
  while (isalpha(data[index]))
   {
    data_name.first_name[i]=data[index];
    data_name.first_name[i+1]='\0';
    index++;
    i++;
   }
  for (;index<max_size&&!isalpha(data[index])&&data[index]!='\0';index++);
  i=0;
  while (isalpha(data[index]))
   {
    data_name.middle_name[i]=data[index];
    data_name.middle_name[i+1]='\0';
    i++;
    index++;
   }
  return;
 }

void kleen_data(name&data_name)
 {
  for (int index=0;index<max_size;index++)
   {
    data_name.last_name[index]='\0';
    data_name.first_name[index]='\0';
    data_name.middle_name[index]='\0';
   }
  return;
 }

void kleen_data(char data[])
 {
  for (int index=0;index<max_size;index++);
   data[index]='\0';
  return;
 }