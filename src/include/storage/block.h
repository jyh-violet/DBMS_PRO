/*
 * block.h
 *
 *  Created on: Oct 17, 2017
 *      Author: jyh
 */

#ifndef BLOCK_H_
#define BLOCK_H_
#include "basic.h"
#include "utils/mlog.h"
#include "utils/hash.h"
#include "storage/storage.h"
#include "storage/relation.h"
#include "storage/buffer.h"
#include "storage/file.h"
#include "storage/tuple.h"

typedef ssize_t BlockAddr;
#define MaxBlockNumber 1024*1024
#define INValidBlockNumber (MaxBlockNumber + 1)


#define BlockItemOffsetSize 15 // <=4k, 15bit is enough
#define BlockItemOffsetOff 0
#define TupleItemFlagSize	2//<=4k,15bit is enough
#define TupleItemFlagOff	15
#define TupleItemLenSize	15//<=4k,15bit is enough
#define TupleItemLenOff		17
#define BlockItemInvalidFlag	0x01
#define IsValidBlockNumber(block) ((block) <= MaxBlockNumber)

#define BD_GetItemOffset(blockHeader, itemId) \
	(((blockHeader)->offsetTable[itemId]).bo_off)
#define BD_SetItemOffsetZero(blockHeader, itemId) \
	{((blockHeader)->offsetTable[itemId]).bo_off = 0;}
#define BD_SetItemOffset(blockHeader, itemId, off) \
	{((blockHeader)->offsetTable[itemId]).bo_off = (off);}
#define BD_CheckItemInValid(blockHeader, itemId) \
	(((((blockHeader)->offsetTable[itemId]).bo_flag)) & BlockItemInvalidFlag)
#define BD_SetItemInValid(blockHeader, itemId) \
	{((((blockHeader)->offsetTable[itemId]).bo_flag)) |= BlockItemInvalidFlag;}
#define BD_GetItemLen(blockHeader, itemId) \
		(((blockHeader)->offsetTable[itemId]).bo_len)
#define BD_GetBlockOffset(blockNumber) (blockNumber * BLOCKSIZE)
#define BLK_GetBlockHeader(block) ((BlockHeader)(block->blk_buffer))
#define BLK_GetBlockHeaderfromBuffer(buffer) ((BlockHeader)(BFM_GetBufferData(buffer)))
typedef struct BlockOffsetTableItem{
	uint32   bo_off:BlockItemOffsetSize, // the offset inside the block
			bo_flag:2, //0:normal, 1:deleted
			bo_len:TupleItemLenSize;//the length of the Item
}BlockOffsetTableItem;

typedef struct BlockData{
	char* blk_buffer;
	Relation blk_relation;
	BlockNumber blk_bnumber;
//	M_type blk_type; //block typle. the types of the block can be found from the basic.h
	File blk_file; // the file that the block belogs to
}BlockData, *Block;

//this struct reflects the content in the disk directly.
typedef struct BlockHeaderData{
	uint16 bhd_blockHeaderLen; //the length of the block header
	uint16 bhd_offTableLen; //the length of the offset table
	uint16 bhd_checksum;
	LocationIndex bhd_lower; // offset to the start of the free space
	LocationIndex bhd_upper; // offset to the end of the free space
	// the offset table. one for an item whose flag is not deleted.
	// that is the item will still in the offset table even if it is deleted.
	//but there is no guarantee that its content is still exist
	BlockOffsetTableItem offsetTable[FLEXIBLE_ARRAY_MEMBER];
}BlockHeaderData, *BlockHeader;


Oid BLK_GetBlockOid(BlockNumber bnum, Oid relOid, Oid fileOid);
Block BLK_CreateBlock(BlockNumber bnum, Relation rel, File file);
Block BLK_CreateBlockFromBuffer(BufferPage bp, Relation rel,  File file);
void BLK_vDestoryBlock(Block block);
BufferPage BLK_LoadBlock(BlockNumber bnum, Relation rel, File file);
ssize_t BLK_sReadBlock(Block block);
ssize_t BLK_sWriteBlock(Block block);
BlockOffsetTableItem CreateBlockOffsetTableItem(uint32 off, uint32 flag, uint32 len);
void BLK_vBlockInsert(Block block, char* buffer, uint len);
uint BLK_uBlockGetFreeSpaceSize(BlockHeader bhd);
bool BLK_bCheckEnoughSpace(BlockHeader bhd, uint len);
void BLK_vGetTupleFromBlock(Block block, Tuple tuple);
void BLK_vGetTupleFromBlockwithIndex(Block block, uint indexInBlock, Tuple tuple);
void BLK_vDeleteTupleFromBlock(Block block, Tuple tuple);
#endif /* BLOCK_H_ */
