/*
 * churn.c
 *
 * usage: churn [-v] maxblock\n");
 *
 *    Allocate & free memory
 *
 *    Tests malloc() and free() by repeatedly allocating and freeing
 *    chunks of memory of up to size maxblock.
 *
 *    -v is verbose mode, and prints a message with each call to malloc/free
 *
 * Bryan Clair 2002-2013
 * v2.0 2010: churn now actually writes data into the memory it's allocated, and checks
 *            that the data survives.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
// #include <./mymalloc.c>

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
    
    if(!ptr) {
        return;
    }
    
    struct Node *node_ptr = (struct Node*) ptr - 1;;
    node_ptr->free = 1;
}





// At any time, churn may have up to NUMSLOTS memory allocations
#define NUMSLOTS 40

char *slots[NUMSLOTS];
size_t sizes[NUMSLOTS];

main(int argc, char *argv[])
{
  int i,n,size,maxblock;
  long j;
  int verbose=0;

  // Read arguments
  maxblock = -1;
  if (argc == 3) {
    if (!strcmp(argv[1],"-d")) {
      maxblock = atoi(argv[2]);
      verbose = 1;
    }
  }
  if (argc == 2)
    maxblock = atoi(argv[1]);

  if (maxblock <= 0) {
    fprintf(stderr,"usage: churn [-d] maxblock\n");
    exit(1);
  }

  // Initialize
  for (i = 0; i<NUMSLOTS; i++) slots[i] = NULL;
  srand(time(NULL));

  long mallocs=0,frees=0;  // number of mallocs, number of frees
  printf("mallocs\tfrees\tsbrk\n");

  // Main loop:
  while (1) {
    n = rand() % NUMSLOTS;

    if (slots[n]) {
      // Going to free slot[n]
      // Check contents to see if it's still 0,1,2,3,4,...
      for (j=1; j<sizes[n]; j++) {
	if (*(slots[n]+j) != (char) j) {
	  printf("Memory corruption detected.\n");
	  exit(1);
	}
      }

      if (verbose) printf("freeing slot %d.\n",n);
      myfree(slots[n]);
      slots[n] = NULL;
      frees++;

    } else {
      // Going to malloc slot[n]
      if (verbose) printf("malloc slot %d.\n",n);

      sizes[n] = rand() % maxblock + 1;
      slots[n] = (char *) mymalloc (sizes[n]);
      if (slots[n] == NULL) {
	fprintf(stderr,"out of memory\n");
	exit(1);
      }

      // Fill block with 0,1,2,3,4,...
      for (j=0; j<sizes[n]; j++) *(slots[n]+j) = (char) j;

      mallocs++;
    }

    // Periodically report progress
    if ((mallocs + frees) % 1000 == 0)
      printf("%ld\t%ld\t%p\n",mallocs,frees,sbrk(0));
  }
}
