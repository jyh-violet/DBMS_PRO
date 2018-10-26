/*
 * fsm.c
 * Created on: Oct 20, 2017
 * Author: jyh
 * Description: define the heap structure and achieve some operation
*/
#include "storage/fsm.h"

/**
 * FSM_CreateHeapFromFile : by jyh
 * init the freeSpace heap.
 * when file is exist, we get the heap from the file.
 * else we create a new null heap, with the max size = HEAP_INIT_SIZE
 * FSH_heap[0] is not used any way
 * */
freeSpaceHeap FSM_CreateHeapFromFile(File file)
{
	uint heapsize = HEAP_INIT_SIZE,
			fshlength = 0,
			maxBlocknum = INValidBlockNumber;
	bool fileExist = File_Exist(file);
	if(fileExist)
	{
		File_sSeek(file, 0);
		File_sRead(file, (char*)(&heapsize), sizeof(uint));
		File_sRead(file, (char*)(&fshlength), sizeof(uint));
		File_sRead(file, (char*)(&maxBlocknum), sizeof(uint));
	}
	freeSpaceHeap fsh = vmalloc(sizeof(freeSpaceHeapHeader));
	fsh->FSH_heap = (freeSpaceItem*)vmalloc( heapsize * sizeof(freeSpaceItem));
	fsh->FSH_length = fshlength;
	fsh->FSH_HeapSize = heapsize;
	fsh->FSH_maxBlockNum = maxBlocknum;
	vmlog(DISP_LOG, "FSM_CreateHeapFromFile--fshlength:%u, heapsize:%u, maxBlockNum:%u",
			fshlength,
			heapsize,
			maxBlocknum);
	if(fileExist)
	{
		File_sRead(file, (char*)fsh->FSH_heap, fsh->FSH_HeapSize * sizeof(freeSpaceItem));
	}
	return fsh;
}


/**
 * FSM_vFlushHeapToFile : by jyh
 * save the free space heap into the file.
 * we save the heapsize and the fsh length first
 * */
void FSM_vFlushHeapToFile(freeSpaceHeap fsh, File file)
{
	vmlog(FSM_LOG, "--fsm.c/FSM_vFlushHeapToFile--start");
	File_sSeek(file, 0);
	File_sWrite(file, (char*)(&fsh->FSH_HeapSize), sizeof(uint));
	File_sWrite(file, (char*)(&fsh->FSH_length), sizeof(uint));
	File_sWrite(file, (char*)(&fsh->FSH_maxBlockNum), sizeof(uint));
	File_sWrite(file, (char*)fsh->FSH_heap, fsh->FSH_HeapSize * sizeof(freeSpaceItem));
}

/**
 * FSM_vDestoryFreeSpaceHeap : by jyh
 * free the space of the heap
 * */
void FSM_vDestoryFreeSpaceHeap(freeSpaceHeap fsh)
{
	if(fsh != NULL)
	{
		vfree((void**)&(fsh->FSH_heap));
	}
	vfree((void**)&fsh);
}

/**
 * vMaxHeapDown : by
 * put the little item down
 * */
void vMaxHeapDown(freeSpaceHeap fsh, uint iPos)
{
	uint iLeft = 2*iPos;
	uint iRight = 2*iPos+1;
	uint iLargest = iPos;
	if(iLeft<=fsh->FSH_length&&fsh->FSH_heap[iLeft].freeSpaceSize > fsh->FSH_heap[iPos].freeSpaceSize)
	{
		iLargest = iLeft;
	}
	if(iRight<=fsh->FSH_length&&fsh->FSH_heap[iRight].freeSpaceSize > fsh->FSH_heap[iLargest].freeSpaceSize)
	{
		iLargest = iRight;
	}
	if(iLargest != iPos)
	{
		fsh->FSH_heap[0] = fsh->FSH_heap[iPos];
		fsh->FSH_heap[iPos] = fsh->FSH_heap[iLargest];
		fsh->FSH_heap[iLargest] = fsh->FSH_heap[0];
		vMaxHeapDown(fsh, iLargest);
	}
}

/**
 * vMaxHeapUp : by jyh
 * put the larger item up
 * */
void vMaxHeapUp(freeSpaceHeap fsh, uint iPos)
{
	uint iParent;
	iParent = iPos/2;
//	vmlog(FSM_LOG, "--fsm.c/vMaxHeapUp-- pos:%u", iPos);
	if(iParent == 0)
	{
		return; //到达根节点
	}
	if(fsh->FSH_heap[iParent].freeSpaceSize < fsh->FSH_heap[iPos].freeSpaceSize) //如果该节点的父亲比他小，就替换
	{
		fsh->FSH_heap[0] = fsh->FSH_heap[iPos];
		fsh->FSH_heap[iPos] = fsh->FSH_heap[iParent];
		fsh->FSH_heap[iParent] = fsh->FSH_heap[0];
		vMaxHeapUp(fsh, iParent); //迭代调用
	}
}

/**
 * FSM_vHeapDeleteNode : by jyh
 * delete a node in the heap
 * */
void FSM_vHeapDeleteNode(freeSpaceHeap fsh, uint iPos)
{
	vmlog(FSM_LOG, "--fsm.c/FSM_vHeapDeleteNode--iPos:%u, block:%u", iPos, fsh->FSH_heap[iPos].blockNum);
	fsh->FSH_heap[0] = fsh->FSH_heap[iPos];
	fsh->FSH_heap[iPos] = fsh->FSH_heap[fsh->FSH_length];
	fsh->FSH_heap[fsh->FSH_length] = fsh->FSH_heap[0];
	fsh->FSH_length --;
	vMaxHeapDown(fsh, iPos);
	vMaxHeapUp(fsh, iPos);
}

/**
 * FSM_vAddNodeToHeap : by jyh
 * add a new block with freespace into the heap
 * */
void FSM_vAddNodeToHeap(freeSpaceHeap fsh, freeSpaceItem fsitem)
{
	if(fsh->FSH_length >= fsh->FSH_HeapSize - 2)
	{//如果堆结构的内存空间不够则新分配内存空间
		freeSpace newbase = (freeSpace )realloc(fsh->FSH_heap,(fsh->FSH_HeapSize + HEAPINCREMENT)*sizeof(freeSpaceItem));
		if(!newbase)
		{
			vmlog(ERROR, "fsm.c/FSM_vAddNodeToHeap realloc the new space error");
			return;
		}
		fsh->FSH_heap = newbase;
		fsh->FSH_HeapSize += HEAPINCREMENT;
	}
	if(fsitem.blockNum > fsh->FSH_maxBlockNum || !IsValidBlockNumber(fsh->FSH_maxBlockNum))
	{
		vmlog(FSM_LOG, "--fsm.c/FSM_vAddNodeToHeap--FSH_maxBlockNum change:%u", fsitem.blockNum);
		fsh->FSH_maxBlockNum = fsitem.blockNum;
	}
	vmlog(FSM_LOG, "--fsm.c/FSM_vAddNodeToHeap--bnum:%u, newFreeSpace:%u", fsitem.blockNum, fsitem.freeSpaceSize);
	fsh->FSH_heap[fsh->FSH_length + 1] = fsitem; //0 node is not used
	fsh->FSH_length ++;
	vMaxHeapUp(fsh,fsh->FSH_length); //调整新节点使得符合最大堆的性质
}

/**
 * FSM_uFindNodeHeap : by jyh
 * get the free space of the given block
 * ret : the index of the block inside the heap. 0 is returned is the block not exist
 * */
uint FSM_uFindNodeHeap(freeSpaceHeap fsh, BlockNumber bnum)
{
	uint iPos = 0;
	uint i = 0;
    for(i=1; i <= fsh->FSH_length; i++)
	{
		if(fsh->FSH_heap[i].blockNum == bnum)
		{
			iPos = i;
			return iPos;
		}
	}
	return 0;
}

/**
 * FSM_uAllocateNodeHeap : by jyh
 * find a proper block to get the needspace.
 * if found, the freespace of the chosen block will be reduced Needspace and rebuild the heap.
 * if not found, the  INValidBlockNumber will be returned
 *
 * */
BlockNumber FSM_uAllocateNodeHeap(freeSpaceHeap fsh, uint Needspace)
{
	uint iPos = 0;
	uint iLeft = 0;
	uint iRight = 0;
	uint iRand_i;
	BlockNumber bnum;
	if(fsh->FSH_heap[1].freeSpaceSize <= Needspace) //the max free space is smaller than need
	{
		return INValidBlockNumber;
	}
    uint i = 0;
	for(i=1; i<=fsh->FSH_length;)
	{
		iLeft = 2*i;
		iRight = 2*i+1;
		srand( (unsigned)time( NULL ) );
		iRand_i = rand()%2;
		if(iLeft > fsh->FSH_length )
		{
			iPos = i;
			break;
		}
		if(iRight > fsh->FSH_length && fsh->FSH_heap[iLeft].freeSpaceSize < Needspace )
		{
			iPos = i;
			break;
		}
		if(iRight > fsh->FSH_length && fsh->FSH_heap[iLeft].freeSpaceSize > Needspace)
		{
			iPos = iLeft;
			break;
		}
		if(fsh->FSH_heap[iLeft].freeSpaceSize <= Needspace && fsh->FSH_heap[iRight].freeSpaceSize <= Needspace)
		{
			iPos = i;
			break;
		}
		if(fsh->FSH_heap[iLeft].freeSpaceSize> Needspace && fsh->FSH_heap[iRight].freeSpaceSize > Needspace)
		{
			if(iRand_i == 0)
			{
				i = iLeft;
				continue;
			}
			else
			{
				i = iRight;
				continue;
			}
		}
		if(fsh->FSH_heap[iLeft].freeSpaceSize > Needspace && fsh->FSH_heap[iRight].freeSpaceSize <= Needspace)
		{
			i = iLeft;
			continue;
		}
		if(fsh->FSH_heap[iLeft].freeSpaceSize <= Needspace && fsh->FSH_heap[iRight].freeSpaceSize > Needspace)
		{
			i = iRight;
			continue;
		}

	}
	if(iPos == 0)
	{
		return INValidBlockNumber;
	}
	vmlog(FSM_LOG, "--fsm.c/FSM_uAllocateNodeHeap--oldFreeSpace:%u, newFreeSpace:%u,  heaplen:%u",
			fsh->FSH_heap[iPos].freeSpaceSize, fsh->FSH_heap[iPos].freeSpaceSize - Needspace, fsh->FSH_length);
	bnum =fsh->FSH_heap[iPos].blockNum;
	fsh->FSH_heap[iPos].freeSpaceSize -= Needspace;
	vmlog(FSM_LOG, "--fsm.c/FSM_uAllocateNodeHeap-- newFreeSpace:%u",
				fsh->FSH_heap[iPos].freeSpaceSize);
	if(fsh->FSH_heap[iPos].freeSpaceSize <= MIN_FreeSpace)
	{
		vmlog(FSM_LOG, "--fsm.c/FSM_uAllocateNodeHeap-- newFreeSpace:%u < MIN_FreeSpace, heaplen:%u",
						fsh->FSH_heap[iPos].freeSpaceSize, fsh->FSH_length);
		FSM_vHeapDeleteNode(fsh, iPos);
		vmlog(FSM_LOG, "--fsm.c/FSM_uAllocateNodeHeap-- after delete heaplen:%u",
				fsh->FSH_length);
	}else
	{
		vMaxHeapDown(fsh, iPos);
	}
	return bnum;
}

/**
 * FSM_vUpdateItem : by jyh
 * update the freespace of a given bolck
 * no use now
 * */
void FSM_vUpdateItem(freeSpaceHeap fsh, BlockNumber bnum, uint16 newFreeSpace)
{
	vmlog(FSM_LOG, "--fsm.c/FSM_vUpdateItem--bnum:%u, newFreeSpace:%u", bnum, newFreeSpace);
	uint index = FSM_uFindNodeHeap(fsh, bnum);
	vmlog(FSM_LOG, "--fsm.c/FSM_vUpdateItem--index:%u", index);
	if((newFreeSpace <= MIN_FreeSpace) && FSM_ValidIndex(fsh, index))
	{
		FSM_vHeapDeleteNode(fsh, index);
	}else if ((newFreeSpace <= MIN_FreeSpace) && (!FSM_ValidIndex(fsh, index)))
	{

	}else if(!FSM_ValidIndex(fsh, index))
	{
		if(bnum > fsh->FSH_maxBlockNum || !IsValidBlockNumber(fsh->FSH_maxBlockNum))
		{
			fsh->FSH_maxBlockNum = bnum;
		}
		freeSpaceItem fsitem = {newFreeSpace, bnum};
		FSM_vAddNodeToHeap(fsh, fsitem);
	}
	else if(fsh->FSH_heap[index].freeSpaceSize != newFreeSpace)
	{
		fsh->FSH_heap[index].freeSpaceSize = newFreeSpace;
		vMaxHeapDown(fsh, index);
		vMaxHeapUp(fsh, index);
	}
	vmlog(FSM_LOG, "--fsm.c/FSM_vUpdateItem-- end");
}

/**
 * FSM_uHeapSort : by jyh
 * put the heap item in up order.
 * ret : the length of the heap
 * */
uint FSM_uHeapSort(freeSpaceHeap fsh)
{
	//BuildMaxHeap(L);
	uint len = fsh->FSH_length;
	uint i = 0;
    for(i = fsh->FSH_length; i >= 1; i--)
	{
		fsh->FSH_heap[0] = fsh->FSH_heap[i];
		fsh->FSH_heap[i] = fsh->FSH_heap[1];
		fsh->FSH_heap[1] = fsh->FSH_heap[0];
		fsh->FSH_length --;
		vMaxHeapDown(fsh, 1);
	}
	return len;
}
/**
 * FSM_printHeap: by jyh
 * WARN: after calling this function ,the heap structure is destroyed to a sorted array
 * */
void FSM_printHeap(freeSpaceHeap fsh)
{
	uint heap_len = FSM_uHeapSort(fsh);
	vmlog(DISP_LOG, "fsh-- display free space heap:");
	vmlog(DISP_LOG, "fsh-- heap len:%u, block number:%u", heap_len, fsh->FSH_maxBlockNum);
	uint i = 0;
    for(i = 1; i < heap_len; i++)
	{
		vmlog(DISP_LOG, "fsh-- index:%u, bnum:%u, freespacesize:%u",
				i, fsh->FSH_heap[i].blockNum, fsh->FSH_heap[i].freeSpaceSize);
	}
}

void FSM_clearHeap(freeSpaceHeap fsh)
{
	fsh->FSH_length = 0;
}

