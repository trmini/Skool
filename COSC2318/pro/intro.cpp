#include "a:\include.h"
#include "a:\display.h"

int main() //intstruction()
 {
  char buffer[4096];
  ifstream input;
  gettext(1,1,80,25,buffer); //Get current screen
  input.open("a:\data1.dta");
  if (!input.fail())
   {
      constream screen;
      char data;
      drawbackground();
      sub_window(screen,5,4,75,21);
      textcolor(YELLOW);
      input.get(data);
      while (!input.eof())
       {
	screen<<data;
	input.get(data);
       }
      while (getch()!=ESC);
   }
  else
   {
    drawbackground();
    constream screen;
    sub_window(screen,30,10,50,13);
    screen<<endl<<"   File not found!"<<endl;
    screen<<" Press ESC to return";
    while (getch()!=ESC);
   }
  puttext(1,1,80,25,buffer); //Draw screen back
  return 0;
 }