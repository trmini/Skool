#include <iostream.h>
#include <constrea.h>

int str_length(const char *string);
char *str_copy(char *target,const char *source);
char *str_dup(const char *source);

int main()
 {
  char string1[]="Welcome to our planet!";
  char string2[40];
  char *string3;

  clrscr();
  cout<<"This is the original string: "<<string1<<endl;
  //Copy string1 into string2
  str_copy(string2,string1);
  cout<<"After copying: "<<endl<<string2<<endl;
  string3=str_dup("We will not hurt you");
  cout<<"Dupplicated string by using str_dup():"<<endl;
  cout<<string3<<endl;
  cout<<"End of test-Hit return key to exit";
  cin.get();
  return 0;
 }
//Function Definition
int str_length(const char *string)
 {
  int count=0;
  for (;*(string+count)!='\0';count++);
  return count;
 }

char* str_copy(char *target,const char *source)
 {
  int string_size=str_length(source);
  for (int index=0;index<=string_size;index++)
		target[index]=source[index];
		//or different command:
		//*(target+index)=*(source+index);
  return target;
 }

char *str_dup(const char *source)
 {
  char *target;
  int string_size=str_length(source);
  target= new char[string_size];
  for (int index=0;index<=string_size;index++)
   *(target+index)=*(source+index);
  return target;
 }