/*
 * block.h
 *
 *  Created on: Oct 22, 2017
 *      Author: jyh
 */
#ifndef FSM_H_
#define FSM_H_

#include "utils/mlog.h"
#include "storage/storage.h"
#include "storage/block.h"
#include "storage/file.h"

#define HEAP_INIT_SIZE 100 //堆结构存储空间的初始分配量
#define HEAPINCREMENT 10   //堆结构存储空间的分配增量
#define MIN_FreeSpace 8 // if freeSpaceHeap < 8, we will delete the node
#define FSM_ValidIndex(fsh, i) ((0 < i) && (i <= fsh->FSH_length))

typedef struct freeSpace
{
	uint16 freeSpaceSize;  // 每个块的空闲空间的大小
	BlockNumber blockNum; // 每个块的位置
}freeSpaceItem, *freeSpace;

typedef struct freeSpaceHeap
{
	freeSpace FSH_heap; //堆结构的数组
	uint FSH_length;     //堆结构的长度
	uint FSH_HeapSize;	//堆结构数组的大小
	uint FSH_maxBlockNum; //the max index of the block of the relation. used to get the number of blocks of a relation
}freeSpaceHeapHeader, *freeSpaceHeap;

freeSpaceHeap FSM_initFreeSpaceHeap();
freeSpaceHeap FSM_CreateHeapFromFile(File file);
void FSM_vFlushHeapToFile(freeSpaceHeap fsh, File file);
void FSM_vDestoryFreeSpaceHeap(freeSpaceHeap fsh);
void FSM_vHeapDeleteNode(freeSpaceHeap fsh, uint iPos);
void FSM_vAddNodeToHeap(freeSpaceHeap fsh, freeSpaceItem fsitem);
uint FSM_uFindNodeHeap(freeSpaceHeap fsh, BlockNumber bnum);
BlockNumber FSM_uAllocateNodeHeap(freeSpaceHeap fsh, uint Needspace);
void FSM_vUpdateItem(freeSpaceHeap fsh, BlockNumber bnum, uint16 newFreeSpace);
uint FSM_uHeapSort(freeSpaceHeap fsh);
void FSM_printHeap(freeSpaceHeap fsh);
void FSM_clearHeap(freeSpaceHeap fsh);
#endif /* FSM_H_ */
