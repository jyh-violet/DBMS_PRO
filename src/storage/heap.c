/*
 * heap.c 
 * Created on: Oct 20, 2017
 * Author: yyl
 * Description: define the heap structure and achieve some operation
*/
#include "storage/heap.h"

/*
 * void vInitHeapList(HeapList *L) : by yyl
 * Create and initial the heap structure.
 * Parameter: HeapList *L point of heapstructure
 * return: void
*/
// 初始化堆结构 
void vInitHeapList(HeapList *L)  
{  
    	L->heap = (ElemType*)vmalloc( HEAP_INIT_SIZE * sizeof(ElemType));  //为堆空间分配内存
	L->iLength = 0;  //堆的长度
    	L->iHeapSize = HEAPINCREMENT; //堆空间列表的大小，初始为HEAPINCREMENT，空间不足是，按照HEAPINCREMENT增量分配
}

/*
 * void vPrintHeapList(HeapList *L) : by yyl
 * Print the heap structure.
 * Parameter: HeapList *L point of heapstructure
 * return: void
*/
// 打印排序表  输出堆结构
void vPrintHeapList(HeapList *L)  
{  
	printf("length = %d\n",L->iLength);   //先输出堆的长度 
	int i = 0;
    for(i = 1; i <= L->iLength; ++i)  
	{
		printf("%d", L->heap[i].iSize); //依次输出堆中每个节点（块）中空闲空间大小
		if(i!=L->iLength) 
		{
			printf("->");
		}
	}		
	printf("\n");  
}  

/*
 * void vMaxHeap1(HeapList *L,int iPos) : by yyl
 * Make the heap is tha max heap and make the small to be down
 * Parameter: 
	HeapList *L: point of heapstructure
	int iPos: position of node in the heap
 * return: void
*/
//堆结构保证最大化的操作1，小数下移
void vMaxHeap1(HeapList *L,int iPos)
{
	int iLeft = 2*iPos;
	int iRight = 2*iPos+1;
	int iLargest;
	if(iLeft<=L->iLength&&L->heap[iLeft].iSize > L->heap[iPos].iSize)
	{
		iLargest = iLeft;
	}
	else
	{
		iLargest = iPos;
	}
	if(iRight<=L->iLength&&L->heap[iRight].iSize > L->heap[iLargest].iSize)
	{
		iLargest = iRight;
	}
	if(iLargest != iPos)
	{
		L->heap[0] = L->heap[iPos];
		L->heap[iPos] = L->heap[iLargest];
		L->heap[iLargest] = L->heap[0];
		vMaxHeap1(L,iLargest);
	}	
}

/*
 * void vMaxHeap2(HeapList *L,int iPos) : by yyl
 * Make the heap is tha max heap and make the large to be up
 * Parameter: 
	HeapList *L: point of heapstructure
	int iPos: position of node in the heap
 * return: void
*/
//堆结构保证最大化的操作2，大数上移
void vMaxHeap2(HeapList *L,int iPos)
{
	int iParent;
	iParent = iPos/2;
	if(iParent == 0)
	{
		return; //到达根节点
	}
	if(L->heap[iParent].iSize < L->heap[iPos].iSize) //如果该节点的父亲比他小，就替换
	{
		L->heap[0] = L->heap[iPos];
		L->heap[iPos] = L->heap[iParent];
		L->heap[iParent] = L->heap[0];
		vMaxHeap2(L,iParent); //迭代调用
	}	
}

/*
 * void vBuildMaxHeap(HeapList *L) : by yyl
 * Make the heap is tha max heap 
 * Parameter: 
	HeapList *L: point of heapstructure
 * return: void
*/
void vBuildMaxHeap(HeapList *L)
{	
	int i=0;
	for(i=L->iLength/2;i>=1;i--)
	{
		vMaxHeap1(L,i);
	}
}

/*
 * vHeapSort(HeapList *L) : by yyl
 * Sort the array
 * Parameter: 
	HeapList *L: point of heapstructure
 * return: void
*/
void vHeapSort(HeapList *L)
{
	//BuildMaxHeap(L);
	int i=0;
	for(i=L->iLength;i>=1;i--)
	{
		L->heap[0] = L->heap[i];
		L->heap[i] = L->heap[1];
		L->heap[1] = L->heap[0];
		L->iLength = L->iLength-1;
		vMaxHeap1(L,1);
	}
}

/*
 * void vDeletNodeHeap(HeapList *L,int iPos): by yyl
 * Delete one node from Heap
 * Parameter: 
	Parameter: 
	HeapList *L: point of heapstructure
	int iPos: position of node in the heap which is the node to be deleted
 * return: void
*/
//堆结构删除一个节点，删除第i个位置的节点。
void vDeletNodeHeap(HeapList *L,int iPos)
{
	L->heap[0] = L->heap[iPos];
	L->heap[iPos] = L->heap[L->iLength];
	L->heap[L->iLength] = L->heap[0];
	L->iLength = L->iLength-1;
	vMaxHeap1(L,iPos);
	vMaxHeap2(L,iPos);
}

/*
 * void vAddNodeToHeapList(HeapList *L,ElemType eNode): by yyl
 * Add one node to Heap
 * Parameter: 
	HeapList *L: point of heapstructure
	ElemType eNode: the node to be added
 * return: void
*/
//堆结构增加一个节点
void vAddNodeToHeapList(HeapList *L,ElemType eNode)
{
	if(L->iLength >= L->iHeapSize)
	{//如果堆结构的内存空间不够则新分配内存空间
		ElemType* newbase = (ElemType* )realloc(L->heap,(L->iHeapSize+HEAPINCREMENT)*sizeof(ElemType));
		if(!newbase)
		{
			printf("mistake1: out of memory\n");
			return;
		}
		L->heap = newbase;
		L->iHeapSize += HEAPINCREMENT;
	}
	L->heap[L->iLength+1] = eNode; //将新节点添加到数组最后
	L->iLength += 1;
	vMaxHeap2(L,L->iLength); //调整新节点使得符合最大堆的性质
}

/*
 * int iFindNodeHeap(HeapList *L,int pos): by yyl
 * Give the iLocal of one Node, find the node and return the iPos of the node in heap 
 * Parameter: 
	HeapList *L: point of heapstructure
	int iLocal: the iLocal of node in the file
 * return: 
	int iPos: the pos of node in the heap
*/
//找出在堆中某个位置的块
int iFindNodeHeap(HeapList *L,int iLocal)
{
	int i=0;
	int iPos = 0;
	for(i=1;i<=L->iLength;i++)
	{
		if(iLocal == L->heap[i].iLocal)
		{
			iPos = i;
			return iPos;
		}
	}
	return -1;
} 

/*
 * int iAllocateNodeHeap(HeapList *L, int iNeedspace, ElemType *eNode): by yyl
 * Give the needspace of new tuple, find the block and return the eNode and iPos of the node in heap 
 * Parameter: 
	HeapList *L: point of heapstructure
	ElemType *eNode: the node to be find
	int iNeedspace: the needspace of new tuple
 * return: 
	int iPos: the pos of node in the heap
*/
//为所需空间needspace找到一个块，并返回其在堆中的位置
int iAllocateNodeHeap(HeapList *L, int iNeedspace, ElemType *eNode)
{	
	int iPos =1;
	int i=0;
	int iLeft = 0;
	int iRight = 0;
	int iRand_i;	
	for(i=1;i<=L->iLength;)
	{		
		iLeft = 2*i;
		iRight = 2*i+1;
		srand( (unsigned)time( NULL ) );
		iRand_i = rand()%2;
		if(iLeft > L->iLength )
		{
			iPos = i;
			break;
		}
		if(iRight > L->iLength && L->heap[iLeft].iSize < iNeedspace )
		{
			iPos = i;
			break;
		}
		if(iRight > L->iLength && L->heap[iLeft].iSize >= iNeedspace)
		{
			iPos = iLeft;
			break;	
		}
		if(L->heap[iLeft].iSize<iNeedspace && L->heap[iRight].iSize<iNeedspace)
		{
			iPos = i;
			break;
		}
		if(L->heap[iLeft].iSize>=iNeedspace && L->heap[iRight].iSize>=iNeedspace)
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
		if(L->heap[iLeft].iSize>=iNeedspace && L->heap[iRight].iSize<iNeedspace)
		{
			i = iLeft;
			continue;
		}
		if(L->heap[iLeft].iSize<iNeedspace && L->heap[iRight].iSize>=iNeedspace)
		{
			i = iRight;
			continue;
		}
		
	}
	*eNode = L->heap[iPos];
	ElemType eNodeNew = L->heap[iPos];
	eNodeNew.iSize -= iNeedspace;
	vDeletNodeHeap(L,iPos);
	vAddNodeToHeapList(L,eNodeNew);	
	return eNodeNew.iLocal;	
}

/*
 * int void vUpdateNodeHeap(HeapList *L, int iLocal, int iSizeNew): by yyl
 * Give the iLocal of the update block and the new size, update the heaps 
 * Parameter: 
	HeapList *L: point of heapstructure
	int iLocal: the block locak in file
	int iNewSize: the new size
 * return: void
*/
//当块进行更新后，得到块在文件中的位置和新的空闲空间大小，对堆进行更新操作
void vUpdateNodeHeap(HeapList *L, int iLocal, int iSizeNew)
{
	ElemType eNodeNew;
	eNodeNew.iLocal = iLocal;
	eNodeNew.iSize = iSizeNew;
	int iPos = iFindNodeHeap(L,iLocal);
	vDeletNodeHeap(L,iPos);
	vAddNodeToHeapList(L,eNodeNew);	
	return;	
}






