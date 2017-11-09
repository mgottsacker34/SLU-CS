// Matt Gannon & Matt Gottsacker
// 11/4/2017
// Program to recreate memory allocation routines
// Doubly linked list implementation borrowed from https://gist.github.com/mycodeschool/7429492

#include <sys/types.h>
#include <pthread.h> // for pthread_create(), phtread_mutex_init
#include <unistd.h> //fork(), execvp(), perror(), waidpid()
#include <stdlib.h> //For exit()
#include <stdio.h> //For printf()
#include <signal.h> //For signal()
#include <errno.h> //For perror & errno

//extern void* mymalloc(size_t size);
//extern void myfree(void *ptr);

#define array_size 500
#define short_array 250

// Create linked list structure
struct Node {
    size_t size;
    int free;               // 1 to indicate free, 0 to indicate not free
    struct Node* next;
};

#define NODE_SIZE sizeof(struct Node)

struct Node* head;
struct Node* tail;

//Helper functions for malloc

//Finding a the first fitting free node
struct Node* first_free(size_t size) {
    struct Node *current = head;
    while((current->next != NULL) && !((current->free == 1) && (current->size >= size))){
        current = current->next;
    }
    return current;
}

//Create a new node/memory block
struct Node *new_node(size_t size) {
    
    printf("starting new node function\n");
    
    //Get memory chunk from OS
    struct Node *new_node;
    new_node = sbrk(0);
    void* ret = sbrk(size  + NODE_SIZE);
    new_node = ret;
    
    //Check for success, else return NULL
    if( ret == (void*)-1 ){
        printf("Error calling sbrk!\n");
        errno = ENOMEM;
        return NULL;
    }
    
    new_node->size = size;
    new_node->next = NULL;
    new_node->free = 0;
    
    printf("new node attributes assigned\n");
    
    if (tail) {
        tail->next = new_node;
    }
    
    printf("Creating head\n");
    tail = new_node;
    
    printf("new node set to tail\n");
    
    return tail;
}


void *mymalloc(size_t size) {
    
    struct Node *pointer;
    
    if (!head) {                        // first call
        printf("Creating head\n");
        pointer = new_node(size);
        printf("Head succesfully created\n");
        head = pointer;
        tail = head;
    } else {
        pointer = first_free(size);
        if (!pointer) {
            pointer = new_node(size);
        } else {
            pointer->free = 0;
        }
    }
    
    return pointer + 1;
    
}



/*
 void myfree( void *ptr );
 void *mycalloc( size_t num_of_elts, size_t elt_size ) {;
 void *myrealloc( void *pointer, size_t size);
 */

int main (int argc, const char *argv[]) {
    
    //Create and destroy two arrays twice
    int *array = (int*) mymalloc(sizeof(int) * array_size);
    int *array2 = (int*) mymalloc(sizeof(int) * array_size);
    
    int i;
    for(i =0 ; i< array_size; i++) {
        array[i] = i;
    }
    
    //myfree(array);
    //myfree(array2);
    
    return 0;
}




