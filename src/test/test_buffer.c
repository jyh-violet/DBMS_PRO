/*
 * test_buffer.c
 *
 *  Created on: Oct 13, 2017
 *      Author: jyh
 *      desc: compile: gcc -std=c99 -Isrc/include src/utils/* src/storage/* src/test/test_buffer.c -o test_buffer
 */
#include "storage/buffer.h"
int main()
{
	BFM_manager = BFM_CreateBufferManager();
	for(uint i = 0; i < 10; i ++)
	{
		BufferPage bp = BFM_GetFreeBuffer(BFM_manager);
		vmlog(BUFFER_LOG, "%d: GetBuffer:%d", i , bp);
	}
	BFM_manager->freeBufferCount = 0;
	BFM_SetBufferAccess((BufferPage)(1));
	BufferPage bp = BFM_GetFreeBuffer(BFM_manager);
	vmlog(BUFFER_LOG, "GetBuffer:%d", bp);
	for(uint i = 0; i < BUFFER_MAX_COUNT / 2; i ++)
	{
		BFM_SetBufferAccess((BufferPage)i);
	}
	bp = BFM_GetFreeBuffer(BFM_manager);
	vmlog(BUFFER_LOG, "GetBuffer:%d", bp);
	BFM_vDestoryBufferManager(BFM_manager);
	BFM_manager = NULL;
	return 0;
}
