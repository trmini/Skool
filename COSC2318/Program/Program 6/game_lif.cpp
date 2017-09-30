#include "gamelife.h"

const int generation_max=15;
//If the system will not stablize, the last generation will be 15

int main()
 {
  //Declare variables
  data_array data,next_data;
  board_array board;
  ifstream infile;
  ofstream outfile;
  int count;
  char file_name[81];
  char answer='Y';
  BOOLEAN the_same=FALSE;

  //Main Part
  quick_kleen();
  welcome();
  do
   {
    quick_kleen();
    kleen_array(data);
    kleen_array(board);
    initialize_array(board);
    cout<<"Enter data file name with full direction: ";
    cin.getline(file_name,81);
    infile.open(file_name);
    while (infile.fail())
     {
      cout<<"Cannot open your file!"<<endl;
      cout<<"Re_enter your file name: ";
      cin.getline(file_name,81);
      infile.open(file_name);
     }
    cout<<"Plese enter your file name to store output data!";
    gotoxy(1,wherey()+2);
    cout<<"Note: if your file has existed, the new data will be saved continuously.";
    gotoxy(1,wherey()-1);
    cout<<"Output file: ";
    cin.getline(file_name,81);
    outfile.open(file_name,ios::app);
    while (outfile.fail())
     {
      cout<<"Cannot create output file!"<<endl;
      cout<<"Please check and run again.";
      exit (1);
     }
    cout<<endl<<"Hit enter to start!";
    cin.get();
    quick_kleen();
    fill_background();
    get_data(infile,data);
    next_data=data;
    count=0;
    textcolor(RED);
    textbackground(LIGHTGRAY);
    while (count<=generation_max&&the_same==FALSE)
     {
      assign_array(next_data,board);
      if (count==0)
       {
	clreol();
	gotoxy(17,1);
	cout<<"The original generation - Hit enter to continue";
       }
      else
       {
	gotoxy(1,1);
	clreol();
	gotoxy(19,1);
	cout<<"Generation number "<<count<<" - Hit enter to continue";
       }
      save_data(board,count,outfile);
      display_array(board);
      getch();
      data=next_data;
      kleen_array(next_data);
      assign_array(next_data,board); //Kleen old cells
      next(data,next_data);
      count++;
      the_same=check_array(data,next_data);
     }
    quick_kleen();
    textcolor(LIGHTGRAY);
    textbackground(BLACK);
    if (count<=generation_max)
     {
      cout<<endl<<"Stop because the ECO_SYSTEM has stablized at generation "<<count-1;
      outfile<<"Stop here because the ECO_SYSTEM has stablized at generation "<<count-1<<endl;
     }
    else
     {
      cout<<endl<<"Stop because ECO_SYSTEM has existed over "<<generation_max<<" generation";
      outfile<<"Stop here because ECO_SYSTEM has existed over "<<generation_max<<" generation"<<endl;
     }
    cout<<endl<<"Data has been saved in "<<file_name<<endl;
    cout<<endl<<"Do you want to continue? Y/N ";
    cin>>answer;
    cin.get();
    infile.close();
    outfile.close();
   }
  while (toupper(answer)=='Y');
  return 0;
 }