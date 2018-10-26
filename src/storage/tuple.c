/*
 * tuple.c
 *
 *  Created on: Oct 17, 2017
 *      Author: jyh
 */
#include "storage/tuple.h"
/**
 * Tup_CreateTuple : by jyh
 * create a tuple and initial the necessary member
 * */
Tuple Tup_CreateTuple(Relation relation)
{
	vmlog(TUPLE_LOG, "--tuple.c/Tup_CreateTuple--create tuple start");
	Tuple tuple = vmalloc(sizeof(TupleData));
	tuple->TD_Relation = relation;
	tuple->TD_memtuple = NULL;
	tuple->TD_memtuple_len = 0;
	tuple->TD_heaptuple_exist = false;
	tuple->TD_memtuple_exist = false;
	Tup_ClearFlag(tuple);
	uint i = 0;
	tuple->TD_heaptuple = vmalloc0(Tup_GetHeapTupSize(tuple));
	for( i = 0; i < tuple->TD_Relation->Rel_attrs.item_num; i ++)
	{
		initHeapTupleItembyAttrData(tuple->TD_heaptuple + i, tuple->TD_Relation->Rel_attrs.item[i]);
	}
	vmlog(TUPLE_LOG, "--tuple.c/Tup_CreateTuple--create tuple end");
	return tuple;
}

/**
 * Tup_vClearTuple : by jyh
 * clear the content of the tuple.
 * the memory of the memtuple will be reset but not free
 * */
void Tup_vClearTuple(Tuple tuple)
{
	if(tuple->TD_memtuple != NULL)
	{
		memset(tuple->TD_memtuple, 0, tuple->TD_memtuple_len);
	}
	tuple->TD_memtuple_len = 0;
	Tup_ClearFlag(tuple);
//	vListDestroy(&(tuple->TD_heaptuple));
	Tup_vClearHeapTuple(tuple);
	tuple->TD_heaptuple_exist = false;
	tuple->TD_memtuple_exist = false;
//	vListInit((List*)&(tuple->TD_heaptuple));
}

void Tup_vClearHeapTuple(Tuple tuple)
{
	uint i = 0;
	for(i = 0; i < Tup_GetTupleAttributeNum(tuple); i++)
	{
		vDestroyHeapTupleItem(tuple->TD_heaptuple + i);
	}
//	memset(tuple->TD_heaptuple, 0 , Tup_GetHeapTupSize(tuple));
	tuple->TD_heaptuple_exist = false;
}

/**
 * Tup_vDestroyTuple : by jyh
 * */
void Tup_vDestroyTuple(Tuple tuple)
{
	vfree((void**)&(tuple->TD_memtuple));
	Tup_vDestoryHeaptup(tuple);
	vfree((void**)&tuple);
	tuple = NULL;
}

/**
 * Tup_vConstructHeaptup : by jyh
 * construct the tuple from the buffer read from the disk
 * */
void Tup_vConstructHeaptup(Tuple tuple, char* buffer)
{
	Tup_vClearHeapTuple(tuple);
//	vParseHeapTupleList(buffer, &(tuple->TD_Relation->Rel_attrs), (List*)&(tuple->TD_heaptuple));
	vParseHeapTuple(buffer, &(tuple->TD_Relation->Rel_attrs),tuple->TD_heaptuple);
	tuple->TD_heaptuple_exist = true;
	tuple->TD_memtuple_exist = false;
//	Tup_printHeapTuple(tuple);

}

void Tup_vDestoryHeaptup(Tuple tuple)
{
//	if(!bListEmpty((List*)&(tuple->TD_heaptuple)))
//	{
////		List node;
//		foreach(node, (tuple->TD_heaptuple))
//		{
//			vDestroyHeapTupleItem((HeapTupleItem*)node->cell);
//		}
//	}
	uint i = 0;
	for(i = 0; i < Tup_GetTupleAttributeNum(tuple); i++)
	{
		vDestroyHeapTupleItem(tuple->TD_heaptuple + i);
	}
	vfree((void**)&(tuple->TD_heaptuple));
	tuple->TD_heaptuple_exist = false;
//	vListDestroy((List*)&(tuple->TD_heaptuple));
}

/**
 * Tup_vGetMemTupleFromHeaptup : by jyh
 * */
void Tup_vGetMemTupleFromHeaptup(Tuple tuple)
{
	if(!tuple->TD_heaptuple_exist)
	{
		vmlog(WARNING, "--tuple.Tup_vGetMemTupleFromHeaptup -- no heaptuple exist");
	}
	Tup_vAllocateMemtuple(tuple);
	tuple->TD_memtuple_len  = uMem2Disk(tuple->TD_memtuple, tuple->TD_heaptuple, Tup_GetTupleAttributeNum(tuple));
	tuple->TD_memtuple_exist = true;
}

/**
 * Tup_vGetHeaptupleFromMemTup : by jyh
 * */
void Tup_vGetHeaptupleFromMemTup(Tuple tuple)
{
	if(!tuple->TD_memtuple_exist)
	{
		vmlog(WARNING, "--tuple.cTup_vGetHeaptupleFromMemTup -- no memtuple exist");
	}
	Tup_vClearHeapTuple(tuple);
	vmlog(TUPLE_LOG, "--tuple.c/Tup_vGetHeaptupleFromMemTup--start");
	vDisk2Mem(tuple->TD_memtuple, tuple->TD_heaptuple, &(tuple->TD_Relation->Rel_attrs));
	tuple->TD_heaptuple_exist = true;
}

/**
 * Tup_vAllocateMemtuple : by jyh
 * allocate the memory for the memtuple.
 * the size allocated is according to the max length of the tuple of the relation
 * */
void Tup_vAllocateMemtuple(Tuple tuple)
{
	if(tuple->TD_memtuple == NULL)
	{
		vmlog(WARNING, "--tuple.c/Tup_vAllocateMemtuple--allocate!!");
		tuple->TD_memtuple = vmalloc(tuple->TD_Relation->Rel_maxtuple_len);
		tuple->TD_memtuple_exist = false;
	}
}

/**
* Tup_vInsert : by jyh
 * insert a tuple into its relation
 * */
void Tup_vInsert(Tuple tuple)
{
	vmlog(TUPLE_LOG, "--tuple.c/Tup_vInsert-- tuple insert start");
	if(!tuple->TD_memtuple_exist)
	{
		Tup_vGetMemTupleFromHeaptup(tuple);
	}
	BlockNumber bnum = Rel_GetBlockToInsert(tuple->TD_Relation, tuple->TD_memtuple_len);
	vmlog(TUPLE_LOG, "--tuple.c/Tup_vInsert-- block number:%u", bnum);
	BufferPage bp = BLK_LoadBlock(bnum, tuple->TD_Relation,
			tuple->TD_Relation->Rel_file);
	vmlog(TUPLE_LOG, "--tuple.c/Tup_vInsert-- bufferPage:%u, pointer:%p",
			bp, BFM_GetBufferData(bp));
	Block block = BFM_GetBlockofBuffer(bp);
	BLK_vBlockInsert(block, tuple->TD_memtuple, tuple->TD_memtuple_len);
	BFM_SetBufferDirty(bp);
	BFM_vFlushBuffer(bp); //flush to disk
	vmlog(TUPLE_LOG, "--tuple.c/Tup_vInsert-- tuple insert end");
}

/**
* Tup_vAppend : by jyh
 * append a tuple to the end of the relation
 * */
void Tup_vAppend(Tuple tuple)
{
	vmlog(TUPLE_LOG, "--tuple.c/Tup_vAppend-- tuple insert start");
	if(!tuple->TD_memtuple_exist)
	{
		Tup_vGetMemTupleFromHeaptup(tuple);
	}
	BlockNumber bnum = tuple->TD_Relation->Rel_curMaxBlock;
	vmlog(TUPLE_LOG, "--tuple.c/Tup_vAppend-- block number:%u", bnum);
	BufferPage bp = BLK_LoadBlock(bnum, tuple->TD_Relation,
			tuple->TD_Relation->Rel_file);
	vmlog(TUPLE_LOG, "--tuple.c/Tup_vAppend-- bufferPage:%u, pointer:%p",
			bp, BFM_GetBufferData(bp));
	Block block = BFM_GetBlockofBuffer(bp);
	BLK_vBlockInsert(block, tuple->TD_memtuple, tuple->TD_memtuple_len);
	BFM_SetBufferDirty(bp);
	BFM_vFlushBuffer(bp); //flush to disk
	vmlog(TUPLE_LOG, "--tuple.c/Tup_vAppend-- tuple insert end");
}

/**
 * Tup_vReadTuple : by jyh
 * get a tuple into the memtuple according to its address.
 * the delete flag will be set if the tuple has been deleted in disk
 * */
void Tup_vReadTuple(Tuple tuple)
{
	vmlog(TUPLE_LOG, "--tuple.c/Tup_vReadTuple-- start");
	BlockNumber bnum = Tup_GetTupleBlockNum(tuple);
	BufferPage bp = BLK_LoadBlock(bnum, tuple->TD_Relation,
			tuple->TD_Relation->Rel_file);
	vmlog(TUPLE_LOG, "--tuple.c/Tup_vReadTuple--BlockNumber:%u bufferPage:%u, buffer:%p",
				bnum, bp, BFM_GetBufferData(bp));
	Block block = BFM_GetBlockofBuffer(bp);
//	vmlog(DISP_LOG, "buffer-- allocate buffer:%u to block:%u", bp, bnum);
	Tup_vAllocateMemtuple(tuple);
	BLK_vGetTupleFromBlock(block, tuple);
	Tup_vGetHeaptupleFromMemTup(tuple);
	vmlog(TUPLE_LOG, "--tuple.c/Tup_vReadTuple-- end");
}


/**
 * Tup_printHeapTuple : by jyh
 * print the tuple to the terminal
 * */
void Tup_printHeapTuple(Tuple tuple)
{
	printf("block:%u,index:%u--",
		(uint)tuple->TD_Addr.ta_blockNum,(uint)tuple->TD_Addr.ta_index);
	if(!tuple->TD_heaptuple_exist)
	{
		Tup_vGetHeaptupleFromMemTup(tuple);
	}
//	if(!bListEmpty((List*)&(tuple->TD_heaptuple)))
//	{
//		List node;
//		foreach(node, (tuple->TD_heaptuple))
//		{
//			vPrintHeapTupleItem((HeapTupleItem*)node->cell);
//		}
	uint i = 0;
	for(i = 0; i < Tup_GetTupleAttributeNum(tuple); i++)
	{
		vPrintHeapTupleItem(tuple->TD_heaptuple + i);
	}
//	}
	printf("\n");
}

/**
 * Tup_vSetAddr : by jyh
 * */
void Tup_vSetAddr(Tuple tuple, TupleAddr addr)
{
	tuple->TD_Addr = addr;
}

/**
 * Tup_cDeleteTuple : by jyh
 * delete a tuple.
 * the delete flag will be set for the tuple passed in
 * */
void Tup_cDeleteTuple(Tuple tuple)
{
	vmlog(TUPLE_LOG, "--tuple.c/Tup_cDeleteTuple-- start");
	BlockNumber bnum = Tup_GetTupleBlockNum(tuple);
	vmlog(TUPLE_LOG, "--tuple.c/Tup_cDeleteTuple-- bnum:%u, index:%u", bnum, Tup_GetTupleIndexInBlock(tuple));
	BufferPage bp = BLK_LoadBlock(bnum, tuple->TD_Relation, tuple->TD_Relation->Rel_file);
	Block block = BFM_GetBlockofBuffer(bp);
	BLK_vDeleteTupleFromBlock(block, tuple);
	BFM_SetBufferDirty(bp);
	BFM_vFlushBuffer(bp); //flush to disk
	vmlog(TUPLE_LOG, "--tuple.c/Tup_cDeleteTuple-- end");
}

/**
 * Tup_setAttr : by jyh
 * set the value of the given attribution of the tuple
 * **/
void Tup_setAttr(Tuple tuple, void* buffer, uint index)
{
	AttributeDataItem attribute = tuple->TD_Relation->Rel_attrs.item[index];
	setHeapTupleItemByData(tuple->TD_heaptuple + index, attribute.type, buffer, attribute.len);
	tuple->TD_heaptuple_exist = true;
	tuple->TD_memtuple_exist = false;
}

/**
 * Tup_setAttrByStr : by jyh
 * set the value of the given attribution of the tuple
 * for number 1, the *buffer = "1"
 * **/
void Tup_setAttrByStr(Tuple tuple, void* buffer, uint index)
{
	AttributeDataItem attribute = tuple->TD_Relation->Rel_attrs.item[index];
	setHeapTupleItemByStr(tuple->TD_heaptuple + index, attribute.type, buffer, attribute.len);
	tuple->TD_heaptuple_exist = true;
	tuple->TD_memtuple_exist = false;
}

HeapTupleItem Tup_getAttr(Tuple tuple, uint index)
{
	if(!tuple->TD_heaptuple_exist)
	{
		Tup_vGetHeaptupleFromMemTup(tuple);
	}
	return tuple->TD_heaptuple[index];
}
