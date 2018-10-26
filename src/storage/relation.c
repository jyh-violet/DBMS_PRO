/*
 * relation.c
 *
 *  Created on: Oct 17, 2017
 *      Author: jyh
 */
#include "storage/relation.h"

void vInitRelation(Relation rel);
void vgetTableFileName(Oid tid, char* filename);
void vgetTableFreeSapceFileName(Oid tid, char* filename);

/**
 * Rel_CreateRelation : by jyh
 * create a relation strcut according to its name and oid
 * */
Relation Rel_CreateRelation(char* relName, Oid rid)
{
	vmlog(RELATION_LOG, "--relation.c/Rel_CreateRelation--create relation start");
	Relation rel = vmalloc(sizeof(RelationData));
	memcpy(rel->Rel_Name,relName, strlen(relName) + 1);
	rel->Rel_rid = rid;
	vInitRelation(rel);
	vmlog(RELATION_LOG, "--relation.c/Rel_CreateRelation--create relation end");
	return rel;
}

/**
 * Rel_CreateTempRelation
 * create a temporary relation for the result.
 * it won't be allocated the files and has no name or id
 * */
Relation Rel_CreateTempRelation()
{
	Relation rel = vmalloc(sizeof(RelationData));
	rel->Rel_attrs.item_num = 0;
	return rel;
}

/**
 * Rel_vDestoryRelation : by jyh
 * */
void Rel_vDestoryRelation(Relation rel)
{
	if(rel->Rel_file != NULL)
	{
		vDestoryFile(rel->Rel_file);
		rel->Rel_file = NULL;
	}
	if(rel->Rel_fsmFile != NULL)
	{
		FSM_vFlushHeapToFile(rel->Rel_fsh, rel->Rel_fsmFile);
	}
	if(rel->Rel_fsh != NULL)
	{
		FSM_printHeap(rel->Rel_fsh);
		FSM_vDestoryFreeSpaceHeap(rel->Rel_fsh)	;
	}
	if(rel->Rel_fsmFile != NULL)
	{
		vDestoryFile(rel->Rel_fsmFile);
		rel->Rel_fsmFile = NULL;
	}
	rel->Rel_maxtuple_len = DefaultMaxTupleLen;
	vfree((void**)&rel);

}

/**
 * vInitRelation : by jyh
 * initialize the necessary information of the relation
 * */
void vInitRelation(Relation rel)
{
	vmlog(RELATION_LOG, "--relation.c/Rel_CreateRelation--init relation start");
	rel->Rel_curBlock = INValidBlockNumber;
	rel->primary_key_attr_index = INVALID_ATTR_INDEX;
	char filename[FILE_PATH_MAX_LEN];
	vgetTableFileName(rel->Rel_rid, filename);
	rel->Rel_file = CreateFile(filename, rel->Rel_rid, rel->Rel_rid * MaxFileNumforRelation);
	vgetTableFreeSapceFileName(rel->Rel_rid, filename);
	rel->Rel_fsmFile = CreateFile(filename, rel->Rel_rid, rel->Rel_rid * MaxFileNumforRelation + 1);
	rel->Rel_fsh = FSM_CreateHeapFromFile(rel->Rel_fsmFile);
	if(IsValidBlockNumber(rel->Rel_fsh->FSH_maxBlockNum))
	{
		rel->Rel_curMaxBlock = rel->Rel_fsh->FSH_maxBlockNum;
	}else
	{
		rel->Rel_curMaxBlock = INValidBlockNumber;
	}
	vmlog(RELATION_LOG, "--relation.c/Rel_CreateRelation--init relation maxBlock:%u end", rel->Rel_curMaxBlock);
}

void Rel_vInitRelationAttrs(Relation rel, char* tableDDLfilename)
{
	vParseTable(tableDDLfilename, &(rel->Rel_attrs));
	rel->Rel_maxtuple_len = uGetMaxTupleLen(&(rel->Rel_attrs));
	strcpy(rel->Rel_Name, rel->Rel_attrs.name);
}

/**
 * Rel_AddNewBlock : by jyh
 * append a new block filled with 0 into the given relation
 * block type is the same as the file type
 * */
BlockNumber Rel_AppendNewBlock(Relation rel, M_type blk_type)
{
	switch(blk_type)
	{
	case F_dataFile:
		vmlog(RELATION_LOG, "--relation.c/Rel_AddNewBlock--start, curMaxBlock:%u", rel->Rel_curMaxBlock);
		BlockNumber bnum;
		if(IsValidBlockNumber(rel->Rel_curMaxBlock))
		{
			bnum = rel->Rel_curMaxBlock + 1;
		}else
		{ //no block of this relation existed
			bnum = 0;
		}
		File_vAddBlock(rel->Rel_file, bnum);
		rel->Rel_curMaxBlock = bnum;
		rel->Rel_curBlock = bnum;
		freeSpaceItem fsitem = {(uint16)BLOCKSIZE - (uint16)sizeof(BlockHeaderData), bnum};
		FSM_vAddNodeToHeap(rel->Rel_fsh, fsitem);
		vmlog(RELATION_LOG, "--relation.c/Rel_AddNewBlock--end add:%u", bnum);
		return bnum;
	}
}

/**
 * Rel_GetBlockToInsert : by jyh
 * get a block with enough space of the  given relation
 * Note: now, we just handle the the content whose length less than BLOCKSIZE
 * len: the length of the conetnt that will be inserted
 * NOTE: this funcion is for the data block.
 * 		that is the free space management is used here to get the block
 *
 * */
BlockNumber Rel_GetBlockToInsert(Relation rel, uint len)
{
	vmlog(RELATION_LOG, "--relation.c/Rel_GetBlockToInsert-- start, curBlock:%d", rel->Rel_curBlock);
	Oid relOid = rel->Rel_rid;
	BufferPage bp  = INVALID_BUFFER;
	BlockNumber bnum = INValidBlockNumber;
	uint insertNeedLen = len + sizeof(BlockOffsetTableItem);
//	if(IsValidBlockNumber(rel->Rel_curBlock))
//	{
//		bp = BFM_SearchExistBuffer(BLK_GetBlockOid(rel->Rel_curBlock, relOid), relOid);
//	}
//	if(IsValidBuffer(bp))
//	{
//		BlockHeader bhd = BLK_GetBlockHeaderfromBuffer(bp);
//		if(BLK_bCheckEnoughSpace(bhd, len))
//		{
//			return rel->Rel_curBlock;
//		}
//	}
	bnum = FSM_uAllocateNodeHeap(rel->Rel_fsh, insertNeedLen);
	vmlog(RELATION_LOG, "--relation.c/Rel_GetBlockToInsert-- block from fsm :%u", bnum);
	if(! IsValidBlockNumber(bnum))
	{
		//no enough space in the exist block
		Rel_AppendNewBlock(rel, F_dataFile);
		 bnum = FSM_uAllocateNodeHeap(rel->Rel_fsh, insertNeedLen);
		 vmlog(RELATION_LOG, "--relation.c/Rel_GetBlockToInsert-- block from fsm after add block:%u", bnum);

	}
	vmlog(RELATION_LOG, "--relation.c/Rel_GetBlockToInsert-- end");
	rel->Rel_curBlock = bnum;
	return bnum;
}

/**
 * vgetTableFileName : by jyh
 * the file name is the oid of the table
 * tid: the oid of the table
 * filename: used to return the filename
 * */
void vgetTableFileName(Oid tid, char* filename)
{
	snprintf(filename, FILE_PATH_MAX_LEN, "%u", tid);
}

/**
 * vgetTableFreeSapceFileName : by jyh
 * */
void vgetTableFreeSapceFileName(Oid tid, char* filename)
{
	snprintf(filename, FILE_PATH_MAX_LEN, "%u.fsm", tid);
}

/**
 * Rel_vScanTable : by jyh
 * scan the table
 * */
void Rel_vScanTable(Relation rel)
{
	if(!IsValidBlockNumber(rel->Rel_curMaxBlock))
	{
		vmlog(WARNING, "---relation.c/Rel_vScanTable--table:%s, no data!", rel->Rel_Name);
		//no data
	//	Tup_vDestroyTuple(tuple);
		return;
	}
    uint i = 0;
	for(i = 0; i <= rel->Rel_curMaxBlock; i ++)
	{
		Tuple tuple = Tup_CreateTuple(rel);
		BufferPage bp = BLK_LoadBlock((BlockNumber)i, rel, rel->Rel_file);
		Block block = BFM_GetBlockofBuffer(bp);
		BlockHeader bhd = BLK_GetBlockHeader(block);
		uint j = 0;
        for(j = 0; j < bhd->bhd_offTableLen; j ++)
		{
			BLK_vGetTupleFromBlockwithIndex(block, j, tuple);
			vmlog(RELATION_LOG, "--relation.c/Rel_vScanTable--block:%u, index:%u,deleteflag:%d", i, j, Tup_CheckDeleteFlag(tuple));
			if(!Tup_CheckDeleteFlag(tuple))
			{
				Tup_vGetHeaptupleFromMemTup(tuple);
				Tup_printHeapTuple(tuple);
			}
		}
		Tup_vDestroyTuple(tuple);
	}
}

/**
 * Rel_vDeleteTuple : by jyh
 * delete the tuple whose index in block is even
 * */
void Rel_vDeleteTuple(Relation rel)
{
	Tuple tuple = Tup_CreateTuple(rel);
	uint i = 0;
    for(i = 0; i <= rel->Rel_curMaxBlock; i ++)
	{
		BufferPage bp = BLK_LoadBlock((BlockNumber)i, rel, rel->Rel_file);
		Block block = BFM_GetBlockofBuffer(bp);
		BlockHeader bhd = BLK_GetBlockHeader(block);
		uint j = 0;
        for(j = 0; j < bhd->bhd_offTableLen; j ++)
		{
			BLK_vGetTupleFromBlockwithIndex(block, j, tuple);
			if(!Tup_CheckDeleteFlag(tuple) && (j % 2 == 0))
			{
				vmlog(RELATION_LOG, "--relation.c/Rel_vDeleteTuple--delete:block:%u, index:%u", i, j);
				Tup_cDeleteTuple(tuple);
			}
		}
	}
	Tup_vDestroyTuple(tuple);
}

uint Rel_findAttrIndex(Relation rel, char* attrName)
{
	uint i = 0;
	for(i = 0; i < rel->Rel_attrs.item_num; i++)
	{
		if(strcmp(rel->Rel_attrs.item[i].key, attrName) == 0)
		{
			return i;
		}
	}
	return INVALID_ATTR_INDEX;
}
