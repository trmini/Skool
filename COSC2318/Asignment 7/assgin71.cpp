#include <iostream.h>

void initialize(int[],int);
float mean(int[],int);
int median(int[],int);

void initialize(int x[],int size)
 {
  int index=0;
  for (;index<size;index++) x[index]=index;
 }

float mean(int x[], int size)
 {
  int index=0;
  int sum=0;
  for (;index<size;index++) sum=sum+x[index];
  return (float(sum)/size);
 }

int median(int x[], int size)
 {
  int index;
  if (size%2!=0) return x[size/2];
  else
   {
    index=size/2;
    return (x[index]+x[index-1])/2;
   }
 }