//Programmer: Trang Luu
//Course	: COSC2315.001
//Program	: Address Book Project
//Date		: Spring 1999

#include "c:\mydocu~1\cosc2315\address\list.h"
#include "c:\mydocu~1\cosc2315\address\function.h"
#include "c:\mydocu~1\cosc2315\address\display.h"
#include "c:\mydocu~1\cosc2315\address\boolean.h"
#include "c:\mydocu~1\cosc2315\address\p_tree.h"

int main()
 {
	int option=1;
	Boolean Again=True;
	Boolean flag=True;
	Boolean exitcase=False;
	PeopleTree AddressBook;
	People APeople;
	ifstream infile;
	ofstream outfile;
	char source[80]="c:\mydocu~1\cosc2315\address\book.dat\0";
	char newfile[80]="c:\mydocu~1\cosc2315\address\book.dat\0";
	char Temp[80];
	char key[40]="Schneider\0";

	infile.open(source);
	if (infile.fail())
	 {
		flag=False;
		cout<<"Cannot load the default "<<source<<" database."<<endl;
		cout<<"Hit enter to continue!";
		cin.get();
	 }
	else
	 {
		cout<<"Loading data...";
		while (infile.peek()!=EOF)
		 {
			ReadRecord(infile,APeople);
			AddressBook.Insert(APeople);
		 }
	 }
	infile.close();
	super_kleen();
	while (Again==True)
	 {
		drawbackground();
		display_main_menu();
		option=control_main_menu(option);
		switch (option)
		 {
		  case 1:
		   instructions();
		   break;
		  case 2:
			if (GetRecord(APeople))
			 {
				if (insert())
				 {
					AddressBook.Insert(APeople);
					flag=True;
				 }
			 }
		   break;
		  case 3:
			if (!flag)
				warning(); //display.h
			else
			 {
				if (InsertKey(key))
				 {
					if (AddressBook.Get(key,APeople))
					 {
						DisplayRecord(APeople); //function.h
						while (getch()!=ESC);
					 }
					else
						notfound(); //display.h
				 }
			 }
		   break;
		  case 4:
			if (!flag)
				warning(); //display.h
			else
			 {
				if (InsertKey(key))
				 {
					if (AddressBook.Get(key,APeople))
					 {
						DisplayRecord(APeople);
						if (agree())
						 {
							AddressBook.Delete(key);
						 }
					 }
					else
					 notfound();
				 }
			 }
		   break;
		  case 5:
		   if (get_input_filename(Temp))
			{
				infile.open(Temp);
				if (infile.fail())
				 {
					notfound();
					infile.clear();
				 }
				else
				 {
					loading();
					flag=True;
					strcpy(source,Temp);
					AddressBook.Destroy();
					while (infile.peek()!=EOF)
					 {
						ReadRecord(infile,APeople);
						AddressBook.Insert(APeople);
					 }
						infile.close();
				 }
			}
		   break;
		  case 6:
			if (over_ride(source))
			 {
				outfile.open(source);
			 }
			else
			 {
				if (get_output_filename(newfile))
				 {
					outfile.open(newfile);
					exitcase=False;
				 }
				else
					exitcase=True;
			 }
			if (outfile.fail()&&exitcase==False)
			 {
				cannotsave();
				outfile.clear();
			 }
			else if (exitcase==False)
			 {
				saving();
				AddressBook.Save(outfile);
				//Save tree by Pre_Order walking.
			 }
			outfile.close();
		   break;
		  case 7:
		   Again=False;
		   break;
		 }
	}
  super_kleen();
  if (flag==True)
   {
	infile.close();
	outfile.close();
   }
  gotoxy(1,1);
  cout<<"Copyright(C) 1999 by Trang Luu - COSC 2315"<<endl;
  return 0;
 }