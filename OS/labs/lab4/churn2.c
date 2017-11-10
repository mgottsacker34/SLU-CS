/*
 * churn2.c
 *    usage: churn2 [-d] maxblock
 *
 *    Test malloc(), calloc(), realloc() and free() by
 *    repeatedly alloc/freeing NUMSLOTS blocks of memory
 *    of random size between 1 and maxblock.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define NUMSLOTS 32
#define CHURNS 20

long *slots[NUMSLOTS];
int sizes[NUMSLOTS];

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
    
    //printf("starting new node function\n");
    
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
    
    //printf("new node attributes assigned\n");
    
    if (tail) {
        tail->next = new_node;
    }
    
    //printf("Creating head\n");
    tail = new_node;
    
    //printf("new node set to tail\n");
    
    return tail;
}


void *mymalloc(size_t size) {
    
    struct Node *pointer;
    
    if (!head) {                        // first call
        //printf("Creating head\n");
        pointer = new_node(size);
        //printf("Head succesfully created\n");
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
    
    //printf("pointer value: %d\n", (pointer));
    return pointer+1;
    
}

void myfree(void *ptr) {
    if(!ptr) { return; }
    struct Node *node_ptr = (struct Node*) ptr - 1;
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


void *myrealloc(void *pointer, size_t size) {
    if (!pointer) {
        mymalloc(size);
    }
    if ((size == 0) && (pointer)) {
        myfree(pointer);
    }
    
    struct Node *node_ptr = (struct Node*) pointer - 1;
    void* new_mem = malloc(size);
    memcpy(new_mem, node_ptr, size);
    myfree(node_ptr);
    return new_mem;
}

main(int argc, char *argv[])
{
  int i,j,n,size,maxblock;
  int c=0, f=0, r=0;
  int d=0;
  int cc=0;

  maxblock = -1;
  if (argc == 3) {
    if (!strcmp(argv[1],"-d")) {
      maxblock = atoi(argv[2]);
      d = 1;
    }
  }
  if (argc == 2)
    maxblock = atoi(argv[1]);

  if (maxblock <= 0) {
    fprintf(stderr,"usage: churn2 [-d] maxblock\n");
    exit(1);
  }

  for (i = 0; i<NUMSLOTS; i++) slots[i] = NULL;
  srand(time(NULL));
  
  printf("calloc\trealloc\tfree\tsbrk\n");

  while (cc < CHURNS) {
    for (i = 0; i < NUMSLOTS * NUMSLOTS; i++) {
      n = rand() % NUMSLOTS;

      if (slots[n]) {
	/* Slot is full */
	if (d) printf("Testing slot %d...",n);

	/* Check contents */
	for (j=1; j<sizes[n]; j++) {
	  if (*(slots[n]+j) != j) {
	    printf("Memory error detected.\n");
	    exit(1);
	  }
	}
	
	if (d) printf("ok\n");

	/* Free or realloc at random */
	if (rand() % 2) {
	  /* Free it */
	  if (d) printf("Freeing slot %d.\n",n);
	  myfree(slots[n]);
	  slots[n] = NULL;
	  f++;

	} else {
	  /* Reallocate it */
	  if (d) printf("Realloc slot %d...",n);
	  size = rand() % maxblock + 1;
	  slots[n] = (long *) myrealloc(slots[n],size*sizeof(long));
	  if (slots[n] == NULL) {
	    fprintf(stderr,"realloc: out of memory\n");
	    exit(1);
	  }
	  if (sizes[n] < size) {
	    if (d) printf("larger.\n");
	    for (j = sizes[n]; j<size; j++) {
	      *(slots[n]+j) = j;
	    }
	  } else {
	    if (d) printf("smaller.\n");
	  }
	  sizes[n] = size;

	  r++;

	}

      } else {
	/* Slot is empty */
	if (d) printf("Calloc  slot %d.\n",n);

	sizes[n] = rand() % maxblock + 1;
	slots[n] = (long *) mycalloc (sizes[n],sizeof(long));

	if (slots[n] == NULL) {
	  fprintf(stderr,"calloc: out of memory\n");
	  exit(1);
	}

	/* Fill block with 1,2,3,4,... */
	for (j=0; j<sizes[n]; j++) {
	  if (*(slots[n]+j)) {
	    printf("Calloc failed to zero memory\n");
	    exit(1);
	  }
	  *(slots[n]+j) = j;
	}

	c++;
      }

    }
    printf("%d\t%d\t%d\t%p\n",c,r,f,sbrk(0));
    c = 0; f = 0; r=0;
    cc++;
  }
}
