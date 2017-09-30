#include <iostream.h>
#include <ctype.h>
#include <conio.h>
#include <string.h>
#include "c:\mydocu~1\cosc2315\address\Boolean.h"
#include "c:\mydocu~1\cosc2315\address\People.h"
#include "c:\mydocu~1\cosc2315\address\P_tree.h"
#include "c:\mydocu~1\cosc2315\address\Display.h"

#ifndef EXTRA_FUNCTION_H
#define EXTRA_FUNCTION_H

void kleenbuff();
Boolean GetRecord(People& APeople);
void DisplayRecord(const People& APeople);
void SaveRecord(ofstream& outfile,const People& APeople);
Boolean ReadRecord(ifstream& infile,People& APeople);

Boolean InsertKey(char[]);
Boolean GetData(char[]);

int compare(const char* s1,const char* s2);
void Visit(People &APeople);

Boolean over_ride(char[]);
Boolean get_output_filename(char target[]);
Boolean get_input_filename(char source[]);

Boolean agree();
Boolean insert();

//Defenition Part

void kleenbuff()
 {
	while (cin.get()!='\n')
	 {
	 }
 }

Boolean GetRecord(People& APeople)
 {
	Name AName;
	Date Birthday;
	PhoneNumber APhone;
	Address AAddress;
	int x=2,y=3,left=2,line=1;
	int x1=23,x2=58,y1=10,y2=19;
	char Answer='Y';

	sub_window(x1,y1,x2,y2,GREEN);

	gotoxy(left,line);
	cout<<"Last name  : ";
	if (!GetData(AName.LastName)) return False;
	gotoxy(x,y);
	clreol();
	cout<<"Name     : "<<AName.LastName;
	x=wherex()+1;
	gotoxy(left,line);
	clreol();
	cout<<"First name : ";
	if (!GetData(AName.FirstName)) return False;
	gotoxy(x,y);
	cout<<", "<<AName.FirstName;
	x=wherex()+1;
	gotoxy(left,line);
	clreol();
	cout<<"Middle name: ";
	if (!GetData(AName.MidName)) return False;
	gotoxy(x,y);
	cout<<" "<<AName.MidName;
	x=left;
	y++;
	gotoxy(left,line);
	clreol();
	cout<<"Birthday? Y/N";
	Answer=getch();
	if (toupper(Answer)=='Y')
	 {
		char temp[5];
		gotoxy(left,line);
		clreol();
		cout<<"Month: ";
		if (!GetData(temp)) return False;
		Birthday.Month=atoi(temp);
		gotoxy(x,y);
		cout<<"Birthday : "<<Birthday.Month;
		x=wherex();
		gotoxy(left,line);
		clreol();
		cout<<"Day  : ";
		if (!GetData(temp)) return False;
		Birthday.Day=atoi(temp);
		gotoxy(x,y);
		cout<<"-"<<Birthday.Day;
		x=wherex();
		gotoxy(left,line);
		clreol();
		cout<<"Year : ";
		if (!GetData(temp)) return False;
		Birthday.Year=atoi(temp);
		gotoxy(x,y);
		cout<<"-"<<Birthday.Year;
		x=left;
		y++;
	 }
	else
	 {
		gotoxy(x,y);
		cout<<"Birthday : NA";
		x=left;
		y++;
	 }
	gotoxy(left,line);
	clreol();
	cout<<"Phone Number? Y/N";
	Answer=getch();
	if (toupper(Answer)=='Y')
	 {
		gotoxy(left,line);
		clreol();
		cout<<"Area Code: ";
		if (!GetData(APhone.AreaCode)) return False;
		gotoxy(x,y);
		cout<<"Telephone: ("<<APhone.AreaCode<<") ";
		x=wherex();
		gotoxy(left,line);
		clreol();
		cout<<"Local number: ";
		if (!GetData(APhone.LocalNumber)) return False;
		if (strlen(APhone.LocalNumber)==7)
		 {
			for (int index=8;index>=3;index--)
			 {
				APhone.LocalNumber[index+1]=APhone.LocalNumber[index];
			 }
			APhone.LocalNumber[index+1]='-';
		 }
		gotoxy(x,y);
		cout<<APhone.LocalNumber;
		x=left;
		y++;
	 }
	else
	 {
		gotoxy(x,y);
		cout<<"Telephone: NA";
		x=left;
		y++;
	 }
	gotoxy(left,line);
	clreol();
	cout<<"Address 1: ";
	if (!GetData(AAddress.Address1)) return False;
	gotoxy(x,y);
	cout<<"Address  : ";
	x=wherex();
	gotoxy(x,y);
	cout<<AAddress.Address1;
	y++;
	gotoxy(left,line);
	clreol();
	cout<<"Address 2: ";
	if (!GetData(AAddress.Address2)) return False;
	if (AAddress.Address2[0]!=NULL)
	 {
		gotoxy(x,y);
		cout<<AAddress.Address2;
		y++;
	 }
	gotoxy(left,line);
	clreol();
	cout<<"City     : ";
	if (!GetData(AAddress.City)) return False;
	gotoxy(x,y);
	cout<<AAddress.City;
	gotoxy(left,line);
	clreol();
	cout<<"State    : ";
	if (!GetData(AAddress.State)) return False;
	gotoxy(x,y);
	if (AAddress.State[0]!=NULL)
		cout<<AAddress.City<<", "<<AAddress.State;
	y++;
	gotoxy(left,line);
	clreol();
	cout<<"Zip Code : ";
	if (!GetData(AAddress.ZipCode)) return False;
	gotoxy(x,y);
	cout<<AAddress.ZipCode;
	APeople.Input(AName,Birthday,APhone,AAddress);
	return True;
 }

void DisplayRecord(const People& APeople)
 {
	Name AName;
	Date Birthday;
	PhoneNumber APhone;
	Address AAddress;
	APeople.Get(AName,Birthday,APhone,AAddress);

	int x1=23,x2=58,y1=10,y2=18;
	int left=2;
	sub_window(x1,y1,x2,y2,GREEN);
	gotoxy(left,2);
	cout<<"Name     : "<<AName.LastName<<", "<<AName.FirstName<<" "<<AName.MidName;
	gotoxy(left,wherey()+1);
	if (Birthday.Month!=0)
	 {
		cout<<"Birthday : "<<Birthday.Month<<"-"<<Birthday.Day<<"-"<<Birthday.Year;
		gotoxy(left,wherey()+1);
	 }
	else
	 {
		cout<<"Birthday : NA";
		gotoxy(left,wherey()+1);
	 }
	if (strlen(APhone.AreaCode)!=0)
		cout<<"Telephone: ("<<APhone.AreaCode<<") "<<APhone.LocalNumber;
	else
		cout<<"Telephone: NA";
	gotoxy(left,wherey()+1);
	cout<<"Address  : ";
	int x=wherex();
	gotoxy(x,wherey());
	cout<<AAddress.Address1;
	gotoxy(x,wherey()+1);
	if (strlen(AAddress.Address2)!=0)
	 {
		cout<<AAddress.Address2;
		gotoxy(x,wherey()+1);
	 }
	cout<<AAddress.City<<", "<<AAddress.State;
	gotoxy(x,wherey()+1);
	cout<<AAddress.ZipCode;
 }

void SaveRecord(ofstream& outfile,const People& APeople)
 {
	Name AName;
	Date Birthday;
	PhoneNumber APhone;
	Address AAddress;
	APeople.Get(AName,Birthday,APhone,AAddress);

	outfile<<AName.LastName<<'\t'<<AName.FirstName<<'\t'<<AName.MidName<<'\t';
	outfile<<Birthday.Month<<'/'<<Birthday.Day<<'/'<<Birthday.Year<<'\t';
	outfile<<APhone.AreaCode<<'-'<<APhone.LocalNumber<<'\t';
	outfile<<AAddress.Address1<<'\t'<<AAddress.Address2<<'\t';
	outfile<<AAddress.City<<'\t'<<AAddress.State<<'\t';
	outfile<<AAddress.ZipCode<<endl;
	return;
 }

//Comment all the records are linear and also seperated by tab character

Boolean ReadRecord(ifstream& infile,People& APeople)
 {
	Name AName;
	Date Birthday;
	PhoneNumber APhone;
	Address AAddress;

	infile.getline(AName.LastName,Max_Size,'\t');
	infile.getline(AName.FirstName,Max_Size,'\t');
	infile.getline(AName.MidName,Max_Size,'\t');
	infile>>Birthday.Month;
	infile.get();
	infile>>Birthday.Day;
	infile.get();
	infile>>Birthday.Year;
	infile.get();
	infile.getline(APhone.AreaCode,5,'-');
	infile.getline(APhone.LocalNumber,10,'\t');
	infile.getline(AAddress.Address1,Max_Size,'\t');
	infile.getline(AAddress.Address2,Max_Size,'\t');
	infile.getline(AAddress.City,30,'\t');
	infile.getline(AAddress.State,30,'\t');
	infile.getline(AAddress.ZipCode,10);
	APeople.Input(AName,Birthday,APhone,AAddress);
	return True;
 }

int compare(const char* s1,const char* s2)
 {
	int length;
	Boolean flag=True;

	 if (strlen(s1)<=strlen(s2))
		length=strlen(s1);
	 else
		length=strlen(s2);
	 for (int i=0;i<length&&flag==True;i++)
	  {
		if (toupper(*(s1+i))!=toupper(*(s2+i)))
			flag=False;
	  }
	 if (flag==True)
		return 0;
	 else return ( (int) toupper(*(s1+i-1))-(int) toupper(*(s2+i-1)) );
 }

void Visit(People &APeople)
 {
	DisplayRecord(APeople);
	cin.get();
 }

Boolean InsertKey(char key[])
 {
	int x1=23,x2=58,y1=12,y2=13;
	sub_window(x1,y1,x2,y2,LIGHTGRAY);
	textcolor(BLACK);
	gotoxy(2,1);
	cprintf("Enter the person's last name:");
	gotoxy(2,2);
	if (GetData(key)&&key[0]!=NULL)
	 {
		return True;
	 }
	else
		return False;
 }

Boolean GetData(char string[])
 {
	char temp;
	int i=0;
	temp=getch();
	while (temp!=ESC&&temp!='\r'&&temp!='\t')
	 {
		if (temp==0)
		 {
			switch (getch())
			 {
			 }
		 }
		else
		 {
			if (temp==8)
			 {
				if (i>0)
				{
					i--;
					gotoxy(wherex()-1,wherey());
					cout<<' ';
					gotoxy(wherex()-1,wherey());
				}
			 }
			else
			 {
				string[i]=temp;
				i++;
				cout<<temp;
			 }
		 }
		temp=getch();
	 }
	string[i]='\0';
	if (temp==ESC)
	 {
		string[0]=NULL;
		return False;
	 }
	else
	 {
		return True;
	 }
 }

Boolean over_ride(char source[])
 {
	char answer='Y';
	int x1=28,x2=52,y1=11,y2=15;
	sub_window(x1,y1,x2,y2,RED);
	textcolor(YELLOW);
	gotoxy(2,2);
	cprintf("Output file: ");
	gotoxy(2,3);
	cout<<source;
	gotoxy(2,4);
	cprintf("Agree? Y/N ");
	answer=getch();
	answer=toupper(answer);
	if (answer=='Y')
		return True;
	else
		return False;
 }

Boolean get_output_filename(char target[])
 {
	char answer='Y';
	int x1=24,x2=56,y1=10,y2=17;
	while (toupper(answer)=='Y')
	 {
		sub_window(x1,y1,x2,y2,GREEN);
		textcolor(YELLOW);
		gotoxy(3,2);
		cprintf("Output file: ");
		gotoxy(3,3);
		if (GetData(target)&&target[0]!=NULL)
		 {
			if (over_ride(target))
			 {
				answer='N';
			 }
			else
				answer='Y';
		 }
		else
			return False;
	 }
	return True;
 }

Boolean get_input_filename(char source[])
 {
	int x1=24,x2=56,y1=13,y2=15;
	sub_window(x1,y1,x2,y2,GREEN);
	textcolor(YELLOW);
	gotoxy(2,1);
	cprintf("Input file: ");
	gotoxy(2,2);
	if (GetData(source)&&source[0]!=NULL)
		return True;
	else
		return False;
 }

Boolean agree()
 {
	char answer='Y';
	int x1=28,x2=52,y1=9,y2=9;
	sub_window(x1,y1,x2,y2,RED);
	textcolor(YELLOW);
	gotoxy(2,1);
	cprintf("Delete this record? Y/N");
	answer=getch();
	answer=toupper(answer);
	if (answer=='Y')
		return True;
	else
		return False;
 }

Boolean insert()
 {
	char answer='Y';
	int x1=27,x2=51,y1=7,y2=7;
	sub_window(x1,y1,x2,y2,RED);
	textcolor(YELLOW);
	gotoxy(2,1);
	cprintf("Insert this record? Y/N");
	answer=getch();
	answer=toupper(answer);
	if (answer=='Y')
		return True;
	else
		return False;
 }

#endif