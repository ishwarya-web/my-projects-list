#include <iostream>
#include <ctime>
int main()
{
  srand(time(NULL));
  int num;
  int guess;
  int trials = 0;
  num = (rand() % 100) + 1;
  do{
    std::cout << "enter the number ";
    std::cin >> guess;
    trials++;
    if(guess > num){
      std::cout << "TOO HIGH!"<<'\n';
    }
    else if(guess < num){
      std::cout << "TOO LOW!\n";
    }
    else {
      std::cout << "CORRECT\n";
    }
  }while(num != guess);

  std::cout<<"number of trials "<<trials<<'\n';
  return 0;
}