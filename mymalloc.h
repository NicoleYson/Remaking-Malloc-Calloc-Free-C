#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include <stdio.h>

#define malloc( x ) betterMalloc( x , __FILE__ , __LINE__ )
#define free( x ) betterFree( x , __FILE__ , __LINE__ )
#define calloc( x,y ) betterCalloc( x,y , __FILE__ , __LINE__ )
#define realloc( x,y ) betterRealloc( x,y , __FILE__ , __LINE__ )


#define blockSize 50000                 //blocksize allocation (can be changed to any affordable size)


typedef struct memEntry_T memEntry;
struct memEntry_T
{
    memEntry *next;
    memEntry *prev;
    int size;                           //size of allocated block
    int free;                           //tracks if free(1) or not (0)
    int signedPattern;                  // ADDED: use a signature/pattern to signify that the pointer is within memEntry structure

};


void *betterMalloc( unsigned int size, char * file, int line);
void betterFree(void * x, char * file, int line);
void * betterCalloc( int items, unsigned int size, char * file, int line);
void * betterRealloc(void * x, int size, char * file, int line);



#endif
