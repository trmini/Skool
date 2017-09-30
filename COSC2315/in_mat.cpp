#include "a:\matrix.h"
#include <iostream.h>

template<class T>
Matrix<T>::Matrix(int rdim, int cdim)
 {
	row_dim=rdim;
	col_dim=cdim;
	a=new T* [row_dim];
	for (int i=0;i<row_dim;i++)
		a[i]=new T[col_dim];
 }

template<class T>
Matrix<T>::Matrix(int rdim, int cdim, T*matrixPtr)
 {
	row_dim=rdim;
	col_dim=cdim;
	a=new T* [row_dim];
	for (int i=0;i<row_dim;i++)
		a[i]=new T[col_dim];

	for (i=0;i<row_dim;i++)
		for (int j=0;j<col_dim;j++)
			a[i][j]=*(matrixPtr + (col_dim*i+j));
 }

template<class T>
Matrix<T>::Matrix(const Matrix<T>&m)
 {
	row_dim=m.row_dim;
	col_dim=m.col_dim;
	a=new T*[row_dim];
	for (int i=0;i<row_dim;i++)
		a[i]=new T[col_dim];

	for (i=0;i<row_dim;i++)
		for (int j=0;j<col_dim;j++)
			a[i][j]=m.a[i][j];
 }

template<class T>
Matrix<T>::~Matrix()
 {
	for(int i=0;i<row_dim;i++)
		delete [] a[i];
	delete []a;
 }

template<class T>
ostream&operator<<(ostream&os, const Matrix<T>&m)
 {
	for(int i=0;i<m.row_dim;i++)
	 {
		for(int j=0;j<m.col_dim;j++)
		 {
			os<<"a["<<i<<","<<j<<"]=";
			os<<m.a[i][j]<<endl;
		 }
	 }
	return os;
 }

template<class T>
Matrix<T>&operator=(const Matrix<T>&m)
 {
	Matrix<T>*result=new Matrix<T>(m.row_dim,m.col_dim);
	for (int i=0;i<m.row_dim;i++)
		for(int j=0;j<m.col_dim;j++)
			result->a[i][j]=m.a[i][j];
	return *result;
 }

template<class T>
Matrix<T>&operator+(const Matrix<T>&m1,const Matrix<T>&m2)
 {
	//Error checking
	if (m1.row_dim!=m2.row_dim||m1.col_dim!=m2.col_dim)
	 {
		cout<<endl<<"Size mismatch!"<<endl;
		return *NULL;
	 }
	Matrix<T>*result=new Matrix<T>(m1.row_dim,m1.col_dim);
	for (int i=0;i<m1.row_dim;i++)
		for(int j=0;j<m1.col_dim;j++)
			result->a[i][j]=m1.a[i][j]+m2.a[i][j];
	return *result;
 }

template<class T>
Matrix<T>&operator*(const Matrix<T>&m1,const Matrix<T>&m2)
 {
	//Error checking
	if (m1.col_dim!=m2.row_dim)
	 {
		cout<<endl<<"Size mismatch!"<<endl;
		return *NULL;
	 }
	Matrix<T>*result=new Matrix<T>(m1.row_dim,m2.col_dim);
	for (int i=0;i<m1.row_dim;i++)
		for(int j=0;j<m2.col_dim;j++)
		 {
			T element_result;
			element_result=m1.a[i][0]*m2.a[0][j];
			for (int count=1;count<m1.col_dim;count++)
				element_result+=m1.a[i][count]*m2.a[count][j];
			result->a[i][j]=element_result;
		 }
	return *result;
 }

template<class T>
Matrix<T>&operator*(const T&scaler,const Matrix<T>&m)
 {
	Matrix<T>*result=new Matrix<T>(m.row_mid,m.col_mid);
	for (int i=0;i<m.row_dim;i++)
		for(int j=0;j<m.col_dim;j++)
		 {
			result->a[i][j]=scaler*m.a[i][j];
		 }
	return *result;
 }

/*template<class T>
Matrix<T>&Identity(const int iden_size)
 {
	Matrix<T>*result=new Matrix<T>(iden_size,iden_size);
	for (int i=0;i<iden_size;i++)
		for (int j=0;j<iden_size;j++)
			if (i==j)
				result->a[i][j]=1;
			 else
				result->a[i][j]=0;
	return *result;
 }*/

template<class T>
Matrix<T>&Transpose(const Matrix<T>&m)
 {
	Matrix<T>*result=new Matrix<T>(m.col_dim,m.row_dim);
	for (int i=0;i<m.row_dim;i++)
		for(int j=0;j<m.col_dim;j++)
			result->a[i][j]=m.a[j][i];
	return *result;
 }

/*template<class T>
Matrix<T> *Inverse(const Matrix<T>&m)
 {
	if (m.row_dim!=m.col_dim) return NULL;
	Matrix<T>*result=new Matrix<T>(m.row_dim,m.col_dim);
	Matrix<T>*temp=Append(m,Identity(m.row_dim);
	if (RowReduce(temp))
	 {
		for (int i=0;i<m.row_dim;i++)
			for(int j=0;j<m.col_dim;j++)
				result->a[i][j]=temp.a[m.row_dim+i][m.col_dim+i];
		return result;
	 }
	else
	 {
	  cout<<endl<<"Non Invertible!"<<endl;
	  return NULL;
	 }
	//Wait for Row Reduce
 }*/
