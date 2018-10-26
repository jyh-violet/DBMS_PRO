/*
 * exectutor.c
 *
 *  Created on: Nov 30, 2017
 *      Author: jyh and bsl
 */

#include "exectutor/exectutor.h"

Tuple exec(State state)
{
	Tuple result = NULL;
	switch(state->tag)
	{
	case State_CreateTable:
		execCreateTable((CreateTableState)state);
		break;
	case State_CreateIndex:
		break;
	case State_DropTable:
		execDropTable((DropTableState)state);
		break;
	case State_DropIndex:
		break;
	case State_InsertTable:
		execInsertTable((InsertTableState)state);
		break;
	case State_DeleteTable:
		execDeleteTable((DeleteTableState)state);
		break;
	case State_UpdateTable:
		execUpdateTable((UpdateTableState)state);
		break;
	case State_Scan:
		result = execScan((ScanState)state);
		break;
	case State_Join:
		result = execJoin((JoinState)state);
		break;
	case State_Select:
		execSelect((SelectState)state);
		break;
	case State_Projection:
		result = execProjection((ProjectionState)state);
		break;
	default:
		vmlog(ERROR, "exec---state type error!");
	}
	return result;
}

Tuple execProjection(ProjectionState projection)
{
	while(true)
	{
		projection->srcTuple = exec(projection->state.child[0]);
		if(projection->srcTuple == NULL)
		{
			return NULL;
		}
		if(projection->desTuple == NULL)
		{
			vmlog(ERROR, "execProjection--desTuple is NULL");
		}
		uint i = 0;
		if(!projection->srcTuple->TD_heaptuple_exist)
		{
			Tup_vGetHeaptupleFromMemTup(projection->srcTuple);
		}
		for(i = 0; i < projection->desRelation->Rel_attrs.item_num; i++)
		{
			HeapTupleItem* srcitem = projection->srcTuple->TD_heaptuple + projection->indexMap[i];
			copyHeapTupleItem(&(projection->desTuple->TD_heaptuple[i]), srcitem);
		}
		projection->desTuple->TD_heaptuple_exist = true;
		projection->desTuple->TD_memtuple_exist = false;
		return projection->desTuple;
	}

}
void execSelect(SelectState select)
{
	while(true)
	{
		Tuple tuple =  exec(select->state.child[0]);
		if(tuple == NULL)
		{
			return ; //scan end
		}
		if(checkTuple(select->state.expr, tuple))
		{
			Tup_printHeapTuple(tuple);
		}
	}
}

void DestoryState(State state)
{
	if(state == NULL)
	{
		return;
	}
	DestoryState(state->child[0]);
	DestoryState(state->child[1]);

	switch(state->tag)
	{
	case State_CreateTable:
		DestoryCreateTableState((CreateTableState)state);
		break;
	case State_DropTable:
		DestoryDropTableState((DropTableState)state);
		break;
	case State_InsertTable:
		DestoryInsertTableState((InsertTableState)state);
		break;
	case State_DeleteTable:
		DestoryDeleteTableState((DeleteTableState)state);
		break;
	case State_UpdateTable:
		DestoryUpdateTableState((UpdateTableState)state);
		break;
	case State_Scan:
		DestoryScanState((ScanState)state);
		break;
	case State_Join:
		DestoryJoinState((JoinState)state);
		break;
	case State_Select:
		DestorySelectState((SelectState)state);
		break;
	case State_Projection:
		DestoryProjectionState((ProjectionState)state);
		break;
	}
	DestoryExpr(state->expr);
	vfree((void**)(&state));
}
