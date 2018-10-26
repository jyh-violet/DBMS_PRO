/*
 * test_fsm.c
 *
 *  Created on: Oct 21, 2017
 *      Author: jyh
 *      desc: compile: gcc -std=c99 -Isrc/include src/utils/* src/storage/* src/test/test_fsm.c -o test_fsm
 */

#include "storage/fsm.h"

int main()
{
	freeSpaceHeapHeader fsh = FSM_initFreeSpaceHeap();
	freeSpaceItem item;
	for(uint i = 0; i < 200; i ++)
	{
		item.blockNum = i;
		item.freeSpaceSize = i;
		FSM_vAddNodeToHeap(&fsh, item);
	}
	uint heap_len = FSM_uHeapSort(&fsh);
	for(uint i = 1; i < heap_len; i++)
	{
		vmlog(FSM_LOG, "fsh: index:%u, bnum:%u, freespacesize:%u",
				i, fsh.FSH_heap[i].blockNum, fsh.FSH_heap[i].freeSpaceSize);
	}
	FSM_vDestoryFreeSpaceHeap(&fsh);
	return 0;
}
