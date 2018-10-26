/*
 * block.c
 *
 *  Created on: Oct 17, 2017
 *      Author: jyh
 */
#include "storage/block.h"


/**
 * BLK_GetBlockOid : by jyh
 * get the oid of the given block in the given relation
 * need to be changed
 * NOTE: the block oid of the same block cloud be changed if
 *
 * */
Oid BLK_GetBlockOid(BlockNumber bnum, Oid relOid, Oid fileOid)
{
	return (Oid)(bnum + fileOid * INValidBlockNumber);
}

/**
 * BLK_CreateBlock : by jyh
 * there is no need to malloc the buffer. Because the buffer will be allocated by the buffer manager
 * NOTE: the file that the block belongs to should exists.
 * */
Block BLK_CreateBlock(BlockNumber bnum, Relation rel, File file)
{
	Block block = vmalloc(sizeof(BlockData));
	block->blk_buffer = NULL;
	block->blk_bnumber = bnum;
	block->blk_relation = rel;
	block->blk_file = file;
}

/**
 * BLK_GetBlockFromBuffer : by jyh
 * construct a block for the given buffer, and set its relation
 * */
Block BLK_CreateBlockFromBuffer(BufferPage bp, Relation rel, File file)
{
	Block block= vmalloc(sizeof(BlockData));
	BFM_BindBufferAndBlock(block, bp);
	block->blk_relation = rel;
	block->blk_file = file;
	return block;
}

/**
 * BLK_vDestoryBlock : by jyh
 * free the memory of the block.
 * NOTE: there is no need to free the buffer of the block. Because the buffer is used circularly
 * */
void BLK_vDestoryBlock(Block block)
{
	if(block != NULL)
	{
		vmlog(BLOCK_LOG, "--block.c/BLK_vDestoryBlock BlockNumber:%u start", block->blk_bnumber);
		vfree((void**)&block);
	}
	vmlog(BLOCK_LOG, "--block.c/BLK_vDestoryBlock end");
}

/**
 * BLK_LoadBlock : by jyh
 * load the content of the given block into a buffer
 * ret : the buffer contain the content of the block
 * */
BufferPage BLK_LoadBlock(BlockNumber bnum, Relation rel, File file)
{
	vmlog(BLOCK_LOG, "--block.c/BLK_LoadBlock BlockNumber:%u start", bnum);
	Oid relOid = rel->Rel_rid;
	BufferPage bp = BFM_SearchExistBuffer(BLK_GetBlockOid(bnum, relOid, file->fileId), relOid);
	if(IsValidBuffer(bp)) //already in memory
	{
		vmlog(BLOCK_LOG, "--block.c/BLK_LoadBlock buffer exist:%u", bp);
		return bp;
	}
	//find a buffer for this block
	bp = BFM_GetFreeBuffer(BFM_manager);
	if(!IsValidBuffer(bp))
	{
		vmlog(ERROR, "ERROR--block.c/BLK_LoadBlock--get buffer fail!");
		return bp;
	}
	Block block = BLK_CreateBlock(bnum, rel, file);
	vmlog(BLOCK_LOG, "--block.c/BLK_LoadBlock relname:%s end", block->blk_relation->Rel_Name);
	BFM_BindBufferAndBlock(block, bp);
	BFM_vAddBufferHash(bp, block);
	// load the content into the buffer from disk
	BLK_sReadBlock(block);
	vmlog(BLOCK_LOG, "--block.c/BLK_LoadBlock BlockNumber:%u end", bnum);
	return bp;
}

/**
 * BLK_sReadBlock : by jyh
 * */
ssize_t BLK_sReadBlock(Block block)
{
	vmlog(BLOCK_LOG, "--block.c/BLK_sReadBlock BlockNumber:%u start", block->blk_bnumber);
	ssize_t read_size = 0;
	ssize_t off = BD_GetBlockOffset(block->blk_bnumber);
	File file = block->blk_file;
	File_sSeek(file, off);
	read_size = File_sRead(file, block->blk_buffer, BLOCKSIZE);
	vmlog(BLOCK_LOG, "--block.c/BLK_sReadBlock buffer:%p, BlockNumber:%u off:%u read_size:%ld  end",
			block->blk_buffer, block->blk_bnumber, off, read_size);
	return read_size;
}

/**
 * BLK_sWriteBlock : by jyh
 * */
ssize_t BLK_sWriteBlock(Block block)
{
	ssize_t write_size = 0;
	ssize_t off = BD_GetBlockOffset(block->blk_bnumber);
	File file = block->blk_file;
	ssize_t file_off = File_sSeek(file, off);
	vmlog(BLOCK_LOG, "--block.c/BLK_sWriteBlock buffer:%p,%p, %hd ",
				block->blk_buffer, (block->blk_buffer + 8), *(char*)(block->blk_buffer + 8));
	write_size = File_sWrite(file, block->blk_buffer, BLOCKSIZE);
	return write_size;
}

/**
 * CreateBlockOffsetTableItem : by jyh
 * */
BlockOffsetTableItem CreateBlockOffsetTableItem(uint32 off, uint32 flag, uint32 len)
{
	BlockOffsetTableItem item;
//	uint32 test;
//	vmlog(BLOCK_LOG, "--block.c/BlockOffsetTableItem--off:%u, flag:%u, len:%u",
//			off << BlockItemOffsetOff, flag, len << TupleItemLenOff);
//	test = (off << BlockItemOffsetOff) + (flag << TupleItemFlagOff )+ (len << TupleItemLenOff);
//	memcpy(&item, &test, sizeof(BlockOffsetTableItem));
	item.bo_flag = flag;
	item.bo_len = len;
	item.bo_off = off;
	vmlog(BLOCK_LOG, "--block.c/BlockOffsetTableItem-- item:%u", item);
	return item;
}

/**
 * BLK_vBlockInsert : by jyh
 * insert the given content in the buffer into the block
 * len: the length to be insert
 * */
void BLK_vBlockInsert(Block block, char* buffer, uint len)
{
	vmlog(BLOCK_LOG, "--block.c/BLK_vBlockInsert start BlockNumber:%u len:%u", block->blk_bnumber, len);
	BlockHeader bhd = BLK_GetBlockHeader(block);
	if(!BLK_bCheckEnoughSpace(bhd,len))
	{
		//this may be happen for update!!
		vmlog(WARNING, "WARNING--block.c/BLK_vBlockInsert--block insert error! there is not enough space in the block, "
						"blockspace:%u, insert len:%u", BLK_uBlockGetFreeSpaceSize(bhd), len);
		BlockNumber bnum = Rel_AppendNewBlock(block->blk_relation, block->blk_file->fileType);
		BufferPage bp = BLK_LoadBlock(bnum, block->blk_relation,
				block->blk_file);
		block = BFM_GetBlockofBuffer(bp);
		bhd = BLK_GetBlockHeader(block);
		vmlog(WARNING, "WARNING--block.c/BLK_vBlockInsert--insert a new block:%u, buffer:%u, new space:%u", bnum, bp, BLK_uBlockGetFreeSpaceSize(bhd));
	}
	uint32 offset = bhd->bhd_upper - len; //the start place of the content
	vmlog(BLOCK_LOG, "--block.c/BLK_vBlockInsert--->bhd_upper:%u, offset:%u, len:%u", bhd->bhd_upper, offset, len);
	uint32 flag = 0;
	BlockOffsetTableItem offItem = CreateBlockOffsetTableItem(offset, flag, (uint32)len);
	uint16 newoffTableLen = bhd->bhd_offTableLen + 1;
	LocationIndex new_lower = bhd->bhd_lower + (uint16)sizeof(BlockOffsetTableItem);
	LocationIndex new_upper = bhd->bhd_upper - (uint16)len;
	uint16 newheaderLen = bhd->bhd_blockHeaderLen + (uint16)sizeof(BlockOffsetTableItem);
	vmlog(BLOCK_LOG, "--block.c/BLK_vBlockInsert offItem:%u", offItem);
	memcpy(block->blk_buffer + offset, buffer, len); //copy the buffer into the block
	bhd->offsetTable[bhd->bhd_offTableLen] = offItem; //set offset table
	bhd->bhd_offTableLen = newoffTableLen;
	bhd->bhd_blockHeaderLen = newheaderLen;
	bhd->bhd_lower = new_lower;
	bhd->bhd_upper = new_upper;
	vmlog(BLOCK_LOG, "--block.c/BLK_vBlockInsert bhd->bhd_upper:%p bhd->bhd_upper:%hd "
			"bhd->offsetTable:%p bhd->offsetTable:%u",
			&(bhd->bhd_upper), *(&(bhd->bhd_upper)),
			bhd->offsetTable, *(bhd->offsetTable));
	vmlog(BLOCK_LOG, "--block.c/BLK_vBlockInsert BlockNumber:%u end", block->blk_bnumber);
}

/**
 * BLK_uBlockGetFreeSpaceSize : by jyh
 * */
uint BLK_uBlockGetFreeSpaceSize(BlockHeader bhd)
{
	return bhd->bhd_upper - bhd->bhd_lower;
}

/**
 * BLK_bCheckEnoughSpace : by jyh
 * check whether there is enough space in the block to insert some content with length of len.
 * the size of the offset table item is considered
 * */
bool BLK_bCheckEnoughSpace(BlockHeader bhd, uint len)
{
	// the content itself and its offset item
	uint needSpace = len + sizeof(BlockOffsetTableItem);
	return (needSpace < BLK_uBlockGetFreeSpaceSize(bhd));
}

/**
 * BLK_vGetTupleFromBlock : by jyh
 * extract the tuple from the block.
 * the delete flag will be set if the tuple has been deleted in disk.
 * */
void BLK_vGetTupleFromBlock(Block block, Tuple tuple)
{
	vmlog(BLOCK_LOG, "--block.c/BLK_vGetTupleFromBlock-- start");
	BlockHeader bhd = BLK_GetBlockHeader(block);
	uint indexInBlock = Tup_GetTupleIndexInBlock(tuple);
	if(BD_CheckItemInValid(bhd, indexInBlock))
	{
		vmlog(BLOCK_LOG, "--block.c/BLK_vGetTupleFromBlock--bhd:%p, bhd->bhd_upper:%u,"
					"indexInBlock:%u  offsetItem:%u offtablelen:%u end",
					bhd,bhd->bhd_upper, indexInBlock,  *(bhd->offsetTable), bhd->bhd_offTableLen);
		Tup_SetDeleteFlag(tuple);
		return;
	}
	uint offsetInBlock = BD_GetItemOffset(bhd, indexInBlock);
	uint tupleLen = BD_GetItemLen(bhd, indexInBlock);
	memcpy(tuple->TD_memtuple, block->blk_buffer + offsetInBlock, tupleLen);
	tuple->TD_memtuple_len = tupleLen;
	tuple->TD_memtuple_exist = true;
	tuple->TD_heaptuple_exist = false;
	vmlog(BLOCK_LOG, "--block.c/BLK_vGetTupleFromBlock--bhd:%p, bhd->bhd_upper:%u,"
			"indexInBlock:%u offsetInBlock:%u tupleLen:%u offsetItem:%u end",
			bhd,bhd->bhd_upper, indexInBlock, offsetInBlock, tupleLen, *(bhd->offsetTable));

}

/**
 * BLK_vGetTupleFromBlockwithIndex : by jyh
 * get the tuple according to the block number and the index in block
 *
 * */
void BLK_vGetTupleFromBlockwithIndex(Block block, uint indexInBlock, Tuple tuple)
{
	vmlog(BLOCK_LOG, "--block.c/BLK_vGetTupleFromBlockwithIndex-- start");
	Tup_vClearTuple(tuple);
	BlockHeader bhd = BLK_GetBlockHeader(block);
	TupleAddr addr;
	addr.ta_blockNum= (uint16)block->blk_bnumber;
	addr.ta_index = indexInBlock;
	Tup_vSetAddr(tuple, addr);
	if(BD_CheckItemInValid(bhd, indexInBlock))
	{
		Tup_SetDeleteFlag(tuple);
		return;
	}
	uint offsetInBlock = BD_GetItemOffset(bhd, indexInBlock);
	uint tupleLen = BD_GetItemLen(bhd, indexInBlock);
	Tup_vAllocateMemtuple(tuple);
	memcpy(tuple->TD_memtuple, block->blk_buffer + offsetInBlock, tupleLen);
	tuple->TD_memtuple_len = tupleLen;
	tuple->TD_memtuple_exist = true;
	tuple->TD_heaptuple_exist = false;
	vmlog(BLOCK_LOG, "--block.c/BLK_vGetTupleFromBlockwithIndex-- bhd->bhd_upper:%u,"
			"indexInBlock:%u offsetInBlock:%u tupleLen:%u end",
			bhd->bhd_upper, indexInBlock, offsetInBlock, tupleLen);

}

/**
 * BLK_vDeleteTupleFromBlock : by jyh
 * set the delete flag for the tuple in the disk
 * */
void BLK_vDeleteTupleFromBlock(Block block, Tuple tuple)
{
	vmlog(BLOCK_LOG, "--block.c/BLK_vDeleteTupleFromBlock-- start bnum:%u, index:%u",
			Tup_GetTupleBlockNum(tuple), Tup_GetTupleIndexInBlock(tuple));
	BlockHeader bhd = BLK_GetBlockHeader(block);
	uint indexInBlock = Tup_GetTupleIndexInBlock(tuple);
	BD_SetItemInValid(bhd, indexInBlock);
	vmlog(BLOCK_LOG, "--block.c/BLK_vDeleteTupleFromBlock-- end");
}
