#include <iostream.h>
#include <constrea.h>
#include "c:\mydocu~1\cosc2315\matrix.h"

int main()
 {
	int M[4][4];
	for (int i=0;i<4;i++)
	 for (int j=0;j<4;j++)
	  M[i][j]=i+j;

	Matrix<int> A(4,4,M[0]);
	Matrix<float> B(4,4);
	Matrix<int> C(A);
	cout<<A.Row();
	cout<<B.Col();
	gotoxy(10,10);
	cout<<C;
	cin.get();
	A.Fill(3);
//	C=Trn(A);
	clrscr();
	cout<<endl;
	A.Output();
	if (C!=A)
	 {
	A.Zero();
	cout<<A<<endl;
	Iden(A);
	cout<<A;
	cout<<endl<<endl;
	C.Output();
	C.Iden();
	C.Insert(2,3,12);
	cout<<C;
	 }
	return 0;
 }