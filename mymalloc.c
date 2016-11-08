#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>                     //Possibly using linux  basic memory management (sbrk)
#include <string.h>
#include "mymalloc.h"

#define blockSignature 0xFAFAFAFA      //recognizing pattern, this is a random hex value that flags something belongs to the chunk you are working on

static char memBlock[blockSize];        // Use this memory to simulate malloc

int large   = 0;                        //Total big chunks size
int small   = 0;                        //Total small chunks size

int consideredLarge = 5000;             //Considered LARGE (can be adjusted)

int rootInit = 0;   //tracks if initialized or not (addresses first detectable error in assignment-free never allocated pointer)
int lastInit = 0;   //signifies if last of memBlock is initialized

static memEntry *root=NULL;                                 //first block of memEntry
static memEntry *last=NULL;                                 //last block of memEntry



/*
 * This function returns a void pointer to the allocated space
 */
 
void * betterMalloc(unsigned int size, char * file, int line)
{
	
    /*
     Smaller chunks are allocated from the left, with root pointing to the start of the chunk.
     The larger chunks are allocated from the right end, with last pointing to the end of the chunk.
     Use bigChunk switch indicates whether it's bigger or smaller.  This reduces fragmentation
     */
    int bigChunk = 0;

    /* Verify if valid size */
    
    
    if (size == 0) {
		printf("Cannot allocate 0 bytes\n" );
        printf("file %s line %d",file,line);
        return NULL;
	
	}
    if(size>(blockSize))
    {
        printf("enter size between 1 and %i\n", blockSize);
        printf("file %s line %d",file,line);
        return NULL;
    }

    if((large+small+sizeof(memEntry)+size+16)>(blockSize))
    {   /* Out-Of-Memory Condition */
    
        printf("Out-of-Memory \n");
        printf("file %s line %d",file,line);
        return NULL;
    }

    if(size > consideredLarge) bigChunk = 1;

    
    memEntry *next = NULL;
    memEntry *ptr;                                              // browse through memBlock via this pointer

    if(!rootInit)
    {                                                           //Initialize root here.
        root = (memEntry*)memBlock;                             //point root to start of the block
        rootInit = 1;
        root->free = 1;
        root->prev=root->next=NULL;                             //initialize the pointers to NULL
        root->size=(blockSize) / 2 - sizeof(memEntry);          //store suitable size for small chunks
        root->signedPattern=blockSignature;                     //signed the block
    }

    if(!lastInit)
    {                                                           //if last is not initialized, point last to end - size required for the chunk
        last=(memEntry*)(memBlock+(blockSize)-sizeof(memEntry)- size);
        lastInit = 1;
        last->free = 1;
        last->prev=last->next=NULL;
        last->size=(blockSize)/2-sizeof(memEntry);              //store suitable size for big chunks
        last->signedPattern=blockSignature;
    }
    ptr = root;                                                 //point to start of the block
    if(!bigChunk)
    {
        do  {
            if(ptr->size < size)    ptr = ptr->next;            //if the current chunk is small, skip it, we can't malloc here
            else if(!ptr->free)     ptr = ptr->next;            //block is free but already allocated, then move to head
            else if(ptr->size < (size+sizeof(memEntry)+4))
            {       /* check if requested size + memEntry overhead size doesnt exceed availble space.
                       Pad by 4 bytes to avoid out of memory condition. */
                ptr->free = 0;                                  //allocate here
                ptr->signedPattern=blockSignature;
                return ((char*)ptr+sizeof(memEntry));           // returns the address where the data field starts from
            }
            else
            {
                next = (memEntry*)((char*)ptr+sizeof(memEntry) + size);
                                                                //create memEntry for a new chunk
                next->prev = ptr;
                next->next = ptr->next;
                if(ptr->next != NULL)   ptr->next->prev = next; //if its not the end of heap, point the next node's previous to next

                ptr->next = next;                               //point ptr's next to next node
                next->size = ptr->size-sizeof(memEntry) - size; //out of the original size, subtract memEntry and size
                next->free = 1;                                 //once ptr is used, the remaining (next) is free
                ptr->size = size;
                ptr->signedPattern = blockSignature;
                ptr->free = 0;                                  // allocated here
                return ((char*)ptr + sizeof(memEntry));
            }

        } while(ptr != NULL);
    }
    else                                                        // Process large chunks here
    {
        ptr = last;
        do
        {
            if(ptr->size<size) ptr = ptr->next;                 //if the current chunk is small, skip it, we can't allocate here

            else if(!ptr->free)  ptr = ptr->next;               //memory available, but already allocated
            else if(ptr->size < (size+sizeof(memEntry)+4))
            {
                ptr->signedPattern = blockSignature;
                ptr->free = 0;                                  //allocate here
                return ptr + 1;                                 //return the address where the data field starts from.
            }

            else
            {
                next = (memEntry*)((char*)ptr - sizeof(memEntry)-size);
                                                                //memEntry for new chunk
                next->prev = ptr;                               //link the next memEntry's previous to current node
                next->next = ptr->next;                         //link the next pointer as next's next node
                if(ptr->next != NULL) ptr->next->prev = next;   //if its not the end of heap, point the next node's previous to next


                ptr->next = next;                               //set ptr's next to next node
                next->size = (blockSize) / 2-sizeof(memEntry) - size;
                next->free = 1;                                 //once ptr is used, the remaining (next) is free
                ptr->size = size;
                ptr->signedPattern = blockSignature;            //mark the block with pre-defined pattern above
                ptr->free = 0;                                  //allocate here
                return ((char*)ptr + sizeof(memEntry));
            }

        } while(ptr != NULL);
    }
    return 0;
}



void betterFree(void * x, char * file, int line)
{
    memEntry *next = NULL;
    memEntry *prev = NULL;
    memEntry *ptr = NULL;

    //check if ptr already exists within the chunk
    if(!((x - (void*)memBlock>=0)&&(x - (void*)memBlock<(blockSize))))
    {
        printf("INPUT is not in the block\n");
        printf("file %s line %d\n",file,line);
        return;
    }
    ptr = (memEntry*)x - 1; //point to memEntry, not the size field we returned in malloc

    //verify if pointer points to a memEntry and not random location
    if((ptr)->signedPattern!=blockSignature)
    {
        printf("Pointer is outside of memEntry\n");
        printf("file %s line %d\n",file,line);
        return;
    }

    if(ptr->free==1) //redundant freeing
    {
        printf("We're trying to free a chunk already freed\n");
        printf("file %s line %d\n",file,line);
        return;
    }

    if (ptr->size < consideredLarge) large -= ptr->size;        //update the allocated space
    else small += ptr->size;

    if((prev = ptr->prev)!= NULL && prev->free)
    {
        prev->size += sizeof(memEntry) + ptr->size;             //add current to  previous  chunk
        prev->next = ptr->next;                                 //link next pointer over the chunk we are freeing
        if(ptr->next != NULL)  ptr->next->prev = prev;          //link prev to previous chunk
    }
    else
    {   //previous is not free, set prev to current chunk
        ptr->free = 1;
        prev = ptr;
    }

    if( (next=ptr->next)!=NULL && next->free)
    {
        prev->size += sizeof(memEntry)+next->size;              //combine current and next chunk
        prev->next = next->next;
        if(next->next != NULL)  next->next = prev;              //link over the current chunk to be freed
    }
    printf("freed block at address %p\n", x);
}

void * betterCalloc( int items, unsigned int size, char * file, int line){
	char * block = (char*) betterMalloc (items* size, __FILE__, __LINE__);   //allocate space
	if (block == NULL) {
		printf("Unable to allocate with calloc \n");
		printf("file %s line %d\n",file,line);
		return NULL;
	}
	memset(block,0,size);                                        //zero out bits
	return block;
	

}

void *betterRealloc(void * x, int size, char * file, int line) {
	memEntry * ptr = root;				                                //Start search in small blocks
	char* original = NULL;
	char * newBlock = NULL;
	
	while(ptr != NULL){						
		if (((char *)ptr+sizeof(memEntry)) < (char *)x) {               //have not found correct memEntry
			ptr = ptr->next;				
		}else if (((char *)ptr+sizeof(memEntry)) > (char *)x) {	        //target is not in the block
			printf("INPUT is not in the block\n");
			printf("file %s line %d\n",file,line);
			return NULL;
		}else { break;							                            //target found
		}
	}
	
	if(ptr == NULL){						//still have not found target
		ptr = last;						   //search big block
		while(ptr != NULL){					
			if (((char *)ptr+sizeof(memEntry)) < (char *)x) {           //increment through blocks
				ptr = ptr->next;				                  
			}else if (((char *)ptr+sizeof(memEntry)) > (char *)x) {		 //target is not in the block	
				printf("INPUT is not in the block\n");
				printf("file %s line %d\n",file,line);

				return NULL;
			}else { break;							                         //target found
			}
		}
	}
	
	if(ptr == NULL){						//target not found in memory block.
		  
		printf("Pointer is outside of memEntry\n");
        printf("file %s line %d\n",file,line);
        return NULL;
	}

	betterFree(x, __FILE__, __LINE__); 
	newBlock = (char*)betterMalloc(size, __FILE__, __LINE__);		//get start of allocated block
	
	if(newBlock == NULL){
		printf ( "Error in realloc\n");
		printf("file %s line %d\n",file,line);

		return NULL;
	}
	
	original = (char *)x;                   
	int i = 0;                                //copy old values to new block
	while(i < size) {
		newBlock[i] = original[i];
		i++;
	}
	

return newBlock;
}



