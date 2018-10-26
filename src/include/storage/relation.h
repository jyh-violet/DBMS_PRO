/*
 * relation.h
 *
 *  Created on: Oct 17, 2017
 *      Author: jyh
 */

#ifndef RELATION_H_
#define RELATION_H_
#include "basic.h"
#include "utils/mlog.h"
#include "storage/storage.h"
#include "storage/buffer.h"
#include "storage/file.h"
#include "storage/block.h"
#include "storage/tuple.h"
#include "storage/fsm.h"
#include "storage/dataType.h"
#include "mparser/parseTable.h"

#define RelationNameMaxLength 100
#define DefaultMaxTupleLen 1024

typedef struct RelationData{
	char Rel_Name[100];
	File Rel_file;
	File Rel_fsmFile; // the file for free space management
	freeSpaceHeap Rel_fsh; //the free space management heap
	Oid Rel_rid;
	BlockNumber Rel_curBlock; //current block to insert the new item
	BlockNumber Rel_curMaxBlock; //current Max block number of this relation
	AttributeData Rel_attrs; // the attribute info list. it is the type and length information if needed of the attributes.
	uint Rel_maxtuple_len;// the max len of the memtuple. this can be calculated according to
	uint primary_key_attr_index;
}RelationData, *Relation;

Relation Rel_CreateRelation(char* relName, Oid rid);
Relation Rel_CreateTempRelation();
void Rel_vDestoryRelation(Relation rel);
BlockNumber Rel_AppendNewBlock(Relation rel, M_type blk_type);
BlockNumber Rel_GetBlockToInsert(Relation rel, uint len);
void Rel_vInitRelationAttrs(Relation rel, char* tableDDLfilename);
void Rel_vScanTable(Relation rel);
void Rel_vDeleteTuple(Relation rel);
uint Rel_findAttrIndex(Relation rel, char* attrName);
#endif /* RELATION_H_ */
