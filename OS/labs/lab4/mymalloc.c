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
#include <string.h>

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
    while((current) && !((current->free == 1) && (current->size >= size))){
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
    void* ret = sbrk(size + NODE_SIZE);
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
    
    printf("pointer value: %d\n", (pointer));
    return pointer+1;
    
}

void myfree(void *ptr) {
    if(!ptr) { return; }
    struct Node *node_ptr = (struct Node*) ptr - 1;;
    node_ptr->free = 1;
}


void *mycalloc(size_t num_of_elts, size_t elt_size) {
    if((num_of_elts == 0) || (elt_size == 0)) {
        return NULL;
    }
    size_t size = num_of_elts * elt_size;
    void *pointer = mymalloc(size);
    memset(pointer, 0, size);
    return pointer;
}


/*
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
    
    /*
    i =0;
    for(;i<array_size; i++){
        printf("%d\n", array[i]);
    }
    */
    
    myfree(*array);
    myfree(*array2);
    
    /*
    //test 2
    char* str;
    // Initial memory allocation
    str = (char *) mymalloc(15);
    strcpy(str, "tutorialspoint");
    printf("String = %s,  Address = %u\n", str, str);

    // Reallocating memory
    str = (char *) realloc(str, 25);
    strcat(str, ".com");
    printf("String = %s,  Address = %u\n", str, str);

    free(str);
    
    */
    
    return 0;
}
