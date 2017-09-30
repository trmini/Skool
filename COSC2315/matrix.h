//Programmer: Trang Luu
//Course    : COSC2315.001
//A Matrix Class

#include <iostream.h>
#include <conio.h>
#include <stdlib.h>
#ifndef MATRIX_H
#define MATRIX_H

template <class T>
class Matrix
 {
	private:
		int row_dim;
		int col_dim;
		T **a;
		void Swap(int row1,int row2);
		//Swap two rows of a Matrix
		//Pre: row1 and row2 are two number indicate rows need to
		//be swapt
		//Post: Two rows are swapt
	public:
		Matrix();
		//Default constructor: Creates default matrix. This matrix has the size
		//0x0. The purpose is it is a variable and can be assigned (initialized)
		//by the = operator.
		//Keyword: Matrix<DefinedType> VariableName;
		//Or: Matrix<DefinedType> VariableName=A;
		//After this initialization VariableName is a copy of A
		//Pre: None
		//Post: A matrix is created
		Matrix(int rdim,int cdim);
		//Constructor: Creats a (row x column) matrix
		//Keyword: Matrix<DefinedType> VariableName(DesiredRow,DesiredCol);
		//Pre: rdim is the desired row, cdim is the desired column
		//Post: A matrix with desried size is created
		Matrix(int rdim,int cdim,const T *ArrayPtr);
		//Constructor: Creats a (row x column) matrix and copy the entries
		//from a two_dimensional array
		//Keyword: Matrix<DefinedType> VariableName(Row,Col,M[0]);
		//Pre: Row, Col is the size of matrix. M[0] is a pointer which
		//point to the first entry of array M[i][j]. Matrix and array must
		//have the same type.
		//Post: A matrix with desried size exists and its entry is a copy
		//of M[i][j]
		Matrix(const Matrix<T> &OriginalMatrix);
		//Constructor: Creat a matrix which is a copy of OriginalMatrix
		//Keyword: Matrix<DefinedType> VariableName(OriginalMatrix);
		//Pre: OriginalMatrix is a matrix which already exists and have
		//the same type with the new matrix
		//Post: A variable which is a copy of OriginalMatrix exists
		~Matrix();
		//Destructor: Automatically delete a matrix if it is out of scope
		//Pre: None
		//Post: Memory which has contained a matrix is released

		//Overloading Operator
		//Precondition: For every operator which works with two matrices
		//such as =,==,!=,+,-, and multiplication between two matrices, both
		//matrices must have the same type.
		Matrix<T> &operator=(const Matrix<T> &Right);
		//Overloading = operator  A=B
		//Pre: As indicated above. A and B should have the same size.
		//Otherwise, no assignment would happen and return A
		//Post: B assigns to A.
		int operator==(const Matrix<T> &) const;
		//Overloading == operator  A==B
		//Pre: As indicated above.
		//Post: return 1 if A equals B. Otherwise, return 0
		int operator!=(const Matrix<T> &) const;
		//Overloading != operator A!=B
		//Pre: As indicated above
		//Post: return 1 if A does not equal B. Otherwise, return 0
		friend ostream& operator<<(ostream& os, const Matrix<T>&);
		//Overloading << operator
		//Pre: None
		//Post: Entire entry is extracted to output stream
		friend Matrix<T>& operator+(const Matrix<T> &,const Matrix<T> &);
		//Overloading + operator (A+B)
		//Pre: As indicated above. A and B should have the same size.
		//Otherwise, abort is called.
		//Post: return a matrix which is the sum of A and B
		friend Matrix<T>& operator-(const Matrix<T> &,const Matrix<T> &);
		//Overloading - operator (A-B)
		//Pre: As indicated above. A and B should have the same size.
		//Otherwise, abort is called
		//Post: return a matrix which is the difference between A and B
		friend Matrix<T>& operator*(const Matrix<T> &,const Matrix<T> &);
		//Overloading * operator (A*B)
		//Pre: As indicated above. As the definition of multiplication
		//between two matrices: the column of A have to be the same as the
		//row of B. If not an abort will be called
		//Post: return a matrix which is the multiplication of A and B
		friend Matrix<T>& operator*(const long double&,const Matrix<T>&);
		//Overloading * operator (k*A)
		//Pre: k is any number
		//Post: return a matrix which is the multiplication of k and A
		friend Matrix<T>& operator*(const Matrix<T> &,const long double&);
		//Overloading * operator (A*k)
		//Pre: k is any number
		//Post: return a matrix which is the multiplication of k and A


		//Member function section
		inline int Row() const;
		//Returns number of row of a matrix A.Row();
		//Pre: None
		//Post: Number of row is returned
		inline int Col() const;
		//Returns number of column of a matrix A.Col();
		//Pre: None
		//Post: Number of column is returned
		Matrix<T>& Fill(T);
		//Fills matrix with a number A.Fill(number);
		//Pre: Desired number and matrix must have the same type
		//Post: Matrix is filled
		Matrix<T>& Zero();
		//Fills matrix with zero (zero matrix) A.Zero();
		//Pre: None
		//Post: A becomes a zero matrix
		Matrix<T>& Iden();
		//Changes A to an indentity matrix A.Iden();
		//Pre: A must be a square matrix
		//Post: A becomes an identity matrix
		void Insert(int,int,const T&);
		//Inserts a number into A at the desried position
		//A.Insert(RowPos,ColPos,Number);
		//Pre: Number and A must have the same type. Position must be
		//in A
		//Post: Desried entry has new value
		int Insert(int,int,const T*);
		//Inserts a an array into a row or column of A
		//A.Insert(Row,Col,M[0]);
		//Pre: Array and matrix must have the same type
		//Row or Col must be zero
		//If Row is 0, column Col will be filled by M
		//If Col is 0, row Row will be filled by M
		//Besure that the size of M  must sastify either number of
		//row or column
		//Post: If success, column n or row m of matrix is filled and
		//finction returns 1. If not, returns 0 and A will not change
		void Output() const;
		//Output entries of matrix
		//A.Output();
		//Pre: None
		//Post: Entries will be displayed

		//Friend function section
		//Provide another version for member functions
		friend Matrix<T>& Fill(Matrix<T>&,T);
		friend Matrix<T>& Zero(Matrix<T>&);
		friend Matrix<T>& Iden(Matrix<T>&);

		friend Matrix<T>& Trn(const Matrix<T>&);
		//Returns a matrix which is a transposition of A
		// Trn(A);
		//Pre: None
		//Post: A transopsiton of A is returned
 };

template <class T>
inline int Matrix<T>::Row() const
 { return row_dim; }

template <class T>
inline int Matrix<T>::Col() const
 { return col_dim; }
#endif


//Implementation Part
//Private Section
template <class T>
void Matrix<T>::Swap(int row1,int row2)
 {
	T Temp;
	for (int index=0;index<col_dim;index++)
	 {
		Temp=a[row1][index];
		a[row1][index]=a[row2][index];
		a[row2][index]=Temp;
	 }
	return;
 }

template <class T>
Matrix<T>::Matrix()
 {
	row_dim=col_dim=0;
	a=0;
 }

template <class T>
Matrix<T>::Matrix(int rdim,int cdim)
 {
	row_dim=rdim;
	col_dim=cdim;
	a=new T*[row_dim];
	for (int i=0;i<row_dim;i++)
		a[i]=new T[col_dim];
 }

template <class T>
Matrix<T>::Matrix(int rdim,int cdim, const T *ArrayPtr)
 {
	row_dim=rdim;
	col_dim=cdim;
	a=new T*[row_dim];
	for (int i=0;i<row_dim;i++)
		a[i]=new T[col_dim];

	for (i=0;i<row_dim;i++)
		for (int j=0;j<col_dim;j++)
			a[i][j]=*(ArrayPtr + col_dim*i + j);
 }

template <class T>
Matrix<T>::Matrix(const Matrix<T> &OriginalMatrix)
 {
	row_dim=OriginalMatrix.row_dim;
	col_dim=OriginalMatrix.col_dim;
	a=new T*[row_dim];

	for (int i=0;i<row_dim;i++)
		a[i]=new T[col_dim];

	for (i=0;i<row_dim;i++)
		for (int j=0;j<col_dim;j++)
			a[i][j]=OriginalMatrix.a[i][j];
 }

template <class T>
Matrix<T>::~Matrix()
 {
	for(int i=0;i<row_dim;i++)
		delete [] a[i];
	delete []a;
 }

//Overloading Operator
template <class T>
Matrix<T> &Matrix<T>::operator=(const Matrix<T> &Right)
 {
	if (row_dim==Right.row_dim&&col_dim==Right.col_dim)
	 {
		if (&Right!=this)
		 {
			for (int i=0;i<Right.row_dim;i++)
				for(int j=0;j<Right.col_dim;j++)
					a[i][j]=Right.a[i][j];
		 }
		return *this;
	 }
	else if (row_dim==0&&col_dim==0)
		  {
			row_dim=Right.row_dim;
			col_dim=Right.col_dim;
			delete a;
			a=new T*[row_dim];
				for (int i=0;i<row_dim;i++)
					a[i]=new T[col_dim];

			for (i=0;i<row_dim;i++)
				for (int j=0;j<col_dim;j++)
					a[i][j]=Right.a[i][j];
		  }
	return *this;
 }

template<class T>
ostream &operator<<(ostream&os, const Matrix<T> &m)
 {
	int x=wherex(), y=wherey();
	for(int i=0;i<m.row_dim;i++)
	 {
		gotoxy(x,y+i);
		for(int j=0;j<m.col_dim;j++)
		 {
			os<<" "<<m.a[i][j];
		 }
	 }
	return os;
 }

template<class T>
Matrix<T> &operator+(const Matrix<T>& m1,const Matrix<T>& m2)
 {

	if (m1.row_dim!=m2.row_dim||m1.col_dim!=m2.col_dim)
	 {
		cout<<endl<<"Matrices must have the same size!"<<endl;
		abort();
	 }
	Matrix<T> *result=new Matrix<T>(m1.row_dim,m1.col_dim);
	for (int i=0;i<m1.row_dim;i++)
		for(int j=0;j<m1.col_dim;j++)
			result->a[i][j]=(m1.a[i][j]+m2.a[i][j]);
	return *result;
 }

template<class T>
Matrix<T> &operator-(const Matrix<T>& m1,const Matrix<T>& m2)
 {

	if (m1.row_dim!=m2.row_dim||m1.col_dim!=m2.col_dim)
	 {
		cout<<endl<<"Matrices must have the same size!"<<endl;
		abort();
	 }
	Matrix<T> *result=new Matrix<T>(m1.row_dim,m1.col_dim);
	for (int i=0;i<m1.row_dim;i++)
		for(int j=0;j<m1.col_dim;j++)
			result->a[i][j]=(m1.a[i][j]-m2.a[i][j]);
	return *result;
 }

template<class T>
Matrix<T> &operator*(const Matrix<T>& m1,const Matrix<T>& m2)
 {
	if (m1.col_dim!=m2.row_dim)
	 {
		cout<<endl<<"Size Mismatch!"<<endl;
		abort();
	 }
	Matrix<T> *result=new Matrix<T>(m1.row_dim,m2.col_dim);
	for (int i=0;i<m1.row_dim;i++)
		for(int j=0;j<m2.col_dim;j++)
		 {
			T entry_result;
			entry_result=m1.a[i][0]*m2.a[0][j];
			for (int count=1;count<m1.col_dim;count++)
				entry_result=entry_result+m1.a[i][count]*m2.a[count][j];
			result->a[i][j]=entry_result;
		 }
	return *result;
 }

template<class T>
Matrix<T> &operator*(const long double &scaler,const Matrix<T> &m)
 {
	Matrix<T> *result=new Matrix<T>(m.row_dim,m.col_dim);
	for (int i=0;i<m.row_dim;i++)
		for(int j=0;j<m.col_dim;j++)
		 {
			result->a[i][j]=scaler*m.a[i][j];
		 }
	return *result;
 }

template<class T>
Matrix<T> &operator*(const Matrix<T> &m,const long double &scaler)
 {
	Matrix<T> *result=new Matrix<T>(m.row_dim,m.col_dim);
	for (int i=0;i<m.row_dim;i++)
		for(int j=0;j<m.col_dim;j++)
		 {
			result->a[i][j]=scaler*m.a[i][j];
		 }
	return *result;
 }

template<class T>
int Matrix<T>::operator==(const Matrix<T> &Right) const
 {
	if ((row_dim!=Right.row_dim)||(col_dim!=Right.col_dim))
	 {
		return 0;
	 }
	for (int i=0;i<row_dim;i++)
		for (int j=0;j<col_dim;j++)
		 {
			if (a[i][j]!=Right.a[i][j])
				{ return 0; }
		 }
	return 1;
 }

template<class T>
int Matrix<T>::operator!=(const Matrix<T> &Right) const
 {
	if (row_dim!=Right.row_dim||col_dim!=Right.col_dim)
	 {
		return 1;
	 }
	for (int i=0;i<row_dim;i++)
		for (int j=0;j<col_dim;j++)
		 {
			if (a[i][j]!=Right.a[i][j])
				{ return 1; }
		 }
	return 0;
 }


//Member function section
template <class T>
Matrix<T> &Matrix<T>::Fill(T number)
 {
	for (int i=0;i<row_dim;i++)
	 {
		for (int j=0;j<col_dim;j++)
			a[i][j]=number;
	 }
	return *this;
 }

template <class T>
Matrix<T> &Matrix<T>::Zero()
 {
	Fill(0);
	return *this;
 }

template <class T>
Matrix<T> &Matrix<T>::Iden()
 {
	Zero();
	for (int index=0;index<row_dim;index++)
	 {
		a[index][index]=1;
	 }
	return *this;
 }

template <class T>
void Matrix<T>::Insert(int rpos,int cpos,const T &number)
 {
	a[rpos][cpos]=number;
	return;
 }

template <class T>
int Matrix<T>::Insert(int row,int column,const T *ArrayPtr)
 {
	if (row==0&&column<col_dim)
	 {
		for (int index=0; index<row_dim; index++)
			a[index][column]=*(ArrayPtr+index);
		return 1; //Success
	 }
	else
	 {
		if (column==0&&row<row_dim)
		 {
			for (int index=0; index<col_dim; index++)
				a[row][index]=*(ArrayPtr+index);
			return 1;  //Success
		 }
		else
		 return 0;        //Fail
	 }
 }

template <class T>
void Matrix<T>::Output() const
 {
	int x=wherex(),y=wherey();
	for (int i=0;i<row_dim;i++)
	 {
		gotoxy(x,y+i);
		for (int j=0;j<col_dim;j++)
			{ cout<<" "<<setw(5)<<a[i][j]; }
	 }
	return;
 }

//Friend function section

template <class T>
Matrix<T>& Trn(const Matrix<T>&m)
 {
	Matrix<T>*result=new Matrix<T>(m.col_dim,m.row_dim);
	for (int i=0;i<m.row_dim;i++)
		for(int j=0;j<m.col_dim;j++)
			result->a[i][j]=m.a[j][i];
	return *result;
 }

template <class T>
Matrix<T>& Fill(Matrix<T> &m,T number)
 {
	return m.Fill(number);
 }

template <class T>
Matrix<T>& Zero(Matrix<T> &m)
 {
	return m.Zero();
 }

template <class T>
Matrix<T>& Iden(Matrix<T> &m)
 {
	return m.Iden();
 }