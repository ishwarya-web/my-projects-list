#include <iostream>
#include <cmath>
int main()
{
  char opr;
  int a,b;
  std::cout<<"enter the operation: "<<'\n';
  std::cin>>opr;
  std::cout<<"enter the numbers "<<'\n';
  std::cin>>a;
  std::cout<<"enter the next number "<<'\n';
  std::cin>>b;
  switch(opr){
    case '+':
      std::cout<<a+b;
      break;
    case '-':
      std::cout<<a-b;
      break;
    case '/':
      std::cout<<a/b;
      break;
    case '*':
      std::cout<<a*b;
      break;
    case '^':
      std::cout<<pow(a,b);
      break;
    default:
      std::cout<<"please enter a valid operation"<<'\n';
      break;
  }
  return 0;
}