// Matt Gannon and Matt Gottsacker
// 9/27/2017
// A program to learn about the inner workings of race conditions

#include <pthread.h> // for pthread_create()
#include <unistd.h> //fork(), execvp(), perror(), waidpid()
#include <stdlib.h> //For exit()
#include <stdio.h> //For printf()
#include <signal.h> //For signal()
int race = 0;

void* adder(int race) {
    int i = 0;
    for(; i<2000000; i++) {
        race++;
    }
    return NULL;
}

void* subtractor(int race) {
    int i = 0;
    for(; i<2000000; i++) {
        race--;
    }
    return NULL;
}

int main( int argc, char* argv[] ){
    
    pthread_t adder_thread;
    pthread_t subtractor_thread;
    
    printf("First check\n");
    
    int create_add_ret = pthread_create(&adder_thread, NULL, adder(race), NULL);
    if(create_add_ret != 0){
        printf("Error creating thread. Error #%d. Exiting.", create_add_ret);
        exit(-1);
    }
    
    printf("Second check\n");
    
    int create_sub_ret = pthread_create(&subtractor_thread, NULL, subtractor(race), NULL);
    if(create_sub_ret != 0){
        printf("Error creating thread. Error #%d. Exiting.", create_sub_ret);
        exit(-1);
    }
    
    printf("Third check\n");
    
    int join_add_ret = pthread_join(adder_thread, NULL);
    if(join_add_ret != 0){
        printf("Error joining thread. Error #%d. Exiting.", join_add_ret);
        exit(-1);
    }
    
    printf("Fourth check\n");;
    
    int join_sub_ret = pthread_join(subtractor_thread, NULL);
    if(join_sub_ret != 0){
        printf("Error joining thread. Error #%d. Exiting.", join_sub_ret);
        exit(-1);
    }
    
    printf("Race is %d\n", race);
}

