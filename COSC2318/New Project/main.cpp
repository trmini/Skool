#include "a:\display.h"
enum BOOLEAN{FALSE,TRUE};

int main()
 {
  int option;
  BOOLEAN again=TRUE;
  while (again==TRUE)
   {
    super_kleen();
    fillbackground();
    display_main_menu();
    option=control_main_menu();
    switch (option)
     {
      case 1:
       instruction();
       break;
      case 2:
       add_sub();
       break;
      case 3:
       reciprocal();
       break;
      case 4:
       mul_div();
       break;
      case 5:
       reduce_rational();
       break;
      case 6:
       mixed();
       break;
      case 7:
       compare();
       break;
      case 8:
       practice();
       break;
      case 9:
       again=FALSE;
       break;
     }
   }
  super_kleen();
  cout<<"Copyright(C) 1998 by Trang Luu - COSC 2318"<<endl;
  return 0;
 }
