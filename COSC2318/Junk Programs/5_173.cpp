// Exercise 5 page 173

#include <iostream.h>
#include <constrea.h>

int main()
 {
  float score=0;
  int aS=0,bS=0,cS=0,dS=0,fS=0;
  clrscr();
  cout<<"To stop and see the result, input a negative number fore score!";
  cout<<endl<<endl;
  while (score>=0)
   {
    cout<<"Score? ";
    cin>>score;
    if (score>100)
     {
      cout<<"Is this too high? Please re-enter another score!"<<endl;
     }
    else if (score>=90)
	  {
	   cout<<" The grade is A"<<endl;
	   aS++;
	  }
    else if (score>=80)
	  {
	   cout<<" The grade is B"<<endl;
	   bS++;
	  }
    else if (score>=70)
	  {
	    cout<<" The grade is C"<<endl;
	    cS++;
	  }
    else if (score>=60)
	  {
	    cout<<" The grade is D"<<endl;
	    dS++;
	  }
    else if (score>=0)
	  {
	    cout<<" The grade is F"<<endl;
	    fS++;
	  }
   }
  clrscr();
  cout<<endl<<"Result!"<<endl<<endl;
  cout<<aS<<"  A'S"<<endl;
  cout<<bS<<"  B'S"<<endl;
  cout<<cS<<"  C'S"<<endl;
  cout<<dS<<"  D'S"<<endl;
  cout<<fS<<"  F'S"<<endl;
  cin.get();
  cin.get();
  clrscr();
  return 0;
 }