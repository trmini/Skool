#include "c:\mydocu~1\cosc\code\include.h"
#include "c:\mydocu~1\cosc\code\display.h"
#include "c:\mydocu~1\cosc\code\practice.h"
#include "c:\mydocu~1\cosc\code\subtasks.h"

int main()
 {
  int option=1;
  BOOLEAN again=TRUE;
  while (again==TRUE)
   {
	super_kleen();
    drawbackground();
    display_main_menu();
    option=control_main_menu(option);
    switch (option)
     {
      case 1:
       display_procedure("data1.dta");
       break;
      case 2:
       display_procedure("data2.dta");
       break;
      case 3:
       display_procedure("data3.dta");
       break;
      case 4:
       display_procedure("data4.dta");
       break;
      case 5:
       display_procedure("data5.dta");
       break;
      case 6:
       display_procedure("data6.dta");
       break;
      case 7:
       display_procedure("data7.dta");
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
  gotoxy(1,1);
  cout<<"Copyright(C) 1998 by Trang Luu - COSC 2318"<<endl;
  return 0;
 }
