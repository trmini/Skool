//Assignment 9

#include <string.h>
#include <iostream.h>
#include <constrea.h>
#include <ctype.h>

enum YES_NO {NO,YES};

YES_NO check_oh(const char[]);
void add_period(char[]);
char punctuation(const char[]);
int count_uppercase(const char[]);
void pluralize(char[]);
void repeat(char[],int);
void reverse(char[]);
void LTRIM(char[]);
void RTRIM(char[]);

void remove_char(char[],int);
void insert_char(char[],int,char);

template <class T>
void swap(T&first, T&second)
 {
  T temp;
  temp=first;
  first=second;
  second=temp;
  return;
 }

int main ()
 {
  return 0;
 }

YES_NO check_oh(const char s[])
 {
  int length=strlen(s);
  if (toupper(s[length-1])=='H'&&toupper(s[length-2])=='O')
   return YES;
  else
   return NO;
 }

void add_period(char s[])
 {
  strcat(s,".");
  return;
 }

char punctuation(char s[])
 {
  int length=strlen(s);
  char temp=s[length-1];
  if (temp=='!') return  temp;
  else return(' ');
 }

int count_uppercase(const char s[])
 {
  int count=0;
  for (int index=0;index<strlen(s);index++)
   {
    if (s[index]>='A'&&s[index]<='Z') count++;
   }
  return count;
 }

void plularize(char s[])
 {
  int length=strlen(s);
  if (s[length-1]=='y')
   {
    s[length-1]='\0'; //Remove 'y'
    strcat(s,"ies");
   }
  else
   {
    if (s[length-1]=='s'||(s[length-1]=='h'&&(s[length-2]=='c'||s[length-2]=='s')))
     strcat(s,"es");
    else
     strcat(s,"s");
   }
  return;
 }

void repeat(char s[],int n)
 {
  char temp[81];
  int index=0;
  while (index<strlen(s))
   {
    temp[index]=s[index];
    index++;
   }
  for (;n>1;n--)
   {
    strcat(s,temp);
   }
  return;
 }

void reverse(char s[])
 {
  int length=strlen(s);
  for (int index=0;index<(length/2);index++)
   swap(s[index],s[length-1-index]);
  return;
 }

void LTRIM(char s[])
 {
  for (int index=0;index<strlen(s);index++)
   {
    if (isspace(s[0])) remove_char(s,0);
    else index=strlen(s);
   }
  return;
 }

void RTRIM(char s[])
 {
  for (int index=0;index<strlen(s);index++)
   {
    if (isspace(s[index]))
     {
      s[index]='\0';
      index=strlen(s);
     }
   }
  return;
 }

//Basic function for string

void remove_char(char s[], int position)
 {
  for (int index=position;index<strlen(s);index++);
   s[index]=s[index+1];
  return;
 }

void insert_char(char s[], int position,char item)
 {
  for (int i=strlen(s);i>position;i--)
   s[i]=s[i-1];
  s[i-1]=item;
  return;
 }