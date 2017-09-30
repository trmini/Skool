// This library is for the purpose of polish the program
// The interface is very important, so it will be the longest library!

void super_kleen();
//void print_title(constream,char[],int);
void fillbackground();
//Paint background

void drawbackground();
//Give the name of the school

void sub_window(int,int,int,int,int=BLUE);
//Creat a window
void draw_bar(int);

void display_main_menu();

int control_main_menu(int initial_postion);

void display_selftest_menu(BOOLEAN mix_flag);

int control_selftest_menu(int initial_position);


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
//cout<<" F1";
//cout<<" Help ³";
  cout<<" ESC";
  cout<<" Exit";
 }

void sub_window(int upleft_x,int upleft_y,int downright_x,int downright_y,int color)
 {
  int row,column,count;
  window(upleft_x,upleft_y,downright_x+4,downright_y+2);
  textbackground(BLACK);
  clrscr();

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
  sub_window(19,6,61,20);
  textcolor(YELLOW);
  gotoxy(17,1);
  cout<<"A Tutorial"<<endl;
  gotoxy(12,2);
  cout<<"for Rational Numbers"<<endl;
  gotoxy(17,4);
  cout<<"MAIN  MENU"<<endl;
  textcolor(LIGHTRED);
  for (row=1;row<=9;row++)
   {
    gotoxy(3,row+5);
    cout<<row<<".";
   }
  textcolor(WHITE);
  gotoxy(6,6);
  cout<<"Introduction"<<endl;
  gotoxy(6,7);
  cout<<"What is a Rational Number"<<endl;
  gotoxy(6,8);
  cout<<"Express as a Mixed Number"<<endl;
  gotoxy(6,9);
  cout<<"Reduce a Rational Number"<<endl;
  gotoxy(6,10);
  cout<<"Addition and Subtraction"<<endl;
  gotoxy(6,11);
  cout<<"Reciprocal or Multiplicative Inverse"<<endl;
  gotoxy(6,12);
  cout<<"Multiplication and Division"<<endl;
  gotoxy(6,13);
  cout<<"Practice"<<endl;
  gotoxy(6,14);
  cout<<"Exit"<<endl;
  window(1,1,80,25);
  setcrsrtype(0);
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

void draw_bar(int line,int start,int end)
 {
  int x;
  int far *point;
  point=(int far *) 0xB8000000;
  line=line+9;
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

void display_selftest_menu(BOOLEAN mix_flag)
 {
  int row=0;
  sub_window(27,8,55,18);
  textcolor(YELLOW);
  gotoxy(11,1);
  cout<<"PRACTICE"<<endl;
  textcolor(LIGHTRED);
  for (row=1;row<=7;row++)
   {
    gotoxy(3,row+3);
    cout<<row<<".";
   }
  textcolor(WHITE);
  gotoxy(6,4);
  cout<<"Addition"<<endl;
  gotoxy(6,5);
  cout<<"Subtraction"<<endl;
  gotoxy(6,6);
  cout<<"Division"<<endl;
  gotoxy(6,7);
  cout<<"Multiplication"<<endl;
  gotoxy(6,8);
  cout<<"All Operations"<<endl;
  if (mix_flag==TRUE)
   {
    gotoxy(6,9);
    cout<<"Show as normal number"<<endl;
   }
  else
   {
    gotoxy(6,9);
    cout<<"Show as mixed number"<<endl;
   }
  gotoxy(6,10);
  cout<<"Exit"<<endl;
  window(1,1,80,25);
  return;
 }

int control_selftest_menu(int initial_position)
 {
  int line;
  char key;
  line=initial_position;
  draw_bar(line,26,55);
  while ( (key=getch()) != ESC)
   {
    if (key==RETURN) return line;
    else
    if (key==0)
     {
      switch (getch())
       {
	case D_ARRO:
	 draw_bar(line,26,55);
	 line++;
	 if (line==8)
	  {
	    line=1;
	    draw_bar(line,26,55);
	  }
	 else draw_bar(line,26,55);
	 break;
	case U_ARRO:
	 draw_bar(line,26,55);
	 line--;
	 if (line==0)
	  {
	    line=7;
	    draw_bar(line,26,55);
	  }
	 else draw_bar(line,26,55);
	 break;
       }
     }
   }
  return 7;
 }
