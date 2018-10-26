/*
 * sysinit.c
 *
 *  Created on: Nov 25, 2017
 *      Author: jyh
 */
#include "server/sysinit.h"
#include "test.h"
#include "exectutor/execTable.h"
/**
 * main : by jyh
 * the server process running background
 * */

void initSysTable()
{
	BFM_manager = BFM_CreateBufferManager();
	sysFile = CreateFile(sysFileName, 0, 0);

	sysTable = Rel_CreateRelation(NOTableName, SYS_TABLE_OID);
	Rel_vInitRelationAttrs(sysTable, SYS_TABLE_DEF);
	sysTableTuple = Tup_CreateTuple(sysTable);
	sysTableScan = vmalloc0(sizeof(SeqScanStateData));
	sysTableScan->scan.relation = sysTable;
	sysTableScan->scan.resultTuple = sysTableTuple;
	sysTableScan->scan.getnext = SeqScan_getNext;
	sysTableScan->scan.method = SeqScan;
	sysTableScan->buffer = INVALID_BUFFER;


	sysAttr = Rel_CreateRelation(NOTableName, SYS_ATTR_OID);
	Rel_vInitRelationAttrs(sysAttr, SYS_ATTR_DEF);
	sysAttrTuple = Tup_CreateTuple(sysAttr);
	sysAttrScan = vmalloc0(sizeof(SeqScanStateData));
	sysAttrScan->scan.relation = sysAttr;
	sysAttrScan->scan.resultTuple = sysAttrTuple;
	sysAttrScan->scan.getnext = SeqScan_getNext;
	sysTableScan->scan.method = SeqScan;
	sysAttrScan->buffer = INVALID_BUFFER;

	sysIndex = Rel_CreateRelation(NOTableName, SYS_INDEX_OID);
	Rel_vInitRelationAttrs(sysIndex, SYS_INDEX_DEF);
	sysIndexTuple = Tup_CreateTuple(sysIndex);
	sysIndexScan = vmalloc0(sizeof(SeqScanStateData));
	sysIndexScan->scan.relation = sysIndex;
	sysIndexScan->scan.resultTuple = sysIndexTuple;
	sysIndexScan->scan.getnext = SeqScan_getNext;
	sysTableScan->scan.method = SeqScan;
	sysIndexScan->buffer = INVALID_BUFFER;

	uint nowTable;
	ssize_t read = File_sRead(sysFile, &nowTable, sizeof(uint));
	if(read == sizeof(uint))
	{
		tableId = nowTable;
	}else
	{
		tableId = baseTableid;
	}

}

void destorySysTable()
{
	Tup_vDestroyTuple(sysTableTuple);
	Tup_vDestroyTuple(sysAttrTuple);
	Tup_vDestroyTuple(sysIndexTuple);

	Rel_vDestoryRelation(sysTable);
	Rel_vDestoryRelation(sysAttr);
	Rel_vDestoryRelation(sysIndex);

	BFM_vDestoryBufferManager(BFM_manager);
	File_sSeek(sysFile, 0);
	File_sWrite(sysFile, &tableId, sizeof(uint));
	vDestoryFile(sysFile);
}
