#include <iostream>
#include <ctime>
char playerschoice(){
  char choice;
  do{
  std::cout << "choose one of the following\n";
  std::cout << "r is for rock\n";
  std::cout << "p is for paper\n";
  std::cout << "s is for sissor\n";
  std::cout << "enter your choice: ";
  std::cin >> choice;
  }while(choice != 'r' && choice != 'p' && choice != 's');
  return choice;
}

void thechoice(char choice){
  switch(choice){
    case 'r' : 
      std::cout << "rock\n";
      break;
    case 'p' : 
      std::cout << "paper\n";
      break;
    case 's' :
      std::cout << "sissor\n";
      break;
  }
}

char computerchoice(){
  srand(time(NULL));
  int num;
  char choice;
  num = rand() % 3 + 1;
  switch(num){
    case 1 : 
      choice = 'r';
      break;
    case 2 : 
      choice = 'p';
      break;
    case 3 : 
      choice = 's';
      break;
  }
  return choice;
}

void winner(char pchoice,char cchoice){
  if((pchoice == 'r' && cchoice == 's') || (pchoice == 's' && cchoice == 'p') || (pchoice == 'p' && cchoice == 'r'))
  {
    std::cout << "YOU WON!\n";
  }
  else if((pchoice == 'r' && cchoice == 'p') ||(pchoice == 'p' && cchoice == 's') || (pchoice == 's' && cchoice == 'r')){
    std::cout << "COMPUTER WON!\n";
  }
  else{
    std::cout << "THAT IS A TIE!\n";
  }
}

int main(){
  
  char pchoice;
  char cchoice;
  pchoice = playerschoice();
  cchoice=computerchoice();
  std::cout << "player choice is ";
  thechoice(pchoice);
  std::cout << "computer choice is ";
  thechoice(cchoice);
  winner(pchoice,cchoice);
  return 0;
}