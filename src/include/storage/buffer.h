/*
 * buffer.h
 *
 *  Created on: Oct 12, 2017
 *      Author: jyh
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include "utils/mlog.h"
#include "basic.h"
#include "utils/hash.h"
#include "storage/storage.h"
#include "storage/relation.h"
#include "storage/file.h"
#include "storage/block.h"
#include "storage/tuple.h"
#include "utils/LinkList.h"

#define BUFFER_SIZE BLOCKSIZE  // buffer size = page size = 4KB
#define BUFFER_MAX_COUNT 10240 // the whole number of the buffer
#define BufferAccessFlag 0x01 // used for replacement .When the buffer is accessed, it will be pined
#define BufferDirtyFlag 0x02 //buffer is changed or not
#define BufferPinFlag 0x04 //buffer is pined and can not be replaced
#define INVALID_BUFFER (BUFFER_MAX_COUNT + 1)
#define IsValidBuffer(b) ((b >= 0) && (b < BUFFER_MAX_COUNT))
struct BufferManagerData;
typedef char BufferData;
typedef struct BufferOverFlowItemData{
	Oid relOid; //the oid of the relation
	uint divider; //Id/ BUFFER_MAX_COUNT
	BufferPage RealBuffer; // the real buffer for the block
}BufferOverFlowItemData, *BufferOverFlowItem;

typedef struct BufferManagerData{
	BufferData buffers[BUFFER_MAX_COUNT][BUFFER_SIZE]; //the list of the allocated buffers
																																																									uint bufferCount; // the number of allocated buffers
	uint freeBuffer[BUFFER_MAX_COUNT];// the indexes of the free buffers
	uint BufferHashNum[BUFFER_MAX_COUNT]; // the number of blocks with the given hash value(buffer index) in memory
	uint8 BufferFlag[BUFFER_MAX_COUNT];

	//NOTE: if buffer search is needed, you should Create a BlockData struct for the given block ,
	//		and bind it with the buffer (use BFM_BindBufferAndBlock)
	Block buffer_block[BUFFER_MAX_COUNT]; //the block of the buffer
	uint freeBufferCount; //the number of free buffers
	uint lastReplaceIndex; // the index of the buffer replaced last time
	List BufferOverFlow[BUFFER_MAX_COUNT];
	BufferPage (*BufferReplaceStrategy)(struct BufferManagerData* bufm);
}BufferManagerData, *BufferManager;

BufferManager BFM_manager; //global

#define BFM_SetBufferAccess(b) {BFM_manager->BufferFlag[(b)] |= BufferAccessFlag;}
#define BFM_ClearBufferAccess(b) {BFM_manager->BufferFlag[(b)] &= ~(BufferAccessFlag);}
#define BFM_BufferAccess(b) (BFM_manager->BufferFlag[(b)] & BufferAccessFlag)

#define BFM_SetBufferDirty(b) {BFM_manager->BufferFlag[(b)] |= BufferDirtyFlag;}
#define BFM_ClearBufferDirty(b) {BFM_manager->BufferFlag[(b)] &= ~(BufferDirtyFlag);}
#define BFM_BufferDirty(b) (BFM_manager->BufferFlag[(b)] & BufferDirtyFlag)

#define BFM_SetBufferPin(b) {BFM_manager->BufferFlag[(b)] |= BufferPinFlag;}
#define BFM_ClearBufferPin(b) {BFM_manager->BufferFlag[(b)] &= ~(BufferPinFlag);}
#define BFM_BufferPin(b) (BFM_manager->BufferFlag[(b)] & BufferPinFlag)


#define BFM_ClearFlag(b) {BFM_manager->BufferFlag[(b)] = 0;}

//get the address of the buffer whose index is b
#define BFM_GetBufferData(b) (BFM_manager->buffers + (b))
#define BFM_BindBufferAndBlock(block, buffer) \
	{ \
		BFM_manager->buffer_block[(buffer)] = (Block)(block); \
		((Block)(block))->blk_buffer = (void*)BFM_GetBufferData((buffer)); \
	}
#define BFM_GetBlockofBuffer(b) (BFM_manager->buffer_block[(b)])
#define BFM_ClearBlockofBuffer(b) {BFM_manager->buffer_block[(b)] = NULL;}

BufferManager BFM_CreateBufferManager();
void BFM_vDestoryBufferManager(BufferManager bufm);
BufferPage BFM_GetFreeBuffer(BufferManager bufm);
BufferPage FindFreeBuffer(BufferManager bufm);
void BFM_vFlushBuffer(BufferPage bp);
void BFM_vFreeBuffer(BufferPage bp);
void BFM_vInitBuffer(BufferPage bp);
BufferPage BFM_SearchExistBuffer(Oid oid, Oid rid);
ssize_t BFM_sLoadBuffer(BufferPage buffer, File file, ssize_t off);
void BFM_vAddBufferHash(BufferPage buffer, Block block);

#endif /* BUFFER_H_ */
