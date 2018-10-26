/*
 * execTable.c
 *
 *  Created on: Nov 25, 2017
 *      Author: jyh and bsl
 */
#include "exectutor/execScan.h"
#include "exectutor/exectutor.h"

/**
 * made by bsl
 * execute create index sql
 * **/
void execCreateIndex(CreateIndexState state)
{

	Tup_setAttr(sysIndexTuple, &(state->creIndex->dataRelation->Rel_rid), sysIndex_indexOid);
	Tup_setAttr(sysIndexTuple, &(state->creIndex->dataRelation->Rel_rid), sysIndex_tblOid);
	Tup_setAttr(sysIndexTuple, state->creIndex->dataRelation->Rel_Name, sysIndex_tblName);
	Tup_setAttr(sysIndexTuple, state->creIndex->index[0], sysIndex_indexName);
	Tup_setAttr(sysIndexTuple, &(state->creIndex->type[0]), sysIndex_indextype);

	int i = 0;
	for(i = 0; i < state->creIndex->attr_len; i ++){
		Tup_setAttr(sysIndexTuple, state->creIndex->attrs[i], sysIndex_indexattr);
		Tup_vInsert(sysIndexTuple);
	}

	/* call create index
	   arg is state */
}

void execCreateTable(CreateTableState state)
{
	Relation rel = state->targetRel;
	//insert the table into sysTable
	Tup_setAttr(sysTableTuple, &tableId, sysTale_tblOid);
	Tup_setAttr(sysTableTuple, rel->Rel_Name, sysTale_tblName);
	Tup_vInsert(sysTableTuple);
//	Rel_vScanTable(sysTable);
	// insert the table into sysAttr(system attribute table)
	Tup_setAttr(sysAttrTuple, &tableId, sysAttr_tblOid);
	Tup_setAttr(sysAttrTuple, rel->Rel_Name, sysAttr_tblName);
	uint i = 0;
	for(i = 0; i < rel->Rel_attrs.item_num; i ++)
	{
		vmlog(INIT_LOG, "execCreateTable---key:%s, type:%d, i:%d",
				rel->Rel_attrs.item[i].key,rel->Rel_attrs.item[i].type,i);
		Tup_setAttr(sysAttrTuple, rel->Rel_attrs.item[i].key, sysAttr_attrName);
		Tup_setAttr(sysAttrTuple, &(rel->Rel_attrs.item[i].type), sysAttr_attrType);
		Tup_setAttr(sysAttrTuple, &(rel->Rel_attrs.item[i].len), sysAttr_attrLength);
		Tup_setAttr(sysAttrTuple, &(i), sysAttr_attrno);
		Tup_vInsert(sysAttrTuple);
	}
//	Rel_vScanTable(sysAttr);

	rel->Rel_rid = tableId;
	tableId ++;
}

void execDropTable(DropTableState state)
{
	char* name = state->targetRel->Rel_Name;
	Expr expr = vmalloc(sizeof(ExprData));
	expr->op = OP_EQUAL;

	ExprElement elem;
	elem.tag = exprelem_column;
	elem.data.column.columnid = sysTale_tblName;
	Expr expr2 = vmalloc(sizeof(ExprData));
	expr2->op = OP_NONE;
	expr2->content.element = elem;

	Expr expr3 = vmalloc(sizeof(ExprData));
	expr3->op = OP_NONE;
	ExprElement elem2;
	memset(&elem2, 0, sizeof(ExprElement));
	setHeapTupleItemByData(&(elem2.data.item), CHAR, name, strlen(name));
	elem2.tag = exprelem_item;
	expr3->content.element = elem2;

	expr->content.operator[0] = expr2;
	expr->content.operator[1] = expr3;

	sysTableScan->scan.state.expr = expr;
	RESET_SEQSCAN(sysTableScan);
	while(true)
	{
		Tuple tuple = execScan((ScanState)sysTableScan);
		if(tuple == NULL)
		{
			break;
		}else
		{
			Tup_cDeleteTuple(tuple);
		}
	}

	expr2->content.element.data.column.columnid = sysAttr_tblName;
	sysAttrScan->scan.state.expr = expr;
	while(true)
	{
		Tuple tuple = execScan((ScanState)sysAttrScan);
		if(tuple == NULL)
		{
			break;
		}else
		{
			Tup_cDeleteTuple(tuple);
		}
	}

	/* call drop table
		arg is state */
}

bool execInsertTable(InsertTableState state)
{
	Tup_vInsert(state->tuple);

	/* call insert table
		arg is state */
}

bool execDeleteTable(DeleteTableState state)
{
	while(true)
	{
		Tuple tuple = exec(state->state.child[0]);
		if(tuple == NULL)
		{
			break;
		}
		if(checkTuple(state->state.expr, tuple))
		{
			state->delIndex->dataTuple = tuple;
			Tup_cDeleteTuple(tuple);
			/* call insert table
				arg is state */
		}
	}
}

void execUpdateTable(UpdateTableState state)
{
	Rel_AppendNewBlock(state->targetRel, F_dataFile); //append a new block to write the new tuple
	while(true)
	{
		Tuple tuple = exec(state->state.child[0]);
		if(tuple == NULL)
		{
			break;
		}
		if(checkTuple(state->state.expr, tuple))
		{
			Tup_vClearTuple(state->newtuple);
			uint i = 0;
			if(!tuple->TD_heaptuple_exist)
			{
				Tup_vGetHeaptupleFromMemTup(tuple);
			}
			//copy
			for(i = 0; i < tuple->TD_Relation->Rel_attrs.item_num; i++)
			{
				copyHeapTupleItem(state->newtuple->TD_heaptuple + i, tuple->TD_heaptuple + i);
			}
			//change
			for(i = 0; i < state->setColumnNum; i ++)
			{
				ExprElement elem = Expr_Calculate(state->expr[i], tuple);
				if(elem.tag != exprelem_item)
				{
					vmlog(ERROR, "execUpdateTable -- expression calculate error!");
				}
				copyHeapTupleItem(state->newtuple->TD_heaptuple + state->columnid[i], &(elem.data.item));
			}
			state->newtuple->TD_heaptuple_exist = true;
			state->newtuple->TD_memtuple_exist = false;
			//delete
			Tup_cDeleteTuple(tuple);
			Tup_vAppend(state->newtuple);
			//insert
		}
	}
}
