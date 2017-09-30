//HOANG LUU
//MAT 3633 RESEARCH PROJECT

#include <iostream.h>
#include <constrea.h>
#include <fstream.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define elements 1000

struct couple
 {
  double x[elements];
  double y[elements];
 };
//Global Variables
double dt;
int type_of_f;

double f_of_y(int,double);
double G(double,double);
void initialize(couple&);

int main()
 {
  couple store;
  double z[elements];
  ofstream output;
  double tout;
  char filename[81];
  char answer;
  int index,method;
  do
   {
    clrscr();
    cout<<"Welcome to differential equation analysis program!"<<endl<<endl;
    cout<<"   Type of equation"<<endl;
    cout<<"1. y'(t)=-y(t)"<<endl;
    cout<<"2. y'(t)=1+[y(t)]ý"<<endl;
    cout<<"3. y'(t)=y(t)-5z(t)"<<endl;
    cout<<"   z'(t)=y(t)-3z(t)"<<endl;
    cout<<"4. y'(t)=y(t)-0.5y(t)*z(t)"<<endl;
    cout<<"   z'(t)=-y(t)+y(t)*z(t)"<<endl;
    cout<<"   Please input your choice: ";
    cin>>type_of_f;
    cout<<endl<<"Input initial value: "<<endl;
    cout<<"y(0) = ";
    cin>>store.y[0];
    if (type_of_f>=3)
     {
      cout<<"z(0) = ";
      cin>>z[0];
     }
    cout<<"Input value for limiting range: "<<endl;
    cout<<"0 ó t ó ";
    cin>>tout;
    cout<<"Input value for step size: "<<endl;
    cout<<"dt = ";
    cin>>dt;
    cin.get();
    if (type_of_f<3)
     {
      cout<<"   Method"<<endl;
      cout<<"1. Forward Euler"<<endl;
      cout<<"2. Backward Euler"<<endl;
      cout<<"   Please input method: ";
      cin>>method;
      cin.get();
     }
    cout<<"Intput file name to store data: ";
    cin.getline(filename,80);
    output.open(filename);
    while (output.fail())
     {
      cout<<"Error when creating output file!"<<endl;
      exit (1);
     }
    initialize(store);
    index=1;
    store.x[0]=0;
    if (store.y[0]==0)
     {
      store.x[1]=dt;
      store.y[1]=dt;
      z[1]=dt;
      index=index+1;
     }
    switch (type_of_f)
     {
      case 1:
      case 2:
       if (method==1) //FORWARD EULER
	{
	 for (;store.x[index-1]<tout&&index<elements;index++)
	  {
	   store.x[index]=store.x[index-1]+dt;
	   store.y[index]=store.y[index-1]+dt*f_of_y(type_of_f,store.y[index-1]);
	  }
	}
       else          //BACKWARD EULER
	{
	 for (;store.x[index-1]<tout&&index<elements;index++)
	  {
	   double temp=store.y[index-1];
	   do
	    {
	     temp=G(store.y[index-1],temp);
	    }
	   while (fabs(temp-G(store.y[index-1],temp))>dt);
	   store.x[index]=store.x[index-1]+dt;
	   store.y[index]=temp;
	  }
	}
       break;
      case 3:
       for (;store.x[index-1]<tout&&index<elements;index++)
	{
	 store.x[index]=store.x[index-1]+dt;
	 store.y[index]=(dt*(store.y[index-1]-5*z[index-1]))+store.y[index-1];
	 z[index]=(dt*(store.y[index-1]-3*z[index-1]))+z[index-1];
	}
       break;
      case 4:
       for (;store.x[index-1]<tout&&index<elements;index++)
	{
	 store.x[index]=store.x[index-1]+dt;
	 store.y[index]=(dt*(store.y[index-1]-0.5*z[index-1]))+store.y[index-1];
	 z[index]=(dt*(store.y[index-1]*z[index-1]-z[index-1]))+z[index-1];
	}
       break;
     }
    output<<"DATA FILE"<<endl;
    for (index=0;index<elements&&store.x[index]!=-1;index++)
     {
      output<<store.x[index]<<'\t'<<store.y[index];
      if (type_of_f>=3)
       output<<'\t'<<z[index]<<endl;
      else output<<endl;
     }
    cout<<"Data has been saved in "<<filename<<endl;
    output.close();
    cout<<"Do you want process one more time? Y/N ";
    cin>>answer;
   }
  while (toupper(answer)=='Y');
  return 0;
 }

//+++Definition for sub_functions+++

void initialize(couple&store)
 {
  int i=0;
  for (;i<elements;i++)
   {
    store.x[i]=-1;
   }
  return;
 }

double f_of_y(int type_of_f,double value)
 {
  if (type_of_f==1)
   return (-value);
  else
   return (1+value*value);
 }

double G(double y_old,double temp)
 {
  return(y_old+dt*f_of_y(type_of_f,temp));
 }
