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
    "LteaM",
    /* First member's full name */
    "Louis Mutricy",
    /* First member's email address */
    "louis.mutricy@polytechnique.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    "",
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
void* end;
block* last;
int enableDefrag =1; // use to disable defragmentatiom operations during reallloc and for perf comparaisons
int part =1;// number of part in memory (ie 1+#malloc-#free)

block* getNext(block *b){
	block * bNext = b->next;
	int bInt = (int)bNext;
	bInt = bInt&(~1);
	return((block*) bInt);	
}
block * getFirst(){
	void *  first =mem_heap_lo();
	//if (verbose) printf("%d \n",(int ) block);
	return  (block*) first;
	
}
int isFree(block* b){
	if (b->next==NULL) return 0;
	int address = (int) b->next;
	//if (verbose) printf("%d \n",1 &address);
	return (1&address);

}

/* block structure (size,pointer next)*/
block * findNextFree(block* b){
	
	//unsigned char =mem_heap_lo(void);
	//block* b = getFirst();
	while ((!isFree(b))&&( (void*)b!=last)){
	// 	printf("trying %p\n",b);
		b = getNext(b);	
	}
	if (!isFree(b))return NULL;
//	printf("free block found : %p\n",(void*)b);
	return b;		
}
int getSize(block* b){
	if ((void *)getNext(b) ==end) {
		//printf("Warning : last Block");
		return(((char*)end-(char*)b)-8);
	}

	return (((char*)getNext(b))-((char*)b)-8);

}

block * getLast(){
	block* b = getFirst();
	while((void *)getNext(b)!=end){
		b = getNext(b);
	}
	return b;
}

// return address with flag 1 
block* setfree(block* b){
	int acc= (int)b;
	acc = acc |1;
	return ((block*)acc);
}
//remove flag or set it to 0
block* clear(block* b){
	int acc = (int)b;
	acc = acc & (~1);
	return ((block*)acc);


}

void increaseSize(int size){
	void* n =mem_sbrk(size);
	block * b =last;
	end =(void*)(((char*) mem_heap_hi())+1);

	//case 1 : last block is free. 
	if (isFree(b)){
	//	end = mem_heap_hi();
	//	end =(void*)(((char*) mem_heap_hi())+1);
		b->next = setfree(end);
		
	
	}
	//case 2 b is not free
	else {
		block* newb = (block*) n;
		last->next= newb;
		newb->next= setfree(end); 	
	}	 
		
	
}
void defragmente(block *b){
	//printf("test\n");	
	if (b>last)return;
	block * n=b;
        
	while ((n!=last)&&(isFree(n))){
               	 
		 n= getNext(n);
        	 b->next=setfree(n);
	}
	//printf("critical : n %p b: %p max: %p, bn: %p \n",n,b,mem_heap_hi(),(void*)getNext(b));
	

}

block * findFirstFree(int size){
	block * b =getFirst();
	b= findNextFree(b);
	block * defragmented = NULL; // last block defragmented
	while(b!=NULL){
		
		if (getSize(b)>=size) break;
		// special case where the last block is free but too small
		// pb : getNext(b) is out of bound
		if (b==last){
			b=NULL;
			break;
		}
		if(isFree(getNext(b))&&(defragmented!=b)){
			//printf("defragmenting at %p\n",b);
			defragmented =b;
			if (enableDefrag)defragmente(b);
		}
		else b=findNextFree(getNext(b));
	}
	if (b==NULL){
		increaseSize(size+8);
		return getLast();
	}
	return b;

}
// alternative strattegy to allocate  but seems less efficient 
block* findOneFree(int size){
	int found =0;
	block * result=NULL;
	block * b =getFirst();
	b= findNextFree(b);
	block * defragmented = NULL; // last block defragmented
	while(b!=NULL){
		
		if (getSize(b)>=size) {
			
			if (!found)result =b;
			else {
				if (getSize(result)>getSize(b))result=b;
			}
			found=1;
			if(getSize(b)<=2*size){
				result=b;
				break;
			}//only take small parts
		}
		// special case where the last block is free but too small
		// pb : getNext(b) is out of bound
		if (b==last){
			b=NULL;
			break;
		}
		if(isFree(getNext(b))&&(defragmented!=b)){
			//printf("defragmenting at %p\n",b);
			defragmented =b;
			if (enableDefrag)defragmente(b);
		}
		else b=findNextFree(getNext(b));
	}
	if (!found){
		increaseSize(size+8-getSize(last));
		return getLast();
	}
	return result;
	
}
//choose strategy ...
block * findOpt(int size){
//	if (part>100){
//		printf("part %d\n",part);
//		return findOneFree(size);
//	}
//	printf("%d\n",part);
	return findFirstFree(size);

}
void take(block *b,int size){
	//block*  nextblock=getNext(b);
	if(getSize(b)-size>8){
		block * newb = (block*)((char*)b+size+8);
		newb ->next =(block*)(((int)getNext(b))|1);
	 	b->next= (block*)(((int)newb)&(~1));
		if(b==last){
			last=newb;
		}
		return;
	}
	b->next= (block*)(((int)getNext(b))&(~1));
	
}

int mm_init(void)
{	
	void * first = mem_sbrk(16);	
	end =(void*)(((char*) mem_heap_hi())+1);
	block * b = (block*) first;
	b ->next = (block*) (((int) end)|1);
	last = getLast();
	return 0;
}



/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    //printf("Try to allocate %d\n",(int)size);
   //int newsize = ALIGN(size + SIZE_T_SIZE);
   int newsize =ALIGN(size+8);  

    
    block* b= findOpt(newsize);
    if (((char*)b+newsize)-((char*)mem_heap_hi())>0){
	printf("ran out of memory\n");
	return NULL;	
    }
    //printf("allocate at %p\n",(void*)b);
    take(b,newsize -8);
    part++;
    return((void*)((( char*) b)+8));
}

/*
 * mm_free - just set the flag to free
 */
void mm_free(void *ptr)
{
	if (ptr==NULL)return;
	part --;
	block *b = (block*)((char*)ptr-8);
	b->next= setfree(b->next);
	defragmente(b);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
	enableDefrag=0;	
	if (size==0){
		enableDefrag=1;
		mm_free(ptr);
		return NULL;
	}
	if (ptr==NULL){
		enableDefrag=1;
		return mm_malloc(size);
		
	}
	void *oldptr = ptr;
	void * newptr;
	size_t copySize;
	size_t oldSize;
	block * oldb = (block *)((char*)ptr - 8);
	oldSize =(size_t) getSize(oldb);
	mm_free(ptr);
	newptr = mm_malloc(size);
	if (newptr ==NULL){
		enableDefrag=1;
		printf("Realloc failed due to malloc failed \n");
		return NULL;
	}
	copySize = oldSize;
	if (copySize >size)copySize=size;
	
	
	memcpy(newptr,oldptr,copySize);
	enableDefrag=1;
	return newptr;
  
}














