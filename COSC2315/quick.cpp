//Qick_Sort
//Programmer: Trang Luu
//COSC 2315.001

#include <constrea.h>
#include <iostream.h>

void quick_sort(int A[], int first, int last);
int partition(int A[], int first, int last);
void swap(int &A, int &B);

int main()
 {
  int A[]={20,12,34,57,56,12,16,8,9,20,39,43,45,23,23,12,456,64,23};
  clrscr();
  cout<<"Unsorted array: ";
  for (int i=0;i<=18;i++)
   {
	cout<<A[i]<<" ";
   }
  quick_sort(A,0,18);
  cout<<endl<<"Sorted array: ";
  for (i=0;i<=18;i++)
   {
	cout<<A[i]<<" ";
   }
  cin.get();
  return 0;
 }

void swap(int &A, int &B)
 {
  int temp;
  temp=A;
  A=B;
  B=temp;
  return;
 }

void quick_sort(int A[],int first, int last)
 {
  if (first<last)
   {
    int p=partition(A,first,last);
    quick_sort(A,first,p-1);
    quick_sort(A,p+1,last);
   }
  return;
 }

int partition(int A[], int first, int last)
 {
  int i=first-1;
  int j=last+1;
  int pivot=(first+last)/2;
  while (i<j)
   {
    do
     j--;
    while (A[j]>=A[pivot]&&j>=pivot);
    do
     i++;
    while (A[i]<=A[pivot]&&i<=pivot);
    if (i<=j)
     {
	swap(A[i],A[j]);
     }
   }
  return j;
 }