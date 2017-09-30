#include <iostream.h>
#include <conio.h>
#include <fstream.h>
#include <ctype.h>
#include <stdlib.h>

const int max_size=10;

void display(int Matrix[][max_size],int size);
void print(ofstream& outfile,int Matrix[][max_size],int size);
void calculate(int result[][max_size],int original[][max_size],int size);

int main()
 {
	int A1[max_size][max_size];
	int An[max_size][max_size];
	int size;
	ofstream outfile;
	char filename[80];
	int i,j;
	int path;
	char Answer='Y';
	while (toupper(Answer)=='Y')
	 {
		clrscr();
		cout<<"What will be the size of network: (less than "<<max_size<<") : ";
		cin>>size;
		cin.get();
		cout<<"Enter the name of output file with full direction: ";
		cin.getline(filename,80);
		outfile.open(filename);
		if (outfile.fail())
		 {
			cout<<"Error when opening file!"<<endl;
			cout<<"Check your disk and reload the program"<<endl;
			exit(1);
		 }
		cout<<"Input data for the actual network (A1)"<<endl;
		for (i=0;i<size;i++)
		 for (j=0;j<size;j++)
			{
				gotoxy(1+8*j,6+i);
				cout<<"("<<i+1<<","<<j+1<<")=";
				cin>>A1[i][j];
				cin.get();
				An[i][j]=A1[i][j];
			}
		cout<<endl;
		outfile<<"Original Martrix:"<<endl;
		print(outfile,A1,size);
		while (toupper(Answer)=='Y')
		 {
			cout<<"Enter the length of path desired: ";
			cin>>path;
			cin.get();
			int count=2;
			while (path>1&&count<=path)
			 {
				calculate(An,A1,size);
				count++;
			 }
			clrscr();
			cout<<"Original Matrix:"<<endl;
			display(A1,size);
			cout<<"Result Matrix with the path of "<<path<<":"<<endl;
			display(An,size);
			outfile<<"Result Matrix with the path of "<<path<<":"<<endl;
			print(outfile,An,size);
			cout<<"Do you want to try another path? Y/N ";
			cin>>Answer;
			cin.get();
		 }
		cout<<"Do you want to run one more time? Y/N ";
		cin>>Answer;
		cin.get();
	 }
	outfile.close();
	clrscr();
	return 0;
 }

void display(int Matrix[][max_size],int size)
 {
	for (int i=0;i<size;i++)
	 {
		for (int j=0;j<size;j++)
			{
				cout<<Matrix[i][j]<<"   ";
			}
		cout<<endl;
	 }
 }

void print(ofstream& outfile,int Matrix[][max_size],int size)
 {
	for (int i=0;i<size;i++)
	 {
		for (int j=0;j<size;j++)
			{
				outfile<<Matrix[i][j]<<"   ";
			}
		outfile<<endl;
	 }

 }

void calculate(int result[][max_size],int original[][max_size],int size)
 {
	int temp[max_size][max_size];
	for (int i=0;i<size;i++)
		for (int j=0;j<size;j++)
		  {
			temp[i][j]=0;
			for (int count=0;count<size;count++)
				temp[i][j]=temp[i][j]+result[i][count]*original[count][j];
		  }
	for (i=0;i<size;i++)
		for (j=0;j<size;j++)
			result[i][j]=temp[i][j];
	return;
 }
