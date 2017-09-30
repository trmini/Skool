#include <fstream.h>
#include <iostream.h>
#include <constrea.h>
#include <ctype.h>
#include <stdlib.h>

enum BOOLEAN{FALSE,TRUE};

template <class T>
//This function swaps two value
void swap_value(T&first,T&second)
 {
  T temp;
  temp=second;
  second=first;
  first=temp;
  return;
 }

template <class T>
//This function sort data by using buble method.
//You give the function array and size
//Function will sort data and save the number of swap and compare
void buble(T x[], int size, int& buble_swap, int& buble_compare)
 {
  BOOLEAN sorted=FALSE;
  int compare_count=0,swap_count=0,i,pass=1;
  while (!sorted)
   {
    sorted=TRUE;
    for (i=0;i<size-pass;i++)
     {
      compare_count++;
      if (x[i]>x[i+1])
       {
	swap_value(x[i],x[i+1]);
	swap_count++;
	sorted=FALSE;
       }
     }
    pass++;
   }
  buble_swap=swap_count;
  buble_compare=compare_count;
  return;
 }

template <class T>
//This function return the postion of the cell has the minimum value
//in an array.
//You give the fuction the array and its size
//For this particular program, you also need to lead the function to the
//start position. This function also save the number of compare in the variable
//compare_count
int min_position(T x[],int start_pos,int size,int&compare_count)
 {
  T min_value=x[start_pos];
  int min_pos=start_pos;
  for (int index=start_pos+1;index<size;index++)
   {
    compare_count++;
    if (x[index]<min_value)
     {
      min_value=x[index];
      min_pos=index;
     }
   }
  return min_pos;
 }

template <class T>
//This is the function to sort by selection
//You give the function your array and the size
//The select_swap and select_compare will contain the number of swap and
//compare after processing data.
void selection(T x[],int size,int&select_swap,int&select_compare)
 {
  int index,min_pos;
  int swap_count=0,compare_count=0;
  for(index=0;index<size-1;index++)
   {
    min_pos=min_position(x,index,size,compare_count);
    if (min_pos!=index)
     {
      swap_value(x[index],x[min_pos]);
      swap_count++;
     }
   }
  select_swap=swap_count;
  select_compare=compare_count;
  return;
 }

template <class T>
//This function gets data from a file
//Data should be in the same type and separated by white spaces
//Give the data your array name, the name of ifstream, and the name of
//a variable will containt array size
void get_data(T stored_data[],ifstream&data_file,int& array_size)
 {
  int index=0;
  while (data_file>>stored_data[index]) index++;
  array_size=(index);
 }

template <class T>
// This function will print data to the output file
// Arrays will be printed by cell
// You need to give the function name of ofstream,three arrays, and its size
// Because it is for this program, it supposes three arrays have the same size
void print_data(ofstream& output_file,T original[],T copy1[],T copy2[],int array_size)
 {
  int index=0;
  for (;index<array_size;index++)
   {
    output_file<<setw(7)<<original[index]<<setw(30)<<copy1[index]<<setw(12)<<copy2[index];
    output_file<<endl;
   }
  return;
 }

void print_header(ofstream&);
//Print the header for the output file;

void print_data(ofstream&,int swap,int compare);
//Print the values of swap and compare to the output file

//++Definition++

void print_header(ofstream&output)
 {
  output<<"Created by PROGRAM 5 - A Sorting Machine"<<endl;
  output<<"========================================"<<endl;
  return;
 }

void print_data(ofstream&output,int swap,int compare)
 {
  output<<setw(10)<<swap<<setw(12)<<compare<<endl;
  return;
 }