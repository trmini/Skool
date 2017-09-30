#include "c:\mydocu~1\cosc\project\display.h"
//#include "c:\mydocu~1\cosc\project\rational.h"

int main()
 {
  int option=0;
  while (option>-1) //Infinitive Loop
   {
    super_kleen();
    drawbackground();
    display_main_menu();
    option=control_main_menu();
    switch (option)
     {
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
	//try_apply();
	break;
      case 9:
	super_kleen();
	cout<<"Copyright(C) 1998 by Trang Luu - COSC2318"<<endl;
	exit(1);
	break;
     }
   }
  return 0;
 }