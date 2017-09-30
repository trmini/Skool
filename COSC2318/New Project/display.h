// This library is for the purpose of polish the program
// The interface is very important, so it will be the longest library!

#define LENGTH 2000

#define RETURN 13
#define ESC 27
#define U_ARRO 72
#define D_ARRO 80

void super_kleen();
//void print_title(constream,char[],int);
void fillbackground();

void drawbackground();
//Give the name of the school

void sub_window(constream&,int,int,int,int);
//Creat a window
void display_main_menu();
int control_main_menu();
void draw_bar(int);

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
  gotoxy(1,1);
  clreol();
  cout<<"          San Antonio College - Department of Math and Computer Science";
  gotoxy(1,25);
  clreol();
  cout<<" F1 Help";
  textbackground(BLACK);
  textcolor(LIGHTGRAY);
 }

void sub_window(constream&inside_window,int upleft_x,int upleft_y,int downright_x,int downright_y)
 {
  constream shadow,outline;
  int row,column,count;
  //int far *point;

  //MAKE SHADOW
  //point=(int far *) 0xB8000000;
  //for (column=upleft_x-1;column<(downright_x+4);column++)
  // for (row=upleft_y-1;row<(downright_y+2);row++)
  //  {
  //   *(point+80*row+column)=*(point+80*row+column)-0x6A00+0x0002;// 0x0700; //Change
  //  }
  shadow.window(upleft_x,upleft_y,downright_x+4,downright_y+2);
  shadow<<setattr((BLACK<<4))<<setcrsrtype(0);
  shadow.clrscr();

  //MAKE OUTLINE
  outline.window(upleft_x-2,upleft_y-1,downright_x+2,downright_y+1);
  outline<<setattr((BLUE<<4)|WHITE)<<setcrsrtype(0);
  outline.clrscr();
  //Corners
  column=(downright_x-upleft_x+4);
  row=(downright_y-upleft_y+2);
  outline<<setxy(2,1)<<char(201);        // �
  outline<<setxy(2,row+1)<<char(200);   //   �
  outline<<setxy(column,1)<<char(187);      // �
  outline<<setxy(column,row+1)<<char(188); // �
  for (count=2;count<=row;count++)
   {
    outline<<setxy(2,count)<<char(186);      // �
    outline<<setxy(column,count)<<char(186); // �
   }
  for (count=3;count<column;count++)
   {
    outline<<setxy(count,1)<<char(205);      // �
    outline<<setxy(count,row+1)<<char(205);    // �
   }
  //CREATE ACCESSABLE WINDOW
  inside_window.window(upleft_x,upleft_y,downright_x,downright_y);
  inside_window<<setattr((BLUE<<4)|WHITE)<<setcrsrtype(0);
  inside_window.clrscr();
  return;
 }

//Definition for sub_function

void display_main_menu()
 {
  constream main_menu;
  int row=0;
  sub_window(main_menu,19,6,61,20);
  textcolor(YELLOW);
  main_menu<<setxy(17,1)<<"A Tutorial"<<endl;
  main_menu<<setxy(12,2)<<"for Rational Numbers"<<endl;
  main_menu<<setxy(17,4)<<"MAIN  MENU"<<endl;
  textcolor(LIGHTRED);
  for (row=1;row<=9;row++)  main_menu<<setxy(3,row+5)<<row<<".";
  textcolor(WHITE);
  main_menu<<setxy(6,6)<<"Instructions"<<endl;
  main_menu<<setxy(6,7)<<"Addition and Subtraction"<<endl;
  main_menu<<setxy(6,8)<<"Reciprocal or Multiplicative Inverse"<<endl;
  main_menu<<setxy(6,9)<<"Multiplication and Division"<<endl;
  main_menu<<setxy(6,10)<<"Reduce a Rational Number"<<endl;
  main_menu<<setxy(6,11)<<"Express As a Mixed Number"<<endl;
  main_menu<<setxy(6,12)<<"Compare Two Rational Numbers"<<endl;
  main_menu<<setxy(6,13)<<"Practice"<<endl;
  main_menu<<setxy(6,14)<<"Quit"<<endl;
  return;
 }

void draw_bar(int line)
 {
  int x;
  int far *point;
  point=(int far *) 0xB8000000;
  line=line+9;
  for (x=18;x<61;x++)
   {
    *(point+80*(line)+x)=*(point+80*(line)+x)^0x3000;
    //Change background for toolbar
    // Use XOR so the original value will be returned after second calls
   }
 }

int control_main_menu()
 {
  int line;
  char key;
  line=1;
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
	 if (line==10)
	  { line=1;
	    draw_bar(line);
	  }
	 else draw_bar(line);
	 break;
	case U_ARRO:
	 draw_bar(line);
	 line--;
	 if (line==0)
	  { line=9;
	    draw_bar(line);
	  }
	 else draw_bar(line);
	 break;
       }
     }
   }
  return 9;
 }
