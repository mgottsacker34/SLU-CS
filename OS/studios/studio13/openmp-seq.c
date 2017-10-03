#include <stdio.h>
#include <stdlib.h>


int primecheck(int arg){

  if(arg==2){
    return 1;
  }

  if(arg%2==0){
    return 0;
  }

  int i = 3;
  for(; i*i <= arg; i+=2){
    if(arg%i == 0){
      return 0;
    }
  }
  return 1;

}

int main(int argc, char* argv[]){

  int num = atoi(argv[1]);
  int i = 2;

  //printf("Primes:\n");

  for(; i<num; i++){
    //if(primecheck(i) == 1){
    //  printf("%d\n", i);
    //}
    primecheck(i);
  }

/*
  if(primecheck(num) == 0){
    printf("%d is NOT prime.\n", num);
  }else{
    printf("%d is prime.\n", num);
  }
*/



  return 0;
}
