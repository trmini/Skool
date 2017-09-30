<HTML><FONT  SIZE=4 PTSIZE=12 FAMILY="SANSSERIF" FACE=".VnCourier" LANG="0">/******************************************/<BR>
/*HOME WORK 5-Check and highlight vowel(s)*/<BR>
/*in a string                             */<BR>
/*Programmers:                            */<BR>
/*MICHAEL GRAY                            */<BR>
/*ERIC NIELSEN                            */<BR>
/*CAROL HRICIHAK                          */<BR>
/*THANH NGUYEN                            */<BR>
/*Class: CS2073.001                       */<BR>
/******************************************/<BR>
<BR>
#include <stdio.h><BR>
#include <ctype.h><BR>
#include <string.h><BR>
<BR>
#define MAX 82<BR>
<BR>
int isvowel(char);<BR>
<BR>
void main(void)<BR>
 {<BR>
  char string[MAX];<BR>
  int index;<BR>
  printf("Input your string: ");<BR>
  gets(string);<BR>
  printf("Highlighted vowel(s):\n");<BR>
  for (index=0;index<strlen(string);++index)<BR>
   {<BR>
    if (isvowel(string[index])!=0)<BR>
     {<BR>
      putchar('<');<BR>
      putchar(toupper(string[index]));<BR>
      putchar('>');<BR>
     }<BR>
    else<BR>
     {<BR>
      putchar(string[index]);<BR>
     }<BR>
   }<BR>
  printf("\n");<BR>
  return;<BR>
 }<BR>
int isvowel(char test_element)<BR>
 {<BR>
  char temp;<BR>
  temp=toupper(test_element);<BR>
  if (temp=='A'||temp=='E'||temp=='I'||temp=='O'||temp=='U')<BR>
   return 1;<BR>
  else return 0;<BR>
 }</HTML>
