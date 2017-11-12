// Matt Gannon & Matt Gottsacker
// 11/4/2017
// Program to recreate memory allocation routines

#include <sys/types.h>
#include <pthread.h> // for pthread_create(), phtread_mutex_init
#include <unistd.h> //fork(), execvp(), perror(), waidpid()
#include <stdlib.h> //For exit()
#include <stdio.h> //For printf()
#include <signal.h> //For signal()
#include <errno.h> //For perror & errno
#include <string.h>


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
    
    //Get memory chunk from OS
    struct Node *new_node;
    new_node = sbrk(0);
    void* ret = sbrk(size + NODE_SIZE);
    new_node = ret;
    
    //Check for success, else return NULL
    if( ret == (void*)-1 ){
        errno = ENOMEM;
        return NULL;
    }
    
    new_node->size = size;
    new_node->next = NULL;
    new_node->free = 0;
    
    
    if (tail) {
        tail->next = new_node;
    }
    
    tail = new_node;
    return tail;
}


void *malloc(size_t size) {
    
    struct Node *pointer;
    
    if (!head) {                        // first call
        pointer = new_node(size);
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
    
    return pointer+1;
    
}

void free(void *ptr) {
    if(!ptr) { return; }
    struct Node *node_ptr = (struct Node*) ptr - 1;
    node_ptr->free = 1;
}


void *calloc(size_t num_of_elts, size_t elt_size) {
    if((num_of_elts == 0) || (elt_size == 0)) {
        return NULL;
    }
    size_t size = num_of_elts * elt_size;
    void *pointer = malloc(size);
    memset(pointer, 0, size);
    return pointer;
}


void *realloc(void *pointer, size_t size) {
    if (!pointer) {
        return malloc(size);
    }
    if ((size == 0) && (pointer)) {
        free(pointer);
        return NULL;
    }
    
    //struct Node *node_ptr = (struct Node*) pointer - 1;
    void* new_mem = malloc(size);
    memcpy(new_mem, pointer, size);
    free(pointer);
    return new_mem;
}
