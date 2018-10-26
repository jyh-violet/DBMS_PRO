#include<stdio.h>  
#include<malloc.h>
#include<stdlib.h> 
#include<time.h>
#include "basic.h"
#define HEAP_INIT_SIZE 100 //堆结构存储空间的初始分配量
#define HEAPINCREMENT 10   //堆结构存储空间的分配增量

typedef struct block
{
	int iSize;  // 每个块的空闲空间的大小
	int iLocal; // 每个块的位置
}ElemType;

typedef struct Heap
{
	ElemType* heap; //堆结构的数组
	int iLength;     //堆结构的长度
	int iHeapSize;	//堆结构数组的大小
}HeapList;

void vInitHeapList(HeapList *L);
void vAddNodeToHeapList(HeapList *L,ElemType eNode); 
void vPrintHeapList(HeapList *L);
void vMaxHeap1(HeapList *L,int iPos);
void vMaxHeap2(HeapList *L,int iPos);
void vBuildMaxHeap(HeapList *L);
void vHeapSort(HeapList *L);
void vDeletNodeHeap(HeapList *L,int iPos);
int iFindNodeHeap(HeapList *L,int iLocal);
int iAllocateNodeHeap(HeapList *L, int iNeedspace, ElemType *eNode);
void vUpdateNodeHeap(HeapList *L, int iLocal, int iSizeNew);
void test();
void randomize();
