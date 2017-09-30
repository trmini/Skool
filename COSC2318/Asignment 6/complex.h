// Do Assignment 6
// Library containts Complex decleration and operation

struct Complex
 {
  int real;
  int image;
 };

Complex cal(Complex,char,Complex);

Complex cal(Complex number1,char operation, Complex number2)
 {
  Complex result;
  if (operation=='+')
   {
    result.real=number1.real+number2.real;
    result.image=number1.image+number2.image;
    return result;
   }
  else if (operation=='-')
   {
    result.real=number1.real-number2.real;
    result.image=number1.image-number2.image;
    return result;
   }
  else if (operation=='*')
   {
    result.real=number1.real*number2.real-number1.image*number2.image;
    result.image=number1.real*number2.image+number2.real*number1.image;
    return result;
   }
  else return result;
 }