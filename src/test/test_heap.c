#include "storage/heap.h"


//test
void test()
{
		HeapList L;  
    		vInitHeapList(&L);	
	
		int iData1[] = {0,4,1,3,2,16,9,10,14,8,7};
		int iData2[] = {0,1,2,3,4,5,6,7,8,9,10};
		ElemType elem_list[10];
		int i = 0;		
		for(i=1;i<=10;i++)
		{
			elem_list[i].iSize = iData1[i];
			elem_list[i].iLocal = iData2[i];
		}
		for(i=1;i<=10;i++)
		{
			//printf("i= = %d, elem_list->iSize = %d, elem_list->iLocal = %d\n",i=,elem_list[i].iSize,elem_list[i].iLocal);
			vAddNodeToHeapList(&L,elem_list[i]);
		}
		vPrintHeapList(&L);
		vDeletNodeHeap(&L,2);
		vPrintHeapList(&L);
		ElemType eNode;
		int iPos = iFindNodeHeap(&L,5,&eNode);
		printf("%d,%d,%d\n",iPos,eNode.iSize,eNode.iLocal);
		iPos = iAllocateNodeHeap(&L,7,&eNode);
		printf("%d,%d,%d\n",iPos,eNode.iSize,eNode.iLocal);	
}

void main()  
{		
	test();
}  
