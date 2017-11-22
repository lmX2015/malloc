/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "evilteam",
    /* First member's full name */
    "Louis Mutricy",
    /* First member's email address */
    "louis.mutricy@polytechnique.edu",
    /* Second member's full name (leave blank if none) */
    "Jean Baptiste de Cagny",
    /* Second member's email address (leave blank if none) */
    "jean-baptiste.de-cagny@polytechnique.edu",
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* 
 * mm_init - initialize the malloc package.
 */
typedef struct block block;
struct block{
	block * next;
};
void* last;

block* getNext(block *b){
	block * bNext = b->next;
	int bInt = (int)bNext;
	bInt = bInt&(~7);
	return((block*) bInt);	
}
block * getFirst(){
	void *  first =mem_heap_lo();
	return  (block*) first;
	
}
int isFree(block* b){
	if (b->next==NULL) return 0;
	int address = (int) b->next;
	return (1&address);
}

/* block structure (size,pointer next)*/
block * findNextFree(){
	
	//unsigned char =mem_heap_lo(void);
	block* b = getFirst();
	while (!isFree(b)){
	 	b = getNext(b);	
	}
	return b;		
}
int getSize(block* b){
	if ((void *)getNext(b) ==last) {
		return((char*)last-(char*)b);
	}
	
	return (((char*)getNext(b))-((char*)b)-4);

}
block * getLast(){
	block* b = getFirst();
	while((void *)getNext(b)!=last){
		b = getNext(b);
		
	}
}

void increaseSize(int size){
	mem_sbrk(size);
	block * b =getLast;
	if (isFree(b)){
		
	}	 
		
	
}
int mm_init(void)
{	
	printf("init");
	void *  first =mem_heap_lo();
	last = mem_heap_hi();
	block * b = (block*) first;
	b ->next = (block*) last;
	mem_sbrk(32);
	//printf("%d\n", mem_heapsize());
	//printf("%d\n", getSize(getFirst()));
	
	return 0;
}



/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    //printf("Try to allocate");
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
  int address = (int) ((block*)ptr)->next;
  ((block*)ptr)->next = (block*) (1|address);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}














