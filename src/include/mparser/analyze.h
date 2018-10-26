/*
 * analyze.h
 *
 *  Created on: Nov 27, 2017
 *      Author: jyh and bsl
 */

#ifndef ANALYZE_H_
#define ANALYZE_H_
#include "basic.h"
#include "storage/relation.h"
#include "grammer.h"
#include "utils/expr.h"
#include "storage/tempFile.h"

typedef enum StateTag{
	State_CreateTable,
	State_CreateIndex,
	State_DropTable,
	State_DropIndex,
	State_InsertTable,
	State_DeleteTable,
	State_UpdateTable,
	State_Scan,
	State_Join,
	State_Select,
	State_Projection
}StateTag;

typedef enum IndexTag{
	Index_CreateIndex,
	Index_DropIndex,
	Index_DropTable,
	Index_InsertTuple,
	Index_DeleteTuple,
	Index_UpdateTuple
}IndexTag;


typedef struct Index{
	IndexTag tag;										// mark the operation type
	Relation dataRelation;								// data relation
	char index[MAX_ATTR_NUM][MAX_TABLE_NAME_LEN];		// index name
	char attrs[MAX_ATTR_NUM][MAX_ATTR_NAME_LEN];		// attr's name array
	uint nums[MAX_ATTR_NUM];							// attr's index
	uint attr_len;										// the number of attr
	Condition type[MAX_ATTR_NUM];							// index type
	Tuple dataTuple;
}IndexData, *Index;

typedef struct State{
	StateTag tag;
	Expr 		expr;
	struct State* child[2]; //binary tree
	Relation*	baseRelation; //the array of the base relation
							// the operation need to use
	uint		baseRelationNumber; // the number of the base relation
}StateData, *State;

typedef struct CreateTableState{
	StateData 	state;
	Relation targetRel; // the relation want to be created
}CreateTableStateData, *CreateTableState;

typedef struct CreateIndexState{
	StateData 	state;
	Relation targetRel; // the target relation
	Index creIndex;
}CreateIndexStateData, *CreateIndexState;

typedef struct DropTableState{
	StateData 	state;
	Relation targetRel; // the target relation
	Index delIndex;
}DropTableStateData, *DropTableState;

typedef struct UpdateTableState{
	StateData 	state;
	Relation targetRel; // the target relation
	Tuple	newtuple;
	Expr	expr[MAX_ATTR_NUM]; // new value, it could be a expression or a value
	uint	columnid[MAX_ATTR_NUM]; // the column that will be changed
	uint 	setColumnNum;
}UpdateTableStateData, *UpdateTableState;

typedef struct DropIndexState{
	StateData 	state;
	//Relation targetRel;
	Index delIndex;
}DropIndexStateData, *DropIndexState;

typedef struct InsertTableState{
	StateData 	state;
	Relation targetRel; // the target relation
	Tuple tuple; // the tuple to be inserted
	Index inrIndex;
}InsertTableStateData, *InsertTableState;

typedef struct DeleteTableState{
	StateData 	state;
	Relation	targetRel; // the target relation
	Index delIndex;
}DeleteTableStateData, *DeleteTableState;

typedef enum ScanMethod{
	SeqScan
}ScanMethod;

typedef struct ScanState{
	StateData 	state;
	Relation	relation;
	Tuple		resultTuple;//the tuple to
	ScanMethod	method;
	Tuple		(*getnext)(struct ScanState* state);
	BlockNumber maxBlockNumbertoScan; //the max block number need to scan
									// this is set to not scan the append while updating
}ScanStateData, *ScanState;


#define RESET_SEQSCAN(seqScan)  {(seqScan)->buffer = INVALID_BUFFER;}
typedef struct SeqScanState{
	ScanStateData scan; //super struct
	BufferPage		buffer; //buffer hold the block data
							//initialize as INVALID_BUFFER
	BlockNumber	blkNumber; //the blocknumber of the block in the buffer
							// initialize as INValidBlockNumber
	uint 		tupleIndex; //the index inside the block of the tuple
						// need to be get next time
}SeqScanStateData, *SeqScanState;

typedef enum JoinMethod{
	NestLoop
}JoinMethod;

typedef struct JoinState{
	StateData	state;
	Tuple		resultTuple;
	JoinMethod	method;
	Relation 	innerRel;
	Relation	outerRel;
	Tuple		(*getnext)(struct JoinState* state);
}JoinStateData, *JoinState;

#define  NestLoopInnerBufferSize (8*1024)
typedef struct NestLoopJoinState{
	JoinStateData	join;
	Tuple*			innnerArray; //inner loop tuple array
								//tuple is created when the state is created
	uint			innerArraysize; //the length of the inner loop tuple array
	uint 			innerSize;//the actual size of the array
	uint			innerPos; //the index of the inner array
							//to be used next size
	Tuple			outerTuple;
	bool			isFirstLoop;
	bool			isLastLoop;
	TMF_File		tmf; //temp file to save the outer tuple;
}NestLoopJoinStateData, *NestLoopJoinState;

typedef struct ProjectionState{
	StateData 	state;
	Tuple		srcTuple;
	Tuple		desTuple;
	Relation	srcRelation;
	Relation	desRelation;
	uint		indexMap[MAX_TABLE_ITEM]; //the map between the source and the dest
}ProjectionStateData, *ProjectionState;

typedef struct SelectState{
	StateData 	state;
}SelectStateData, *SelectState;


typedef struct RelationInfo{
	Relation* rels; //relation the query used
	uint* base; // the base index of each relation index
	uint relNum; // the number of the relation
	uint* columnsUsed; // One-dimensional array to represent 2-d
						//the columns used in every relation
						// if used it is the new index + 1,
						// if unused it is 0.
						//e.g. for the first table, 0 used, 1 unused, 2 used
						// then, columnsUsed[0][0] = 1, columnsUsed[0][1] = 0,columnsUsed[0][2] = 2,
	uint* columNum; // the number of the columns used in every relation
}RelationInfo;

Index Rel_CreateIndex();
Index Rel_GetIndexByTableName(char* tableName);

State analyze_gramTree(SqlManager sql);
Expr analyze_getExpr(pWhereDefinition parsertree, Relation rels[], uint relNum, uint base[]);

CreateIndexState analyze_CreateIndexAttrs(pCreateIndex parsertree, Relation rel);
CreateTableState analyze_CreateTableAttrs(pAttrList attrList, Relation rel);
InsertTableState analyze_InsertTable(pInsertTable parsertree);
DropIndexState analyze_DropIndex(pDropIndex parsertree);
DropTableState analyze_DropTable(pDropTable parsertree);
DeleteTableState analyze_DeleteTable(pDeleteTable parsertree);
UpdateTableState analyze_UpdateTable(pUpdateTable parsertree);

CreateIndexState analyze_CreateIndex(pCreateIndex parsertree);
CreateTableState analyze_CreateTable(pCreateTable parsertree);
SelectState analyze_SelectTable(pSelectTable parsertree);
void getRelationAttrs(Relation rel);
ScanState ConstructScanState(ScanMethod method, Relation relation, Expr expr);
SeqScanState ConstructSeqScanState();
JoinState  ConstructJoinState(JoinMethod method, Relation innerRel, Relation outerRel, Expr expr);
SelectState ConstructSelectState(Expr expr);
NestLoopJoinState ConstructNestLoopJoinState(Relation innerRel, Relation outerRel);
SeqScanState ConstructSeqScanState();
ProjectionState ConstructProjectionState(pAttrList target, Relation rels[], uint relNum, uint base[]);
ProjectionState getProjectionInfo(pAttrList target, Expr expr, RelationInfo* relInfo);

void DestoryCreateTableState(CreateTableState state);
void DestoryDropTableState(DropTableState state);
void DestoryUpdateTableState(UpdateTableState state);
void DestoryInsertTableState(InsertTableState state);
void DestoryDeleteTableState(DeleteTableState state);
void DestoryScanState(ScanState state);
void DestorySeqScanState(SeqScanState state);
void DestoryJoinState(JoinState state);
void DestoryNestLoopJoinState(NestLoopJoinState state);
void DestoryProjectionState(ProjectionState state);
void DestorySelectState(SelectState state);

#endif /* ANALYZE_H_ */
