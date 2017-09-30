// Trang Luu
// COSC 2318.001
// Program 3: When will the IRS visit?
// Due date: 10/05/98
// Display a table of values indicating the asset's current value and current
// depreciation amount.

#include "program3.h"

int main()
 {
  double original_cost, cost, salvage, depreciation;
  int count, year, life;
  char answer='Y', item_name[21];

  welcome();  // Welcome!
  decimal(2); // set display mode for real number

/*Main part of program*/

  while (toupper(answer)=='Y')
   {
    depreciation=0.0;
    get_data(item_name,original_cost,year,life,salvage);
    count=0;
    cost=original_cost;
    print_data(item_name,original_cost,year,life,salvage);

    while (count<life)
     {
      depreciation=de_amount(cost,life); //Calculate depereciation value by function
      if ((cost-depreciation)>salvage)
       {
	print_data((year+count),cost,depreciation);
	cost-=depreciation;
       }
      else
       {
	depreciation=(cost-salvage);
	print_data((year+count),cost,depreciation);
	cost-=depreciation;
       }

      if ((count%10)==9 && count!=(life-1))
       {
	 gotoxy(22,24);
	 cout<<"Hit enter to view the next page...";
	 cin.get();
	 clrscr();
	 print_data(item_name,original_cost,year,life,salvage);
       }

      count++;
     }
    gotoxy(22,24);
    cout<<"Do you want to continue for another item? Y/N ";
    cin>>answer;
    cin.get();
    clrscr();
   }

/*End of main part*/

 cout<<"Thank you!";
 return 0;
 }