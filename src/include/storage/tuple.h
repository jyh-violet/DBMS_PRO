/*
 * tuple.h
 *
 *  Created on: Oct 17, 2017
 *      Author: jyh
 */

#ifndef TUPLE_H_
#define TUPLE_H_
#include "basic.h"
#include "utils/mlog.h"
#include "storage/storage.h"
#include "storage/relation.h"
#include "storage/buffer.h"
#include "storage/file.h"
#include "storage/block.h"
#include "mparser/parseTuple.h"
#include "MemAndDisk.h"


typedef char* MemTuple;
typedef HeapTupleItem* HeapTuple; //this need to be implemented later

#define Tup_GetTupleBlockNum(tuple) (tuple->TD_Addr.ta_blockNum)
#define Tup_GetTupleIndexInBlock(tuple) (tuple->TD_Addr.ta_index)
#define Tup_GetTupleAttributeNum(tuple)  (tuple->TD_Relation->Rel_attrs.item_num)
#define Tup_GetHeapTupSize(tuple) 	(sizeof(HeapTupleItem) * Tup_GetTupleAttributeNum(tuple))

typedef struct TupleAddr{
	uint32   ta_blockNum:16, // the blockNumber of the block
			ta_index:16; // the index of the tuple inside the block
}TupleAddr;
#define TupleDeleteFalg 0x01 // the tuple is deleted or not
#define Tup_SetDeleteFlag(tuple) {tuple->TD_Flag |= TupleDeleteFalg;}
#define Tup_ClearDeleteFlag(tuple) {tuple->TD_Flag &= ~TupleDeleteFalg;}
#define Tup_CheckDeleteFlag(tuple) (tuple->TD_Flag & TupleDeleteFalg)
#define Tup_ClearFlag(tuple) 		{tuple->TD_Flag = 0;}
typedef struct TupleData{
	Relation TD_Relation;
	HeapTuple TD_heaptuple;//it is a list of the attributes of the tuple in fact. the type of the cell is HeapTupleItem*
	MemTuple TD_memtuple; //the connect will be written into the disk
	bool TD_heaptuple_exist;
	bool TD_memtuple_exist;
	uint TD_memtuple_len;
	TupleAddr TD_Addr; //the address of the tuple
	uint8 TD_Flag;
}TupleData, *Tuple;

void Tup_vInsert(Tuple tuple);
void Tup_vAppend(Tuple tuple);
void Tup_vGetMemTupleFromHeaptup(Tuple tuple);
void Tup_vGetHeaptupleFromMemTup(Tuple tuple);
//int Tup_iConstructTuple(Tuple tuple);
Tuple Tup_CreateTuple(Relation relation);
void Tup_vClearTuple(Tuple tuple);
void Tup_vClearHeapTuple(Tuple tuple);
void Tup_vDestroyTuple(Tuple tuple);
void Tup_vReadTuple(Tuple tuple);
void Tup_vSetAddr(Tuple tuple, TupleAddr addr);
void Tup_setAttr(Tuple tuple, void* buffer, uint index);
void Tup_setAttrByStr(Tuple tuple, void* buffer, uint index);
HeapTupleItem Tup_getAttr(Tuple tuple, uint index);
void Tup_cDeleteTuple(Tuple tuple);
void Tup_vConstructHeaptup(Tuple tuple, char* buffer);
void Tup_vAllocateMemtuple(Tuple tuple);
void Tup_printHeapTuple(Tuple tuple);
void Tup_vDestoryHeaptup(Tuple tuple);

#endif /* TUPLE_H_ */
