void display_procedure(char file_name[])
 {
  ifstream input;
  input.open(file_name);
  if (!input.fail())
   {
      char data;
      drawbackground();
      sub_window(5,4,75,21);
      textcolor(YELLOW);
      input.get(data);
      int column=1,row=1;
      while (!input.eof()&&row<=18)
       {
	if (data=='\n')
	 {
	  row++;
	  column=0;
	 }
	else
	 {
	  gotoxy(column,row);
	  cout<<data;
	 }
	input.get(data);
	column++;
       }
      while (getch()!=ESC);
      input.close();
      window(1,1,80,25);
   }
  else
   {
    input.clear();
    drawbackground();
    sub_window(30,10,50,13,RED);
    gotoxy(1,2);
    cout<<"   File not found!";
    gotoxy(1,3);
    cout<<" Press ESC to return";
    while (getch()!=ESC);
    window(1,1,80,25);
   }
  return;
 }

int practice()
 {
  int lesson=1;
  BOOLEAN again=TRUE,mix_flag=FALSE;
  drawbackground();
  while (again==TRUE)
   {
    display_selftest_menu(mix_flag);
    lesson=control_selftest_menu(lesson);
    switch (lesson)
     {
      case 1:
      case 2:
      case 3:
      case 4:
       self_test(lesson,mix_flag);
       break;
      case 5:
       self_test(-1,mix_flag);
       break;
      case 6:
       window(1,1,80,25);
       gotoxy(1,25);
       textcolor(WHITE);
       textbackground(RED);
       clreol();
       cout<<"ESC Return";
       sub_window(30,12,50,16,RED);
       if (mix_flag==FALSE)
	{
	 mix_flag=TRUE;
	 gotoxy(1,2);
	 cout<<"Every fraction number";
	 gotoxy(1,3);
	 cout<<"  will be displayed";
	 gotoxy(1,4);
	 cout<<"   as mixed number";
	 while (getch()!=ESC);
	}
       else
	{
	 mix_flag=FALSE;
	 gotoxy(1,2);
	 cout<<"Every fraction number";
	 gotoxy(1,3);
	 cout<<"   will be showed";
	 gotoxy(1,4);
	 cout<<"  as normal number";
	 while (getch()!=ESC);
	}
       break;
      case 7:
       again=FALSE;
       break;
     }
   }
  return 0;
 }