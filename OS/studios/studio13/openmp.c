#include <stdio.h>
#include <stdlib.h>

int primecheck(int arg){
  
  int i = 2;
  for(; i < arg; i++){
    if(arg%i == 0){
      return 0;
    }
  }
  return 1;
}

int main(int argc, char* argv[]){

  int num = atoi(argv[1]);
  if(primecheck(num) == 0){
    printf("Not prime.\n");
  }else{
    printf("Prime!\n");
  }




  return 0;
}

