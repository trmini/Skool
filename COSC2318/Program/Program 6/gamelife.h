#include <iostream.h>
#include <fstream.h>
#include <constrea.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

const int max_size=10;
// max_size must be less than or equal to 10 to display on the screen
// For an output file every number is OK
const int LENGTH=2000;

const int board_row=(2*max_size+1);
const int board_column=(4*max_size+1);


enum BOOLEAN {FALSE=0,TRUE=1};

struct board_array
 {
  char board[board_row][board_column];
 };

struct data_array
 {
  char board[max_size][max_size];
 };

void quick_kleen();
//the same as clrscr()

void fill_background();
//paint background

void kleen_array(data_array&);
//Initialize every cells by space
//data type is data_array

void kleen_array(board_array&);
//Initialize every cells by spaces
//data type is board_array

void initialize_array(board_array&);
//Put borders into board
//Data type is board_array

void get_data(ifstream&,data_array&);
//get data from a file and put it into data_array
//This function will preprocess data by considering x or X only
//Finally, counters saved in data_array are X only

BOOLEAN check_array(data_array,data_array);
//input data of type data_array
//returns TRUE if they are the same;
//Otherwise returns FALSE

void assign_array(data_array,board_array&);
//Move actual data from data_array into board_array, which contains the borders

void save_data(board_array,int,ofstream&);
//Save board_array in an output file

void display_array(board_array);
//Display board_array at the center of the screen

void next(data_array,data_array&);
//Calculate for the next generation
//Data to consider is from data_array and the next generation is saved a new array

int surround(data_array,int,int);
//returns the number of counters around a cell
//the first int is the row and the other is the column

void welcome();
//Welcome to the game

//+++Definition+++

void quick_kleen()
 {
  int far *point;
  int addr;
  point=(int far *) 0xB8000000;
  for (addr=0;addr<LENGTH;addr++) *(point+addr)=0x0700;
  gotoxy(1,1);
  return;
 }

void fill_background()
 {
  int far *point;
  int addr;
  point=(int far *) 0xB8000000;
  for (addr=0;addr<LENGTH; addr++)
   *(point+addr)=0x17B2; //char(178)|0x1700
 }

void kleen_array(data_array&databoard)
 {
  int row,column;
  for (row=0;row<max_size;row++)
   for (column=0;column<max_size;column++)
    databoard.board[row][column]=' ';
  return;
 }

void kleen_array(board_array&checkerboard)
 {
  int row,column;
  for (row=0;row<board_row;row++)
   for (column=0;column<board_column;column++)
    checkerboard.board[row][column]=' ';
  return;
 }

void initialize_array(board_array&checkerboard)
 {
  int row,column;
  //Assign corner
  checkerboard.board[0][0]=char(201);            //É
  checkerboard.board[0][board_column-1]=char(187); //»
  checkerboard.board[board_row-1][0]=char(200); //È
  checkerboard.board[board_row-1][board_column-1]=char(188); //¼
  //Assixgn frame
  for (column=1;column<board_column-1;column++)
   {
    if ((column%4)==0)
     {
      checkerboard.board[0][column]=char(209); //Ñ
      checkerboard.board[board_row-1][column]=char(207); //Ï
     }
    else
     {
      checkerboard.board[0][column]=char(205); //Í
      checkerboard.board[board_row-1][column]=char(205); //Í
     }
   }
  for (row=1;row<board_row-1;row++)
   {
    if ((row%2)==0)
     {
      checkerboard.board[row][0]=char(199); //Ç
      checkerboard.board[row][board_column-1]=char(182); //¶
     }
    else
     {
      checkerboard.board[row][0]=char(186); //º
      checkerboard.board[row][board_column-1]=char(186); //º
     }
   }
  //Assign elements
  for (row=2;row<board_row-2;row=row+2)
   {
   for (column=1;column<board_column-1;column++)
    {
     if ((column%4)==0)
      {
       checkerboard.board[row-1][column]=char(179);   //³
       checkerboard.board[row][column]=char(197);     //Å
       checkerboard.board[row+1][column]=char(179);   //³
      }
     else
      {
       checkerboard.board[row][column]=char(196);     //Ä
      }
    }
   }
  return;
 }

void get_data(ifstream&infile,data_array&databoard)
 {
  int row,column;
  char temp;
  for (row=0;row<max_size;row++)
   for (column=0;column<max_size;column++)
    {
     if (infile>>temp)
      {
       if (toupper(temp)=='X') databoard.board[row][column]='X';
      }
    }
  return;
 }

BOOLEAN check_array(data_array data1,data_array data2)
 {
  int row,column;
  for (row=0;row<max_size;row++)
   for (column=0;column<max_size;column++)
    if (data1.board[row][column]!=data2.board[row][column]) return FALSE;
  return TRUE;
 }

void assign_array(data_array databoard, board_array&checkerboard)
 {
  int row,column;
  for (row=0;row<max_size;row++)
   for (column=0;column<max_size;column++)
    checkerboard.board[2*row+1][4*column+2]=databoard.board[row][column];
  return;
 }

void save_data(board_array checkerboard,int count,ofstream&outfile)
 {
  int row,column;
  if (count==0)
   {
    outfile<<"The original generation"<<endl;
   }
  else
   {
    outfile<<"Generation number "<<count<<endl;
   }
  for (row=0;row<board_row;row++)
   {
    for (column=0;column<board_column;column++)
     {
      outfile<<checkerboard.board[row][column];
     }
    outfile<<endl;
   }
  outfile<<endl;
  return;
 }

void display_array(board_array checkerboard)
 {
  int row,column,address;
  int far *point;
  point=(int far *) 0xB8000000;
  for (row=0;row<board_row;row++)
   for (column=0;column<board_column;column++)
    {
     char temp;
     address=80*(row+(25-board_row)/2)+(column+(80-board_column)/2);
     temp=checkerboard.board[row][column];
     if (temp=='X'||temp==' ')
      *(point+address)=temp|0x1A00;//Green on blue |0x0F00 for normal
     else
      *(point+address)=temp^0xE100;//Yellow on blue ^0xF100 for normal
    }
  //make shadow for checkerboard
  for (row=0;row<board_row;row++)
   {
    address=80*(row+1+(25-board_row)/2)+(board_column+(80-board_column)/2);
    *(point+address)=0x0;
    *(point+address+1)=0x0;
   }
  for (column=0;column<board_column-2;column++)
   {
    address=80*(board_row+(25-board_row)/2)+((80-board_column)/2+column+2);
    *(point+address)=0x0;
   }
  return;
 }

void next(data_array original,data_array&copy)
 {
  int row,column;
  int counter;
  kleen_array(copy);
  for (row=1;row<max_size-1;row++)
   for (column=0;column<max_size;column++)
    {
     counter=surround(original,row,column);
     if (original.board[row][column]=='X')
      {
       if (counter==2||counter==3) copy.board[row][column]='X';
      }
     else
      {
       if (counter==3) copy.board[row][column]='X';
      }
    }
  return;
 }

int surround(data_array original,int row,int column)
 {
  int i,j;
  int counter=0;
  if (column!=0&&column!=(max_size-1))
   {
    for (i=-1;i<2;i++)
     for (j=-1;j<2;j++)
      {
       if (original.board[row+i][column+j]=='X') counter++;
      }
   }
  else
   {
    for (i=-1;i<2;i++)
     {
      if (original.board[row+i][column]=='X') counter++;
      if (original.board[row+i][abs(column-1)]=='X') counter++;
     }
   }
  if (original.board[row][column]=='X') counter--;
  return counter;
 }

void welcome()
 {
  gotoxy(28,1);
  cout<<"Welcome to the game of life!";
  gotoxy(1,3);
  cout<<"  First of all you need a file that contains the original generation. You must"<<endl;
  cout<<"use a X (or x) to represent a counter and a _ (or any other character) for the"<<endl;
  cout<<"absence of a counter. Your data should be some like the following matrix."<<endl<<endl;
  cout<<"An example for matrix 10x10"<<endl;
  cout<<"__________"<<endl;
  cout<<"x_x_xx_xxx"<<endl;
  cout<<"xxxxx_____"<<endl;
  cout<<"___xxxxxxx"<<endl;
  cout<<"__xxx___x_"<<endl;
  cout<<"x__xxxxx__"<<endl;
  cout<<"__x_xx_x_x"<<endl;
  cout<<"xx_xx__xx_"<<endl;
  cout<<"x_xxx__x__"<<endl;
  cout<<"__________"<<endl;
  cout<<endl<<endl;
  cout<<"The game is assumed that the first and the last row are infertile regions."<<endl;
  cout<<"Do not put any counters there. The program will stop reading data when the"<<endl;
  cout<<"board is full. Make sure that your data is correct before running the game."<<endl;
  cout<<"Now you can hit enter to begin!";
  cin.get();
 }