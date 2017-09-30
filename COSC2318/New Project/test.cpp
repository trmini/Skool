#include "a:\include.h"
#include "a:\rational.h"

int main()
 {
  rational_number alpha;
  clrscr();
  cout<<"Input a formal rational number: ";
  get_fraction(alpha);
  show_mixed(alpha);
  cin.get();
 }