// Trang Luu
// COSC 2318.001
// Program 4: WordPerfect Move Over
// Due date: 10/14/98
// Count the nonblank characters, alphabetical characters, nonblank lines,
// words, and sentences in a text file.
// Then calculate the average number of characters per word and the average
// number of words per sentence.

#include <constrea.h>
#include <ctype.h>
#include <fstream.h>
#include <iostream.h>
#include <math.h>
#include <stdlib.h>


char target[50], source[50];
int nonblank, alphabetical, line, word, sentence;
ifstream input;
ofstream output;
//Globle variables

void check_data(char,char);
//This function checks a character and its following character in order
//to process data.
//check_data(temp1,temp2);
//temp1 is read from file and temp2 is character that follows temp1
//Depends on values of temp1 and temp2 this function will change the
//values of nonblank, alphabetical, line, word, and sentence.


void write_data();
//Write data to output file after complete processing

void welcome();
//Give some ideas about the program to user

int main()
 {
  char answer='Y', temp1, temp2;

  clrscr();
  welcome();
  do
  {
   cout<<"Please enter your desired output file with full direction:"<<endl;
   cin>>target;
   cin.get();
   cout<<"Are you sure there is no files with this name? (Y/N) ";
   cin>>answer;
   cin.get();
   cout<<endl;
  }
  while (toupper(answer)!='Y');
  output.open(target);
  if (output.fail())
   {
    cout<<"Cannot create the output file!"<<endl;
    cout<<"Check and run program again."<<endl;
    exit(1);
   }
  output<<"Created by PROGRAM4 - WordPerfect Move Over!"<<endl;
  output<<"N/A = Not Applicable"<<endl<<endl;
  output.close();
  clrscr();

  while (toupper(answer)=='Y')
   {
    nonblank=0;
    alphabetical=0;
    line=0;
    word=0;
    sentence=0;

    cout<<"Please enter your text file with full direction:"<<endl;
    cin>>source;
    cin.get();
    input.open(source);
    while (input.fail())
     {
      input.clear();
      cout<<"Error when open input file!"<<endl;
      cout<<"Re-enter your file name: ";
      cin>>source;
      cin.get();
      input.open(source);
     }
    input.get(temp1);
    while (!input.eof())
     {
      input.get(temp2);
      check_data(temp1,temp2);
      temp1=temp2;
     }
    input.close();
    output.open(target, ios::app);
    if(output.fail())
     {
      cout<<"Error when trying to save data!"<<endl;
      exit(1);
     }
    write_data();
    output.close();
    cout<<"Done!"<<endl;
    cout<<"Results have been stored in "<<target<<endl;
    cout<<"Do you want to process one more file? Y/N ";
    cin.get(answer);
    cin.get();
   }
  cout<<"Thank you!"<<endl; 
  return 0;
 }

void check_data(char temp1, char temp2)
{

 if (int(temp1)>=127) return;
 //Skip character that is not in the first table;

 if (!isspace(temp1)) //Nonblank?
  {
   nonblank++;
  }

 if (temp1!='\n'&&(temp2=='\n'||int(temp2)==-1)) line++; //Line?
//Suppose that a blank-line is a line which has only return character.
//That means a line with spaces,tabs, etc... is a nonblank-line.
 if (isalpha(temp1)) //Alphebetical character?
  {
   alphabetical++;
   if (ispunct(temp2)||isspace(temp2)||int(temp2)==-1) //Word?
    {
     word++;
     if (int(temp2)==46) sentence++; //Sentence?
    }
  }
 return;
}

void write_data()
 {
  output<<"Checked file: "<<source<<endl<<endl;
  output<<"Nonblank character(s)    : "<<nonblank<<endl;
  output<<"Alphabetical character(s): "<<alphabetical<<endl;
  output<<"Nonblank line(s)         : "<<line<<endl;
  output<<"Word(s)                  : "<<word<<endl;
  output<<"Sentence(s)              : "<<sentence<<endl;
  output<<"Average number of characters per word : ";
  if (word==0) output<<"N/A"<<endl;
  else output<<ceil((double(alphabetical)/word)-0.5)<<endl;
  output<<"Average number of words per sentence  : ";
  if (sentence==0) output<<"N/A"<<endl;
  else output<<ceil((double(word)/sentence)-0.5)<<endl;
  output<<"============================================="<<endl<<endl;
  return;
 }

void welcome()
{
 cout<<"Welcome!"<<endl<<endl;
 cout<<"     This small program will help you count the nonblank characters, the"<<endl;
 cout<<"alphabetical characters, the nonblank lines, the words, and the sentence"<<endl;
 cout<<"in your text file. After that, it also gives the average number of char-"<<endl;
 cout<<"acters per word and words per sentence."<<endl;
 cout<<"     For one run, you can check as many files as you want.  Results will"<<endl;
 cout<<"be stored in one file."<<endl<<endl;
 return;
}
