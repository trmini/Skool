//Merge-Sort with linked list
//Programmer: Trang Luu
//COSC 2315.001
//San Antonio College
//Comment: This is the most solid code I've ever thought about for Merge-Sort.
//Thanks God I cannot find any mistake.

#include <iostream.h>
#include <constrea.h>
#include "a:\linked.h"

void merge_sort(ListClass<int> A,int first,int last);
void merge(ListClass<int> A,int first,int middle,int last);

int main()
 {
  ListClass<int> A;
  int temp;
  for (int i=1;i<=20;i++) A.InsertItem(1,random(100));
  clrscr();
  cout<<"Unsorted array: ";
  for (i=1;i<=20;i++)
   {
	A.GetItem(i,temp);
	cout<<temp<<" ";
   }
  merge_sort(A,1,20);
  cout<<endl<<"Sorted array: ";
  for (i=1;i<=20;i++)
   {
	A.GetItem(i,temp);
	cout<<temp<<" ";
   }
  cin.get();
  return 0;
 }

void merge_sort(ListClass<int> A,int first,int last)
 {
  if (first<last)
   {
	int middle=(first+last)/2;
	merge_sort(A,first,middle);
	merge_sort(A,middle+1,last);
	merge(A,first,middle,last);
   }
  return;
 }

void merge(ListClass<int> A,int first, int middle, int last)
 {
  int *temp=new int[(last-first)];
  int count=0;
  int first_l=first;
  int first_r=middle+1;
  int A_first_l,A_first_r;
  A.GetItem(first_l,A_first_l);
  A.GetItem(first_r,A_first_r);
  while ((first_l<=middle)&&(first_r<=last))
   {
	if (A_first_l<=A_first_r)
	 {
		temp[count]=A_first_l;
		first_l++;
		A.GetItem(first_l,A_first_l);
	 }
	else
	 {
		temp[count]=A_first_r;
		first_r++;
		A.GetItem(first_r,A_first_r);
	 }
	count++;
   }
  if (first_l>=middle)
   {
	for (int index=first_r;index<=last;index++)
	 {
	  A.GetItem(index,temp[count]);
	  count++;
	 }
   }
  if (first_r>=last)
   {
	for (int index=first_l;index<=middle;index++)
	 {
	  A.GetItem(index,temp[count]);
	  count++;
	 }
   }
  for (count=first;count<=last;count++)
   {
	A.DeleteItem(count);
	A.InsertItem(count,temp[count-first]);
   }
  delete temp;
  return;
 }