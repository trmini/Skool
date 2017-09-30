//Programmer: Trang Luu
//Course	: COSC2315.001
//Program	: Simulation for airport that have one lane
//Date		: Spring 1999
#include "a:\queue.h"
#include <iostream.h>
#include <conio.h>
#include <stdlib.h>
#include <ctype.h>

struct Plane
 {
	unsigned long int TimeIn;
	unsigned long int TimeOut;
 };

//Comment: I do not use function for this program since I don't want to pass
//so many variables each time I need to call a function.
//Suggestion: Use Global variables to hold input data. This program does not
//change this values during processing,so global variables could act like
//user_defined constants.

void newline();

int main()
 {
	unsigned long int TIME=0,count=0;
	char Answer='Y';
	Plane APlane;
	unsigned int LandTime,TakeTime,CrashTime,CountDown;
	unsigned int ATime,DTime; //Average time arrival of planes to land or takeoff.

	cout.setf(ios::fixed);
	cout.setf(ios::showpoint);
	cout.precision(2);

	while (toupper(Answer)=='Y')
	 {
		clrscr();
		Q<Plane> Landing;
		Q<Plane> Takeoff;
		Q<Plane> PLanding;//Temp. Q for the purpose of priority
		unsigned long int crash=0,land=0,takeoff=0;
		unsigned long int WaitLand=0,WaitTakeoff=0;
		Boolean Busy=False,Stop=False,Priority=False;
		//Flags for flyway busy, stop simulation, and priority
		int Percent_Done=0;

		window(1,1,80,25);
		textcolor(WHITE);
		textbackground(RED);
		gotoxy(1,25);
		cprintf("Note: All time values should have minute as their unit and be positive integer!");
		clreol();
		gotoxy(1,1);
		cout<<"INPUT DATA!"<<endl;
		cout<<"Time for landing process: ";
		cin>>LandTime;
		newline();
		cout<<"Time for takeoff process: ";
		cin>>TakeTime;
		newline();
		cout<<"Average time of arrival: ";
		cin>>ATime;
		newline();
		cout<<"Average time of departure: ";
		cin>>DTime;
		newline();
		cout<<"Maximum of time all planes can fly around (0 to randomize this value): ";
		cin>>CrashTime;
		newline();
		if (CrashTime==0)
		 {
			Priority=True;
			cout<<"     Enter the average time that a plane can fly around: ";
			cin>>CrashTime;
			newline();
		 }
		cout<<"The total length of time to be simulated: ";
		cin>>TIME;
		newline();
		cout<<endl<<"Simulating..."<<endl;
		gotoxy(1,wherey()+1);
		for (int j=0;j<=20;j++)
		 cprintf(" ");
		gotoxy(1,wherey()-1);

		//Start simulating
		for (count=0;count<=TIME&&!Stop;count++)
		 {
			//When time still in range
			if (count<=TIME)
			 {
				if (random(2*ATime)==0)
				 {
					if (!Priority)
					 {
						APlane.TimeIn=count;
						APlane.TimeOut=count+CrashTime;
						Landing.EnQ(APlane);
					 }
					//Bonus part.Put arrive planes into Q in order
					else
					 {
						Plane TempPlane;
						APlane.TimeIn=count;
						APlane.TimeOut=count+1+random(2*CrashTime);
						while(Landing.DeQ(TempPlane))
						 {
							if (APlane.TimeOut<=TempPlane.TimeOut)
							 {
								PLanding.EnQ(APlane);
								APlane.TimeIn=0;
							 }
							PLanding.EnQ(TempPlane);
							//Move data to temp. Q which is in order
						 }
						if (APlane.TimeIn!=0)
							PLanding.EnQ(APlane);
							//Check if this record has been inserted into PLanding
							//If not put it at the end
						while (PLanding.DeQ(TempPlane))
						 {
							Landing.EnQ(TempPlane);
							//Copy Q back;
						 }
					 }
				 }
				if (random(2*DTime)==0)
				 {
					APlane.TimeIn=count;
					APlane.TimeOut=0;
					Takeoff.EnQ(APlane);
				 }
				//Just fancy stuff
				if (Percent_Done!=100*count/TIME)
				 {
					Percent_Done=100*count/TIME;
					if (Percent_Done%5==0)
					 {
						gotoxy(1,wherey());
						cout<<Percent_Done<<" % completed.";
						gotoxy(Percent_Done/5,wherey()+1);
						cout<<"ฒฒ";
						gotoxy(1,wherey()-1);
					 }
				 }
			 }
			else
			 {
				if (Landing.Empty()&&Takeoff.Empty())
					Stop=True;
					//After simulation, set flag to True if both Qs become empty
			 }
			//When flyway free
			if (!Busy)
			 {
				while (!Landing.Empty()&&!Busy)
				 {
					Landing.DeQ(APlane);
					if (APlane.TimeOut<count)
					 {
						crash++;
					 }
					else
					 {
						land++;
						APlane.TimeOut=count;
						WaitLand=WaitLand+(count-APlane.TimeIn);
						Busy=True;
						CountDown=LandTime;
					 }
				 }
				if (!Busy&&!Takeoff.Empty())
				 {
					Takeoff.DeQ(APlane);
					APlane.TimeOut=count;
					takeoff++;
					WaitTakeoff=WaitTakeoff+(count-APlane.TimeIn);
					Busy=True;
					CountDown=TakeTime;
				 }
			 }
			else
			 {
				CountDown--;
				if (CountDown==0) Busy=False;
			 }
		 }
		//Display result
		textcolor(LIGHTGRAY);
		textbackground(BLACK);
		clrscr();
		textcolor(WHITE);
		textbackground(RED);
		cprintf(" ษอออออออออออออออออออป \r\n");
		cprintf(" บ SIMULATION RESULT บ \r\n");
		cprintf(" ศอออออออออออออออออออผ \r\n");
		cout<<endl<<"Source"<<endl;
		cout<<"Time for landing process: "<<LandTime<<" minutes"<<endl;
		cout<<"Time for takeoff process: "<<TakeTime<<" minutes"<<endl;
		cout<<"Average time of arrival: "<<ATime<<" minutes"<<endl;
		cout<<"Average time of departure: "<<DTime<<" minutes"<<endl;
		if (Priority)
		 {
			cout<<"Average time that a plane can fly around: "<<CrashTime<<" minutes"<<endl;
		 }
		else
			cout<<"Maximum of time all planes can fly around: "<<CrashTime<<" minutes"<<endl;
		cout<<endl<<"Result"<<endl;
		cout<<"Total time for simulating: ";
		if (TIME/1440>0)
		 {
			cout<<TIME/1440<<" days "<<(TIME%1440)/60<<" hours and "<<(TIME%1440)%60<<" minutes."<<endl;
		 }
		else
		 {
			cout<<TIME/60<<" hours and "<<TIME%60<<" minutes."<<endl;
		 }
		cout<<"Total landing plane: "<<land<<endl;
		cout<<"Total takeoff plane: "<<takeoff<<endl;
		cout<<"Total crash plane: "<<crash<<endl;
		cout<<"Percentage of crash over total arrival: "<<100*(float (crash)/(crash+land))<<" %"<<endl;
		cout<<"Average of time a plane needs to wait for landing: "<<WaitLand/double (land)<<" mimutes"<<endl;
		cout<<"Average of time a plane needs to wait for takeoff: "<<WaitTakeoff/double (takeoff)<<" minutes"<<endl;
		cout<<endl<<"Do you want to simulate one more time? Y/N ";
		cin>>Answer;
		cin.get();
		textcolor(LIGHTGRAY);
		textbackground(BLACK);
	 }
	clrscr();
	cout<<"Have a good day!"<<endl;
	return 0;
 }


//Clear buffer and moves cursor to new line
void newline()
 {
	int left=1;
	cin.get();
	gotoxy(left,wherey());
	return;
 }