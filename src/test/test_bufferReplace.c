/*
 * test_bufferReplace.c
 *
 *  Created on: Oct 24, 2017
 *      Author: jyh
 *     compile:gcc -std=c99 -Isrc/include src/utils/* src/storage/* src/parser/* src/test/test_bufferReplace.c -o test_bufferReplace.c
 */

#include "storage/relation.h"

int main()
{
	time_t t_start = time(NULL);
	char *dataName  = "src/data/supplier.tbl";
	char *tableName = "src/table/table";
	Oid tid = (Oid) 2;
	BFM_manager = BFM_CreateBufferManager();
	Relation relation = NULL;
	relation = Rel_CreateRelation("test", tid);
	Rel_vInitRelationAttrs(relation, tableName);
	//insert
	Tuple tuple = Tup_CreateTuple(relation);
	uint addrs[11][2]={
			{1,1},
			{2,1},
			{3,1},
			{2,2},
			{4,1},
			{5,1},
			{6,2},
			{2,1},
			{7,1},
			{6,1},
			{3,1}
	};
	TupleAddr addr;
	for(uint i = 0; i < 11; i ++)
	{
		addr.ta_blockNum = addrs[i][0];
		addr.ta_index = addrs[i][1];
		Tup_vSetAddr(tuple, addr);
		Tup_vReadTuple(tuple);
//		vmlog(DISP_LOG, "test--block:%u, index:%u,deleteflag:%d",
//				addr.ta_blockId, addr.ta_index, Tup_CheckDeleteFlag(tuple));
		if(!Tup_CheckDeleteFlag(tuple))
		{
			Tup_vGetHeaptupleFromMemTup(tuple);
			Tup_printHeapTuple(tuple);
		}
	}

	Tup_vDestroyTuple(tuple);
	Rel_vDestoryRelation(relation);
	relation = NULL;
	BFM_vDestoryBufferManager(BFM_manager);
	return 0;
}
