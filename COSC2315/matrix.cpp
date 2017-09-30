#include "a:\in_mat.cpp"
#include <constrea.h>
#include <iostream.h>
const int size=3;

int main()
 {
	int M[size][size];
	float M2[size][size];
	for (int i=0; i<size; i++)
		for (int j=0;j<size;j++)
		 {
			M[i][j]=i+j;
			M2[i][j]=i*3.2;
		 }
	Matrix<int>A(3,3,M);
	Matrix<float>B(3,3);
//	Matrix<Matrix<float>> C(3,3,A);
	B=A;
	cout<<A+B<<endl;
//	cout<<C<<endl;
	for(i=1;i<10;i++)
		A=A*A;
	cout<<A;
	return 0;
 }