/*
 * buffer.c
 *
 *  Created on: Oct 12, 2017
 *      Author: jyh
 * */

#include "storage/buffer.h"

int iInitBufferManager(BufferManager bufm);
BufferPage FindFreeBuffer(BufferManager bufm);
BufferPage BufferReplace_Clock(BufferManager bufm);
BufferPage BufferReplace(BufferManager bufm);
bool BFM_checkBufferOverFlowItem(void* a, void* b);

/**
 * BFM_CreateBufferManager : by jyh
 * Create and initial the buffer manager.
 * ret: the pointer of the created buffer manager
 * */
BufferManager BFM_CreateBufferManager()
{
	BufferManager bufm = vmalloc(sizeof(BufferManagerData));
	int initRes = iInitBufferManager(bufm);
	if(initRes < 0)
	{
		vmlog(BUFFER_LOG, "--buffer.c/CreateBufferManager-- fail to init the buffer manager");
		return NULL;
	}
//	for(uint i = 0; i < BUFFER_MAX_COUNT; i ++)
//	{
//		vmlog(BUFFER_LOG, "--buffer.c/CreateBufferManager--i:%u, BufferHashNum:%u", i, bufm->BufferHashNum[i]);
//	}
	return bufm;

}

/**
 * iInitBufferManager : by jyh
 * Initial the buffer manager
 * ret: >=0:normal, <0: error
 * */
int iInitBufferManager(BufferManager bufm)
{
	memset(bufm, 0 , sizeof(BufferManagerData));
	int result = 0;
	bufm->bufferCount = 0;
	bufm->freeBufferCount = BUFFER_MAX_COUNT;
	uint i = 0;
    for (i = 0; i < BUFFER_MAX_COUNT; i++)
	{
		//0 is initialized as the last one,
		//and we get the indexes in the freeBuffer in the reverse order
		bufm->freeBuffer[i] = BUFFER_MAX_COUNT - 1 - i;
//		vmlog(BUFFER_LOG, "--buffer.c/iInitBufferManager-bufm->buffers[i].bufferIndex :%u", bufm->buffers[i].bufferIndex );
	}
	bufm->BufferReplaceStrategy = BufferReplace_Clock;
	for (i = 0; i < BUFFER_MAX_COUNT; i ++)
	{
		vListInit(bufm->BufferOverFlow + i);
	}
	return result;
}

/**
 * BFM_vDestoryBufferManager : by jyh
 * destroy the structure of the buffer manager.
 * this function is used for doing something in the future
 * */
void BFM_vDestoryBufferManager(BufferManager bufm)
{
	vmlog(BUFFER_LOG, "--buffer.c/BFM_vDestoryBufferManager--start");
	uint i = 0;
    for(i = 0; i < BUFFER_MAX_COUNT; i ++)
	{
		if(BFM_GetBlockofBuffer((BufferPage)i) != NULL) // still in use
		{
			BFM_vFreeBuffer((BufferPage)i);
		}
	}
	for(i = 0; i < BUFFER_MAX_COUNT; i ++)
	{
		if(!bListEmpty(&(BFM_manager->BufferOverFlow[i])))
		{
			vmlog(WARNING, "--buffer.c/BFM_vDestoryBufferManager--List is not empty");
	//		continue;
		}
		vListDestroy(&(BFM_manager->BufferOverFlow[i]));
		if(bufm->BufferHashNum[i] != 0)
		{
			vmlog(BUFFER_LOG, "--buffer.c/BFM_vDestoryBufferManager--i:%u, BufferHashNum:%u", i, bufm->BufferHashNum[i]);
		}
	}
	vmlog(BUFFER_LOG, "--buffer.c/BFM_vDestoryBufferManager--start");
	vfree((void**)&bufm);
}

/**
 * BFM_GetBuffer : by jyh
 * Get a buffer from the given buffer manager. THe buffer returned has been reset to 0.
 * ret: the pointer of the allocated buffer, NULL is returned if there is any error
 * */
BufferPage BFM_GetFreeBuffer(BufferManager bufm){
	BufferPage buf = FindFreeBuffer(bufm); //get a location in the buffer
	vmlog(BUFFER_LOG, "--buffer.c/BFM_GetFreeBuffer--bufIdex:%d", buf);
	if(!IsValidBuffer(buf))
	{
		return INVALID_BUFFER;
	}
//	vmlog(BUFFER_LOG, "--buffer.c/pGetBuffer--bufIdex:%u, pbuffer:%p", bufIndex, pbuffer);
	BFM_vInitBuffer(buf); //reset the buffer
//	vmlog(DISP_LOG, "buffer--allocate buffer: %u", buf);
	return buf;
}

/**
 * FindFreeBuffer : by jyh
 * find a free buffer form the given  buffer manager.
 * Replacement strategy would be applied if there is no free buffer.
 * ret: the index of the allocated buffer
 * */
BufferPage FindFreeBuffer(BufferManager bufm)
{
	BufferPage res = 0;
	if (bufm->freeBufferCount > 0)
	{
		bufm->freeBufferCount --;
		vmlog(BUFFER_LOG, "--buffer.c/FindFreeBuffer--allocate free buffer:%d",bufm->freeBuffer[bufm->freeBufferCount]);
		return bufm->freeBuffer[bufm->freeBufferCount];
	}else
	{
		//need to replace one buffer and get a free buffer
		res = BufferReplace(bufm);
	}
	return res;
}

/**
 * BFM_vFlushBuffer : by jyh
 * flush the given buffer page to the disk
 * */
void BFM_vFlushBuffer(BufferPage bp)
{
	vmlog(BUFFER_LOG, "--buffer.c/BFM_vFlushBuffer--buf:%d start",bp);
	Block block = BFM_GetBlockofBuffer(bp);
	if(BFM_BufferDirty(bp))
	{
		BLK_sWriteBlock(block);
		BFM_ClearBufferDirty(bp);
	}
//	FSM_vUpdateItem(block->blk_relation->Rel_fsh, block->blk_bnumber,
//			BLK_uBlockGetFreeSpaceSize(BLK_GetBlockHeaderfromBuffer(bp)));
	vmlog(BUFFER_LOG, "--buffer.c/BFM_vFlushBuffer--buf:%d end",bp);
}

/**
 * BFM_vFreeBuffer : by jyh
 * reset the Buffer to unused state
 * */
void BFM_vFreeBuffer(BufferPage bp)
{
	vmlog(BUFFER_LOG, "--buffer.c/BFM_vFreeBuffer--buf:%d start",bp);
	if(BFM_BufferDirty(bp))
	{
		BFM_vFlushBuffer(bp);
	}
	Block block = BFM_GetBlockofBuffer(bp);
	if(block != NULL)
	{
		HashValue hvalue =  Hash_GetHashValueforOid(block->blk_bnumber);
		uint divider = Hash_GetHashDividerforOid(block->blk_bnumber);
		BFM_manager->BufferHashNum[hvalue] --;

		//delete it from the BufferOverFlow list
		BufferOverFlowItemData targetFlow = {block->blk_relation->Rel_rid,
				divider,
				0};
		BufferOverFlowItem rmitem = pListRemove(&(BFM_manager->BufferOverFlow[hvalue]), &targetFlow, BFM_checkBufferOverFlowItem);
		vmlog(LINKLIST_LOG, "--buffer.c/BFM_vFreeBuffer--remove:relid:%u, divider:%u, rmitem:%p, BFM_manager->BufferOverFlow[%u]:%p",
				targetFlow.relOid, targetFlow.divider, rmitem, hvalue, BFM_manager->BufferOverFlow[hvalue]);
		vfree((void**)&rmitem);
		BLK_vDestoryBlock(BFM_GetBlockofBuffer(bp));
	}else
	{
		vmlog(WARNING, "ERROR!!--buffer.c/BFM_vFreeBuffer--no block is bind for the buffer:%d",bp);
	}

	BFM_vInitBuffer(bp);
	vmlog(BUFFER_LOG, "--buffer.c/BFM_vFreeBuffer--buf:%d end",bp);
}

/**
 * BFM_vInitBuffer : by jyh
 * initialize a buffer page
 * */
void BFM_vInitBuffer(BufferPage bp)
{
	vmlog(BUFFER_LOG, "--buffer.c/BFM_vInitBuffer--buf:%d, bufferbase:%p, pointer:%p",
			bp, BFM_manager->buffers, BFM_GetBufferData(bp));
	memset(BFM_GetBufferData(bp), 0, BUFFER_SIZE);
	BFM_ClearFlag(bp);
	BFM_ClearBlockofBuffer(bp);

}

BufferPage BufferReplace_Clock(BufferManager bufm)
{
	uint start = bufm->lastReplaceIndex + 1;
    uint i = 0;
//	vmlog(BUFFER_LOG, "--buffer.c/uBUfferReplace_Clock-start:%u", start);
	for(i = 0; i < BUFFER_MAX_COUNT * 2; i++)
	{
		uint index = (start + i ) % BUFFER_MAX_COUNT;
//		vmlog(BUFFER_LOG, "--buffer.c/uBUfferReplace_Clock-index:%u, bufm->BufferUse[index]:%d",
//				index, bufm->BufferUse[index]);
		if(!(BFM_BufferAccess(index)))
		{
			if(!(BFM_BufferPin(index)))
			{
				return index;
			}
		}else
		{
			BFM_ClearBufferAccess(index);
		}
	}
}

/**
 * BufferReplace ：jyh
 * replace a buffer. Flush it if needed
 * ret: the index of the replaced buffer
 * */
BufferPage BufferReplace(BufferManager bufm)
{
	BufferPage buffer;
	buffer = bufm->BufferReplaceStrategy(bufm);
	vmlog(BUFFER_LOG, "--buffer.c/BufferReplace-- replace buffer :%u", buffer);
	vmlog(BUFFER_LOG, "buffer-- replace buffer :%u", buffer);
	bufm->lastReplaceIndex = buffer;
	BFM_vFreeBuffer(buffer);
	return buffer;
}

/**
 * BFM_SearchExistBuffer : by jyh
 * search for the buffer for the  object with the given oid
 * ret : the buffer of the given object if it has been in the memory already, else, INVALID_BUFFER
 * */
BufferPage BFM_SearchExistBuffer(Oid oid, Oid rid)
{
	vmlog(BUFFER_LOG, "--buffer.c/BFM_SearchExistBuffer--start oid:%u, rid:%u", oid, rid);
	HashValue hvalue =  Hash_GetHashValueforOid(oid);
	if(BFM_manager->BufferHashNum[hvalue] == 0) // the block is not in the buffer
	{
		return INVALID_BUFFER;
	}
	uint divider = Hash_GetHashDividerforOid(oid);
	vmlog(BUFFER_LOG, "--buffer.c/BFM_SearchExistBuffer--divider:%u, hashvalue:%u, rid:%u", divider,hvalue, rid );
	BufferOverFlowItemData targetFlow = {rid, divider, 0};
	BufferOverFlowItem findResult =
			(BufferOverFlowItem)pListFind(&(BFM_manager->BufferOverFlow[hvalue]), &targetFlow, BFM_checkBufferOverFlowItem);
	if(findResult != NULL)
	{
		vmlog(BUFFER_LOG, "--buffer.c/BFM_SearchExistBuffer--exist:divider:%u, rid:%u", findResult->divider, findResult->relOid );
		BFM_SetBufferAccess(findResult->RealBuffer);
		return findResult->RealBuffer;
	}
//	for(uint i = 0; i < 5; i ++)
//	{
//		BufferOverFlowItemData overItem= BFM_manager->BufferOverFlow[hvalue][i];
//		if((overItem.relOid == rid) && (overItem.divider == divider))
//		{
//			return BFM_manager->BufferOverFlow[hvalue][i].RealBuffer;
//		}
//	}
	vmlog(BUFFER_LOG, "--buffer.c/BFM_SearchExistBuffer--end");
	return INVALID_BUFFER;
}

/**
 * BFM_sLoadBuffer: by jyh
 * load something from the disk into the buffer. the length to be read is the size of a block
 * off: the offset of the content
 * ret : the size of what has been read
 *
 * NOTE: no use, we implement the reading in block.c(BLK_sReadBlock)
 * */
ssize_t BFM_sLoadBuffer(BufferPage buffer, File file, ssize_t off)
{
	ssize_t read_size = 0;
	File_sSeek(file, off);
	read_size = File_sRead(file, (char*)BFM_GetBufferData(buffer), BLOCKSIZE);
	return read_size;
}

/**
 * BFM_vAddBufferHash : by jyh
 * add an item for the overfloe table
 * NOTE: this need to be change into list. it is not right now!
 * */
void BFM_vAddBufferHash(BufferPage buffer, Block block)
{
	Oid oid = BLK_GetBlockOid(block->blk_bnumber, block->blk_relation->Rel_rid, block->blk_file->fileId);
	HashValue hvalue =  Hash_GetHashValueforOid(oid);
	uint divider = Hash_GetHashDividerforOid(oid);
	vmlog(BUFFER_LOG, "--buffer.c/BFM_vAddBufferHash--hvalue:%u, divider:%u, blocknum:%u, oid:%u",
			hvalue, divider, block->blk_bnumber, oid);
	BFM_manager->BufferHashNum[hvalue] ++;
	BufferOverFlowItem item = vmalloc(sizeof(BufferOverFlowItemData));
	item->RealBuffer = buffer;
	item->divider = divider;
	item->relOid = block->blk_relation->Rel_rid;
	vListInsert(&(BFM_manager->BufferOverFlow[hvalue]), item);

}

bool BFM_checkBufferOverFlowItem(void* a, void* b)
{
	BufferOverFlowItem ba = (BufferOverFlowItem)a;
	BufferOverFlowItem bb = (BufferOverFlowItem)b;
	if((ba->relOid == bb->relOid) && (ba->divider == bb->divider))
	{
		return true;
	}
	else
	{
		return false;
	}

}
