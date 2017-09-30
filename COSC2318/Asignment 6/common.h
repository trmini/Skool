// This is a library that containts common taskts
// Be carefull to read every taskt before using

#include <math.h>

enum BOOLEAN {FALSE,TRUE};
void Kleenbuf();

void Kleenbuf()
 {
  cin.get();
  while (cin.get()!='\n') cin.get();
  return;
 }