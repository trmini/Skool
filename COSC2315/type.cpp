#include <iostream.h>
#include <conio.h>
#include <string.h>

#define ESC 27

int main()
 {
	clrscr();
	char temp;
	char string[80];
	int i=0;
	cout<<endl<<"Input data: ";
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
	cout<<endl<<string;
	cin.get();
	return 0;
 }