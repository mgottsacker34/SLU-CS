#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>

int main(int argc, char* argv[]){

  int i;
  int num = atoi(argv[1]);

  omp_set_num_threads(5);
  printf("Max threads: %d\n", omp_get_max_threads());

  #pragma omp parallel for schedule(dynamic, 4)
  for(i = 0; i < num; i++){
    if(i <= 4){
      sleep(1);
    }
    printf("Current loop index: %d, currently executing thread: %d\n", i, omp_get_thread_num());
  }
  return 0;
}
