/*
 * sysinit.h
 *
 *  Created on: Nov 25, 2017
 *      Author: jyh
 */

#ifndef SYSINIT_H_
#define SYSINIT_H_

#include "storage/relation.h"
#include "utils/tool.h"
#include "mparser/analyze.h"
#include "exectutor/execScan.h"

#define SYS_TABLE_OID 	0
#define SYS_ATTR_OID	1
#define SYS_INDEX_OID	2
#define SYS_TABLE_DEF 	"src/server/systable"
#define SYS_ATTR_DEF	"src/server/sysattr"
#define SYS_INDEX_DEF	"src/server/sysindex"

Relation sysTable, sysAttr, sysIndex; //system tables;
Tuple sysTableTuple, sysAttrTuple, sysIndexTuple;
SeqScanState sysTableScan, sysAttrScan, sysIndexScan;

#define sysFileName "system"
File sysFile;

#define baseTableid (256) //some id is reserved for the system
uint tableId; //the oid of the table next created

typedef enum sysTableAttr{
	sysTale_tblOid,
	sysTale_tblName
}sysTableAttr;

typedef enum sysAttrAttr{
	sysAttr_tblOid,
	sysAttr_tblName,
	sysAttr_attrName,
	sysAttr_attrType,
	sysAttr_attrLength,
	sysAttr_attrno
}sysAttrAttr;

typedef enum sysIndexAttr{
	sysIndex_indexOid,
	sysIndex_tblOid,
	sysIndex_tblName,
	sysIndex_indexName,
	sysIndex_indexattr,
	sysIndex_indextype
}sysIndexAttr;


void initSysTable();
void destorySysTable();

#endif /* SYSINIT_H_ */
