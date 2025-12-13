#include <stdio.h>
int main(){
  FILE *fptr;
  fptr = fopen("text.txt","w");
  char ch;
  fprintf(fptr,"%c",'i');
  fprintf(fptr,"%c",'s');
  fprintf(fptr,"%c",'h');
  fprintf(fptr,"%c",'u');
  fclose(fptr);
  return 0;
}