// Introductory programing assignment 0
// Programmer : Thanh Nguyen
// Class      : CS 2073-001
// Due Date   : September 23, 1998

#include <stdio.h>

void main (void)
 {
  float principle, rate, interest;
  int time;
  // Collecting data
  printf(" Input principle : ");
  scanf("%f", &principle);
  printf(" Input rate (%) and time (days) : ");
  scanf("%f %i", &rate, &time);
  // Calculation
  interest=principle*rate/100*time/365;
  // Giving result
  printf(" Interest is : $%8.2f\n",interest);
 }