#include "a:\sortlib.h"

#define max_size 75

void welcome();

int main()
 {
  ifstream input;
  ofstream output;
  int original_data[max_size],copy1_data[max_size],copy2_data[max_size];
  char file_name[max_size],answer='Y';
  int array_size,index;
  int buble_compare,buble_swap;
  int select_compare,select_swap;

  while (toupper(answer)=='Y')
   {
    welcome();
    clrscr();
    cout<<"Please enter your data file with full direction: "<<endl;
    cin>>file_name;
    cin.get();
    input.open(file_name);
    while (input.fail())
     {
      input.clear();
      cout<<"Error when open input file!"<<endl;
      cout<<"Re-enter your file name: ";
      cin>>file_name;
      cin.get();
      input.open(file_name);
     }
    do
     {
      cout<<"Please enter your desired output file with full direction:"<<endl;
      cin>>file_name;
      cin.get();
      cout<<"Are you sure there is no files with this name? (Y/N) ";
      cin>>answer;
      cin.get();
      cout<<endl;
     }
    while (toupper(answer)!='Y');
    output.open(file_name);
    if (output.fail())
     {
      cout<<"Cannot create the output file!"<<endl;
      cout<<"Check and run program again."<<endl;
      exit(1);
     }
    print_header(output);
    cout<<"Processing...."<<endl;
    get_data(original_data,input,array_size);
    for (index=0;index<array_size;index++)
     {
      copy1_data[index]=original_data[index]; // Copy data
      copy2_data[index]=original_data[index];
     }
    buble(copy1_data,array_size,buble_swap,buble_compare); //Sorting
    selection(copy2_data,array_size,select_swap,select_compare); //Sorting
    output<<"Data Length: "<<array_size<<" cell(s)"<<endl;
    output<<"                Swap(s) and Compare(s)"<<endl;
    output<<"Buble    : ";
    print_data(output,buble_swap,buble_compare);
    output<<"Selection: ";
    print_data(output,select_swap,select_compare);
    output<<endl;
    output<<"Original Data   Sorted Data:     Buble     Selection"<<endl<<endl;
    print_data(output,original_data,copy1_data,copy2_data,array_size);
    cout<<"Data have been saved in "<<file_name<<endl;
    cout<<endl<<"Do you want to sort another data file? Y/N ";
    cin>>answer;
    cin.get();
    input.close();
    output.close();
   }
  return 0;
 }

void welcome()
 {
  clrscr();
  cout<<"                        Welcome to the sorting program!"<<endl<<endl;
  cout<<"   This program will sort your data in the order from small to larger number."<<endl;
  cout<<"You give the program your data file; it will read data into an array and sort"<<endl;
  cout<<"on this array. You also give the output file to receive sorted data. In your"<<endl;
  cout<<"output file you will see the number of swap and compare. These numbers show the"<<endl;
  cout<<"difference between two methods-buble and selection."<<endl;
  cout<<endl<<"                                               Hit enter to continue!";
  cin.get();
  return;
 }