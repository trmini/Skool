// This library is for the purpose of polish the program
// The interface is very important, so it will be the longest library!
#include "c:\mydocu~1\cosc2315\address\Boolean.h"
#ifndef DISPLAY_STUFF_H
#define DISPLAY_STUFF_H

//Essential function
void super_kleen();

void fillbackground();
//Paint background

void drawbackground();
//Give the name of the school

void sub_window(int,int,int,int,int=BLUE);
//Creat a window

void draw_bar(int);

void display_main_menu();

int control_main_menu(int initial_postion);
//End of essential function

//Display message
void instructions();
void warning();
void notfound();
void cannotsave();
void saving();
void loading();


//++++++++Definition+++++++++++

void super_kleen()
 {
  int far *point;
  int address;
  point=(int far *) 0xB8000000;
  for (address=0;address<LENGTH;address++) *(point+address)=0x0700;
 }

void fillbackground()
 {
  int far *point;
  int address;
  point=(int far *) 0xB8000000;
  for (address=0;address<LENGTH; address++)
   *(point+address)=0x71B0; //char(178)|0x1700
 }

void drawbackground()
 {
  fillbackground();
  textbackground(4);
  textcolor(15);
  window(1,1,80,25);
  gotoxy(1,1);
  clreol();
  cout<<"          San Antonio College - Department of Math and Computer Science";
  gotoxy(1,25);
  clreol();
  textcolor(BLACK);
  cprintf(" F1");
  cout<<" Help";
  cout<<" ³";
  cprintf(" ESC");
  cout<<" Exit";
 }

void sub_window(int upleft_x,int upleft_y,int downright_x,int downright_y,int color)
 {
  int row,column,count;
  int far *point;
  point=(int far *) 0xB8000000;
  for (row=upleft_y-1;row<downright_y+2;row++)
	for (column=upleft_x-1;column<downright_x+4;column++)
	 {
		*(point+row*80+column)=*(point+row*80+column)&0x00FF; //Make neutral color
		*(point+row*80+column)=*(point+row*80+column)|0x0800; //Dark background and text
	 }
 /*
  window(upleft_x,upleft_y,downright_x+4,downright_y+2);
  textbackground(BLACK);
  clrscr();
  This is the old version of shadow
 */

  //MAKE OUTLINE
  window(upleft_x-2,upleft_y-1,downright_x+2,downright_y+1);
  textbackground(color);
  textcolor(WHITE);
  clrscr();
  //Corners
  column=(downright_x-upleft_x+4);
  row=(downright_y-upleft_y+2);
  gotoxy(2,1);
  cout<<char(201);        // É
  gotoxy(2,row+1);
  cout<<char(200);   //   È
  gotoxy(column,1);
  cout<<char(187);      // »
  gotoxy(column,row+1);
  cout<<char(188); // ¼
  for (count=2;count<=row;count++)
   {
	gotoxy(2,count);
	cout<<char(186);      // º
	gotoxy(column,count);
	cout<<char(186); // º
   }
  for (count=3;count<column;count++)
   {
	gotoxy(count,1);
	cout<<char(205);      // Í
	gotoxy(count,row+1);
	cout<<char(205);    // Í
   }
  //CREATE ACCESSABLE WINDOW
  window(upleft_x,upleft_y,downright_x,downright_y);
  textbackground(color);
  textcolor(WHITE);
  clrscr();
  return;
 }

//Definition for sub_function

void display_main_menu()
 {
  int row=0;
  int x1=30,x2=50,y1=7,y2=16;
  int options=7;
  sub_window(x1,y1,x2,y2);
  setcrsrtype(0);
  textcolor(YELLOW);
  gotoxy((x2-x1-9)/2+1,1);
  cout<<"MAIN  MENU"<<endl;
  textcolor(LIGHTRED);
  for (row=1;row<=options;row++)
   {
	gotoxy(3,row+2);
	cout<<row<<".";
   }
  textcolor(WHITE);
  gotoxy(6,3);
  cout<<"Instructions"<<endl;
  gotoxy(6,wherey());
  cout<<"Insert Record"<<endl;
  gotoxy(6,wherey());
  cout<<"View Record"<<endl;
  gotoxy(6,wherey());
  cout<<"Delete Record"<<endl;
  gotoxy(6,wherey());
  cout<<"Load Data"<<endl;
  gotoxy(6,wherey());
  cout<<"Save Data"<<endl;
  gotoxy(6,wherey());
  cout<<"Quit"<<endl;
  window(1,1,80,25);
  return;
 }

void draw_bar(int line)
 {
  int x;
  int far *point;
  point=(int far *) 0xB8000000;
  line=line+7;
  for (x=29;x<50;x++)
   {
	*(point+80*(line)+x)=*(point+80*(line)+x)^0x3000;
	//Change background for toolbar
	// Use XOR so the original value will be returned after second calls
   }
 }

void draw_bar(int line,int start,int end)
 {
  int x;
  int far *point;
  point=(int far *) 0xB8000000;
  line=line+7;
  for (x=start;x<end;x++)
   {
	*(point+80*(line)+x)=*(point+80*(line)+x)^0x3000;
	//Change background for toolbar
	// Use XOR so the original value will be returned after second calls
   }
 }

int control_main_menu(int initial_position)
 {
  int line;
  char key;
  line=initial_position;
  draw_bar(line);
  while ( (key=getch()) != ESC)
   {
	if (key==RETURN) return line;
	else
	if (key==0)
	 {
	  switch (getch())
	   {
		case D_ARRO:
			draw_bar(line);
			line++;
			if (line==8)
			 {
				line=1;
				draw_bar(line);
			 }
			else draw_bar(line);
		break;
		case U_ARRO:
			draw_bar(line);
			line--;
			if (line==0)
			 {
				line=7;
				draw_bar(line);
			 }
			else draw_bar(line);
		break;
	   }
	 }
   }
  return 7;
 }

void instructions()
 {
	int x1=24,x2=57,y1=12,y2=12;
	sub_window(x1,y1,x2,y2,RED);
	gotoxy(2,1);
	textcolor(YELLOW);
	cprintf("Sorry! I haven't done this part!");
	while (getch()!=ESC);
	return;
 }

void warning()
 {
	int x1=22,x2=59,y1=12,y2=12;
	sub_window(x1,y1,x2,y2,RED);
	gotoxy(2,1);
	textcolor(YELLOW);
	cprintf("Sorry! You need to load data first!");
	while (getch()!=ESC);
	return;
 }

void notfound()
 {
	int x1=33,x2=48,y1=9,y2=11;
	sub_window(x1,y1,x2,y2,RED);
	gotoxy(4,2);
	textcolor(YELLOW);
	cprintf("Not found!");
	while (getch()!=ESC);
	return;
 }

void cannotsave()
 {
	int x1=28,x2=52,y1=10,y2=14;
	sub_window(x1,y1,x2,y2,RED);
	gotoxy(3,2);
	textcolor(YELLOW);
	cprintf("Cannot save to disk!");
	gotoxy(3,3);
	cprintf(" Please try again!");
	while (getch()!=ESC);
	return;
 }

void saving()
 {
	int x1=24,x2=57,y1=14,y2=14;
	sub_window(x1,y1,x2,y2,LIGHTGRAY);
	gotoxy(2,1);
	textcolor(BLACK);
	cprintf("Saving...");
	return;
 }

void loading()
 {
	int x1=24,x2=57,y1=14,y2=14;
	sub_window(x1,y1,x2,y2,LIGHTGRAY);
	gotoxy(2,1);
	textcolor(BLACK);
	cprintf("Loading...");
	return;
 }
#endif