//Merge-Sort
//Programmer: Trang Luu
//COSC 2315.001
//San Antonio College
//Comment: This is the most solid code I've ever written for Merge-Sort.

#include <iostream.h>
#include <constrea.h>

void merge_sort(int A[],int first,int last);
void merge(int A[],int first,int middle,int last);

int main()
 {
  int A[]={20,12,34,57,56,12,16,8,9,20,39,43,45,23,23,12,456,64,23};
  clrscr();
  cout<<"Unsorted array: ";
  for (int i=0;i<=18;i++)
   {
	cout<<A[i]<<" ";
   }
  merge_sort(A,0,18);
  cout<<endl<<"Sorted array: ";
  for (i=0;i<=18;i++)
   {
	cout<<A[i]<<" ";
   }
  cin.get();
  return 0;
 }

void merge_sort(int A[],int first,int last)
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

void merge(int A[],int first, int middle, int last)
 {
  int *temp;
  temp=new int[(last-first)];
  int count=0;
  int first_l=first;
  int first_r=middle+1;
  while ((first_l<=middle)&&(first_r<=last))
   {
	if (A[first_l]<=A[first_r])
	 {
		temp[count]=A[first_l];
		first_l++;
	 }
	else
	 {
		temp[count]=A[first_r];
		first_r++;
	 }
	count++;
   }
  if (first_l>=middle)
   {
	for (int index=first_r;index<=last;index++)
	 {
	  temp[count]=A[index];
	  count++;
	 }
   }
  if (first_r>=last)
   {
	for (int index=first_l;index<=middle;index++)
	 {
	  temp[count]=A[index];
	  count++;
	 }
   }
  for (count=first;count<=last;count++)
   {
	A[count]=temp[count-first];
   }
  delete temp;
  return;
 }