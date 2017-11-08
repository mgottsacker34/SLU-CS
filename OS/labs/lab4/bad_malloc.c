//Please remember that this is a BAD implementation of malloc!
//It should work, but it does not re-use memory efficiently.

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

//Every time malloc is called we request new memory from the OS. If we succeed,
//we return the pointer from sbrk(). If we fail, we set errno and return NULL.
//We do not search for unallocated memory that can be re-used.
void* malloc(size_t size){

	//Get memory chunk from OS
	void* ret = sbrk(size);

	//Check for success, else return NULL
	if( ret == (void*)-1 ){
		printf("Error calling sbrk!\n");
		errno = ENOMEM;
		return NULL;
	}

	printf("Malloc called with size %d, returning pointer to %p\n", size, ret);

	return ret; 

}

//This free function does nothing. It does not reclaim memory.
//It returns nothing. It conforms to the man page that specifies that
//if free is called with a NULL pointer, then we do nothing.
void free(void *ptr){
	if( ptr != NULL )
		printf("Free called on pointer %p\n", ptr);
}

