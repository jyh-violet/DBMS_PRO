/*
 * analyze.c
 *
 *  Created on: Nov 27, 2017
 *      Author: jyh and bsl
 */

#include "mparser/analyze.h"
#include "exectutor/execScan.h"
#include "exectutor/execJoin.h"
#include "server/sysinit.h"

Oid checkTableNameExist(char* name);
Oid checkIndexNameExist(char* name);
void analyze_createTbaleConstraint(List constraints, Relation rel, uint i, bool* setkey, bool* setauto, uint* autoindex);
Expr ConstructAndExpr(Expr expr1, Expr expr2);
State analyze_handleJoin(Relation rels[], uint relNum, Expr expr, RelationInfo* relInfo);
void optimize_expr(State state, RelationInfo* relInfo);
bool push_expr(State state, Expr expr);
bool checkStateIncludeExpr(State state, Expr expr);
void rewrite_column_item(Expr expr, RelationInfo* relInfo, Relation startRel, bool firsttime);
Attribute_Elem* getTargetColums(pAttrList target, RelationInfo* relInfo);
void getExprColums(Expr expr, RelationInfo* relInfo);

char* stateStr[] = {
		"State_CtrateTable",
		"State_CreateIndex",
		"State_DropTable",
		"State_DropIndex",
		"State_InsertTable",
		"State_DeleteTable",
		"State_UpdateTable",
		"State_Scan",
		"State_Join",
		"State_Select",
		"State_Projection"
};

State analyze_gramTree(SqlManager sql)
{
	State state;
	switch(sql.tag)
	{
	case NODE_CREATE_TABLE:
		state = (State)analyze_CreateTable(sql.sql);
		break;
	case NODE_CREATE_INDEX:
		break;
	case NODE_DROP_TABLE:
		state = (State)analyze_DropTable(sql.sql);
		break;
	case NODE_DROP_INDEX:
		break;
	case NODE_INSERT_TABLE:
		state = (State)analyze_InsertTable(sql.sql);
		break;
	case NODE_DELETE_TABLE:
		state = (State)analyze_DeleteTable(sql.sql);
		break;
	case NODE_SELECT_TABLE:
		state = (State)analyze_SelectTable(sql.sql);
		break;
	case NODE_UPDATE_TABLE:
		state = (State)analyze_UpdateTable(sql.sql);
		break;
	case NODE_NONE:
		vmlog(ERROR, "analyze_gramTree---sql Syntax error!!!");
		break;
	default:
		vmlog(ERROR, "analyze_gramTree---sql Syntax error!!!");
		break;
	}
	vmlog(GRAMMER_LOG, "analyze end !! --tag:%s", stateStr[state->tag]);
	return state;
}

CreateTableState analyze_CreateTable(pCreateTable parsertree)
{
	Oid tableOid = checkTableNameExist(parsertree->exists->name);
	if(tableOid != 0)
	{
		if(parsertree->exists->flag)
		{
			vmlog(PROMPT, "table %s alreay exist", parsertree->exists->name);
			exit(0);
		}else
		{
			//ERROR will exist
			vmlog(ERROR, "create error!!table %s alreay exist!", parsertree->exists->name);
		}
	}

	Relation relation = Rel_CreateRelation(parsertree->exists->name, 0);
	CreateTableState state = analyze_CreateTableAttrs(parsertree->attrs, relation);
	vListDestroy(&(parsertree->attrs->list));
	vfree((void**)&(parsertree->attrs));
	vfree((void**)&(parsertree->exists->name));
	vfree((void**)&(parsertree->exists));
	vfree((void**)&(parsertree));
	return state;

}

DropTableState analyze_DropTable(pDropTable parsertree)
{
	Oid tableOid = checkTableNameExist(parsertree->exists->name);
	if(tableOid == 0)
	{
		if(parsertree->exists->flag)
		{
			vmlog(PROMPT, "table %s not exist", parsertree->exists->name);
			exit(0);
		}else
		{
			//ERROR will exist
			vmlog(ERROR, "drop error!!table %s not exist!", parsertree->exists->name);
		}
	}

	Relation relation = Rel_CreateRelation(parsertree->exists->name, tableOid);
	DropTableState state = (DropTableState)vmalloc(sizeof(DropTableStateData));

	state->state.tag = State_DropTable;
	state->state.child[0] = NULL;
	state->state.child[1] = NULL; //no child
	state->targetRel = relation;
	vfree((void**)&(parsertree->exists->name));
	vfree((void**)&(parsertree->exists));
	vfree((void**)&(parsertree));
	return state;

}

InsertTableState analyze_InsertTable(pInsertTable parsertree)
{
	// relation、编号、索引类型
	Oid tableOid = checkTableNameExist(parsertree->name);
	if(tableOid == 0)
	{
		vmlog(ERROR, "table %s not exist", parsertree->name);
	}

	Relation relation = Rel_CreateRelation(parsertree->name, tableOid);

	//get the attributions of the table from the system table
	getRelationAttrs(relation);
	Tuple tuple = Tup_CreateTuple(relation);
	if(parsertree->attrs != NULL)
	{
		List attrnode;
		List valuenode = parsertree->values->list;
		valuenode = ((List)valuenode)->next;
		foreach(attrnode, parsertree->attrs->list)
		{
			pAttrItem attr = attrnode->cell;
			if((attr->table != NULL) && (strcmp(attr->table, parsertree->name) != 0))
			{
				vmlog(ERROR, "parser---sql Syntax error!Table name is not matched, "
						"earlier is :%s, later is :%s", parsertree->name, attr->table);
			}
			uint index = Rel_findAttrIndex(relation, attr->name);
			if(!IS_VALLID_ATTR_INDEX(index))
			{
				vmlog(ERROR, "parser---sql Syntax error!attribute name error. table :%s has no attribute:%s",
						parsertree->name, attr->name);
			}
			if(valuenode == NULL)
			{
				vmlog(ERROR, "parser---sql Syntax error!"
						"The number of attributes and the values is not matched");
			}
			Tup_setAttrByStr(tuple, valuenode->cell, index);
			valuenode = valuenode->next;
			vfree((void**)&(attr->name));
			vfree((void**)&(attr->table));
		}
		vListDestroy(&(parsertree->attrs->list));
		//default value should be set here
		if(valuenode != NULL)
		{
			vmlog(ERROR, "parser---sql Syntax error!"
					"The number of attributes and the values is not matched");
		}
		vfree((void**)&(parsertree->attrs));
	}else
	{
		uint index = 0;
		List valuenode ;
		foreach(valuenode, parsertree->values->list)
		{
			Tup_setAttrByStr(tuple, valuenode->cell, tuple->TD_Relation->Rel_attrs.item_num - 1 - index);
			index ++;
			if(index > relation->Rel_attrs.item_num)
			{
				vmlog(ERROR, "parser---sql Syntax error!"
						"The number of attributes and the values is not matched");
			}
		}
		if(index < relation->Rel_attrs.item_num)
		{
			vmlog(ERROR, "parser---sql Syntax error!"
					"The number of attributes and the values is not matched");
		}

	}
	vListDestroy(&(parsertree->values->list));
	vfree((void**)&(parsertree->values));
	InsertTableState state = (InsertTableState)vmalloc(sizeof(InsertTableStateData));

	state->state.tag = State_InsertTable;
	state->state.child[0] = NULL;
	state->state.child[1] = NULL; //no child
	state->targetRel = relation;
	state->tuple = tuple;
	vfree((void**)&(parsertree->name));
	vfree((void**)&(parsertree));
	return state;
}

UpdateTableState analyze_UpdateTable(pUpdateTable parsertree)
{
	Oid tableOid = checkTableNameExist(parsertree->name);
	if(tableOid == 0)
	{
		vmlog(ERROR, "table %s not exist", parsertree->name);
	}
	Relation relation = Rel_CreateRelation(parsertree->name, tableOid);
	getRelationAttrs(relation);
	Relation rels[1];
	uint 	base[1];
	base[0] = 0;
	rels[0] = relation;
	UpdateTableState state = (UpdateTableState)vmalloc(sizeof(UpdateTableStateData));
	state->state.tag = State_UpdateTable;
	state->targetRel = relation;
	state->newtuple = Tup_CreateTuple(relation);
	state->state.expr = analyze_getExpr(parsertree->where, rels, 1, base);
	ScanState scanState = ConstructScanState(SeqScan, relation, NULL);
	state->state.child[0] = (State)scanState;
	state->state.child[1] = NULL; //no child

	List listnode;
	uint columnindex = 0;
	foreach(listnode, parsertree->list->list)
	{
		pSetDefinition set = listnode->cell;
		pAttrItem attr = set->attr;
		if((attr->table != NULL) && (strcmp(attr->table, parsertree->name) != 0))
		{
			vmlog(ERROR, "parser---sql Syntax error!Table name is not matched, "
					"earlier is :%s, later is :%s", parsertree->name, attr->table);
		}
		uint index = Rel_findAttrIndex(relation, attr->name);
		if(!IS_VALLID_ATTR_INDEX(index))
		{
			vmlog(ERROR, "parser---sql Syntax error!attribute name error. table :%s has no attribute:%s",
					parsertree->name, attr->name);
		}
		state->columnid[columnindex] = index;
		state->expr[columnindex] = getExpr(set->expr, rels, 1, base);
		columnindex ++;
		vfree((void**)&(attr->name));
		vfree((void**)&(attr->table));
		vfree((void**)&(set->attr));
	}

	state->setColumnNum = columnindex;

	vListDestroy(&(parsertree->list->list));

	vfree((void**)&(parsertree->name));
	vfree((void**)&(parsertree->where));
	vfree((void**)&(parsertree));
	return state;
}

DeleteTableState analyze_DeleteTable(pDeleteTable parsertree)
{
	Oid tableOid = checkTableNameExist(parsertree->name);
	if(tableOid == 0)
	{
		vmlog(ERROR, "table %s not exist", parsertree->name);
	}

	Relation relation = Rel_CreateRelation(parsertree->name, tableOid);
	getRelationAttrs(relation);
	Relation rels[1];
	uint base[1];
	base[0] = 0;
	rels[0] = relation;
	DeleteTableState state = (DeleteTableState)vmalloc(sizeof(DeleteTableStateData));

	state->state.tag = State_DeleteTable;
	state->targetRel = relation;
	state->state.expr = analyze_getExpr(parsertree->where, rels, 1, base);
	ScanState scanState = ConstructScanState(SeqScan, relation, NULL);
	state->state.child[0] = (State)scanState;
	state->state.child[1] = NULL; //no child
	vfree((void**)&(parsertree->name));
	vfree((void**)&(parsertree->where));
	vfree((void**)&(parsertree));
	return state;

}

SelectState analyze_SelectTable(pSelectTable parsertree)
{
	Relation* rels = vmalloc(parsertree->froms->size * (sizeof(Relation)));
	uint i = 0;
	List listnode;
	RelationInfo relationInfo;
	//analyze the table names
	foreach(listnode, parsertree->froms->list)
	{
		//list is in reverse order
		uint index = parsertree->froms->size - 1 - i;
		char* name = (char*) listnode->cell;
		Oid tableOid = checkTableNameExist(name);
		if(tableOid == 0)
		{
			vmlog(ERROR, "table %s not exist", name);
		}
		rels[index] = Rel_CreateRelation(name, tableOid);
		getRelationAttrs(rels[index]);
		i ++;
	}
	uint* base = vmalloc0(sizeof(uint) * parsertree->froms->size); //get the attribute base index
															//inside the result relation
	base[0] = 0;
	for(i = 1; i < parsertree->froms->size; i ++)
	{
		base[i] = base[i - 1] + rels[i - 1]->Rel_attrs.item_num;
	}
	relationInfo.base = base;
	relationInfo.relNum = parsertree->froms->size;
	relationInfo.rels = rels;
	relationInfo.columnsUsed = vmalloc0(sizeof(uint) * relationInfo.relNum * MAX_TABLE_ITEM);
	relationInfo.columNum = vmalloc0(sizeof(uint) * MAX_TABLE_ITEM);
	for(i = 0; i < relationInfo.relNum; i ++)
	{
		int j = 0;
		for(j = 0; j < relationInfo.rels[i]->Rel_attrs.item_num; j ++)
		{
			relationInfo.columNum[i] ++;
			relationInfo.columnsUsed[i * relationInfo.relNum + j] = relationInfo.columNum[i];
		}
	}
	Expr expr = analyze_getExpr(parsertree->where, rels, parsertree->froms->size, base);
	State project = (State)getProjectionInfo(parsertree->targets, expr,&relationInfo);
	State joins = (State)analyze_handleJoin(rels, parsertree->froms->size, expr, &relationInfo);
//	State project = ConstructProjectionState(parsertree->targets, rels, parsertree->froms->size, base);
	SelectState select = ConstructSelectState(NULL);
	if(project == NULL)
	{
		select->state.child[0] = joins;
	}else
	{
		project->child[0] = joins;
		select->state.child[0] = project;
	}
	select->state.baseRelation = rels;
	select->state.baseRelationNumber = parsertree->froms->size;
	optimize_expr((State)select, &relationInfo);
	vfree((void**)(&base));
	vfree((void**)(&rels));
	vfree((void**)(&relationInfo.columnsUsed));
	vfree((void**)(&relationInfo.columNum));
	return select;
}

/**
 * getColumns
 * target	: the select columns of the query
 * expr		: the expression of the query, we must retain the colums used in it
 * **/
ProjectionState getProjectionInfo(pAttrList target, Expr expr, RelationInfo* relInfo)
{
	if(target == NULL)
	{
		return NULL;
	}
	uint i  = 0;
	for(i = 0; i < relInfo->relNum; i ++)
	{
		relInfo->columNum[i] = 0;
	}
	memset(relInfo->columnsUsed, 0, sizeof(uint) * relInfo->relNum * MAX_TABLE_ITEM);
	getExprColums(expr, relInfo);
	Attribute_Elem* attrs = getTargetColums(target, relInfo);
	// now the relInfo->columnsUsed record the column used or not
	// we will calculate the new index
	for(i = 0; i < relInfo->relNum; i ++)
	{
		uint j = 0;
		for(j = 0; j < relInfo->rels[i]->Rel_attrs.item_num; j ++)
		{
			if(relInfo->columnsUsed[i * relInfo->relNum + j] >= 1)
			{
				relInfo->columNum[i] ++;
				relInfo->columnsUsed[i * relInfo->relNum + j] = relInfo->columNum[i];
			}
		}
	}

	//now we will construct the last projection
	ProjectionState project = vmalloc0(sizeof(ProjectionStateData));
	Relation resultRel = Rel_CreateTempRelation();
	for(i = 0; i < target->size; i ++)
	{
		uint newcolumid = 0;
		uint j = 0;
		for(j = 0; j < relInfo->relNum; j ++)
		{
			if(relInfo->rels[j]->Rel_rid == attrs[i].tableOid)
			{
				resultRel->Rel_attrs.item[i] = relInfo->rels[j]->Rel_attrs.item[
												attrs[i].columnid - relInfo->base[j]
																		   ];
				break;
			}
			newcolumid += relInfo->columNum[j];
		}
		uint indexi = j;
		uint indexj = attrs[i].columnid - relInfo->base[j];
		if(relInfo->columnsUsed[indexi * relInfo->relNum + indexj] < 1)
		{
			vmlog(ERROR, "getColumns -- the column is used but not selectde");
		}
		project->indexMap[i] = newcolumid +relInfo->columnsUsed[indexi * relInfo->relNum + indexj] - 1;
		resultRel->Rel_attrs.item_num ++;
	}
	resultRel->Rel_maxtuple_len = uGetMaxTupleLen(&(resultRel->Rel_attrs));
	project->desRelation = resultRel;
	project->desTuple = Tup_CreateTuple(resultRel);
	project->state.tag = State_Projection;
	project->state.baseRelation = relInfo->rels;
	project->state.baseRelationNumber = relInfo->relNum;
	project->state.child[0] = NULL;
	project->state.child[1] = NULL;
	return project;
}

/**
 * getTargetColums
 * get the information of the select columns of the query
 * **/
Attribute_Elem* getTargetColums(pAttrList target, RelationInfo* relInfo)
{
	if(target == NULL)
	{
		return NULL;
	}
	Attribute_Elem* attrs = vmalloc0(sizeof(Attribute_Elem) * target->size);
	uint i = 0;
	List listnode;
	foreach(listnode, target->list)
	{
		uint index =  target->size - 1 - i;
		pAttrItem attrItem = (pAttrItem)listnode->cell;
		attrs[index] = getAttrElem(attrItem, relInfo->rels, relInfo->relNum, relInfo->base);
		if(attrItem->table != NULL)
		{
			printf("%s.%s\t|", attrItem->table, attrItem->name);
		}else
		{
			printf( "%s\t|", attrItem->name);
		}
		uint j = 0;
		for(j = 0; j < relInfo->relNum; j ++)
		{
			if(relInfo->rels[j]->Rel_rid == attrs[index].tableOid)
			{
				break;
			}
		}
		uint indexi = j;
		uint indexj = attrs[index].columnid - relInfo->base[j];
		relInfo->columnsUsed[indexi * relInfo->relNum + indexj] = 1;
		i ++;
	}
	printf("\n");
	return attrs;
}

/**
 * getExprColums
 * get the columns used in the expression
 * the columns will be recorded in the relInfo
 * **/
void getExprColums(Expr expr, RelationInfo* relInfo)
{
	if(expr == NULL)
	{
		return;
	}
	if(expr->op == OP_NONE && expr->content.element.tag == exprelem_column)
	{
		Attribute_Elem column = expr->content.element.data.column;
		uint j = 0;
		for(j = 0; j < relInfo->relNum; j ++)
		{
			if(relInfo->rels[j]->Rel_rid == column.tableOid)
			{
				break;
			}
		}
		uint indexi = j;
		uint indexj = column.columnid - relInfo->base[j];

		relInfo->columnsUsed[indexi * relInfo->relNum + indexj]= 1;
	}else if(expr->op != OP_NONE)
	{
		getExprColums(expr->content.operator[0], relInfo);
		getExprColums(expr->content.operator[1], relInfo);
		getExprColums(expr->content.operator[2], relInfo);
	}
}

void optimize_expr(State state, RelationInfo* relInfo)
{
	if(state == NULL)
	{
		return;
	}
	Relation* rels = relInfo->rels;
	uint* base = relInfo->base;
	uint relNum = relInfo->relNum;
	vmlog(OPTIMIZER_LOG, "state:%s, including relations:", stateStr[state->tag]);
	uint i = 0;
	for(i = 0; i < state->baseRelationNumber; i ++)
	{
		vmlog(OPTIMIZER_LOG, "relname:%s, relOid:%d",
				state->baseRelation[i]->Rel_Name, state->baseRelation[i]->Rel_rid);
	}
	vmlog(OPTIMIZER_LOG, "expr including relations----before push:");
	if(state->expr != NULL)
	{
		for(i = 0; i < state->expr->relsNumber; i ++)
		{
			vmlog(OPTIMIZER_LOG, "relOid:%d", state->expr->relsOidArray[i]);
		}
		if(push_expr(state->child[0], state->expr))
		{
			state->expr = NULL;
		}
		if(push_expr(state->child[1], state->expr))
		{
			state->expr = NULL;
		}
	}
	vmlog(OPTIMIZER_LOG, "expr including relations----after push:");
	if(state->expr != NULL)
	{
		for(i = 0; i < state->expr->relsNumber; i ++)
		{
			vmlog(PROMPT, "relOid:%d", state->expr->relsOidArray[i]);
		}
	}
	vmlog(OPTIMIZER_LOG, "state:%s push end~~~~", stateStr[state->tag]);
	for(i = 0; i < 2; i++) // push down projection
	{
		optimize_expr(state->child[i], relInfo);
	}
	rewrite_column_item(state->expr, relInfo, state->baseRelation[0], true);
	vmlog(OPTIMIZER_LOG, "state:%s rewrite_column end~~~~", stateStr[state->tag]);
}

/**
 * change the columnid of the exprelem_column
 * columnid = baseid + incolumn, baseid is the sum of the number of the attributes of the relations before it
 * 								incolumn is the id inside its relation
 * if firsttime is true: need to change the baseid and incolumn
 * 					false: just change the baseid
 * */
void rewrite_column_item(Expr expr, RelationInfo* relInfo, Relation startRel, bool firsttime)
{
	if(expr == NULL)
	{
		return;
	}
	Relation* rels = relInfo->rels;
	uint* base = relInfo->base;
	uint relNum = relInfo->relNum;
	uint i = 0, startIndex;
	for(i = 0; i < relNum; i ++)
	{
		if(rels[i]->Rel_rid == startRel->Rel_rid)
		{
			startIndex = i;
			break;
		}
	}
	if(i >= relNum)
	{
		vmlog(ERROR, "relation :%s is not in the from list", startRel->Rel_Name);
	}
	vmlog(OPTIMIZER_LOG, "startIndex:%d , startRelation:%s", startIndex, startRel->Rel_Name);
	if(expr->op == OP_NONE && expr->content.element.tag == exprelem_column)
	{
//		expr->content.element.data.column.columnid -= base[startIndex];
		Oid relId = expr->content.element.data.column.tableOid;
		uint columnNumSum = 0;
		for(i = startIndex; i < relNum; i ++)
		{
			if(rels[i]->Rel_rid == relId)
			{
				break;
			}
			columnNumSum += relInfo->columNum[i];
		}
		if(i >= relNum)
		{
			vmlog(ERROR, "relation :%d is not found", relId);
		}

		uint columnOriginId = expr->content.element.data.column.columnid - relInfo->base[i];
		if(firsttime)
		{
			uint indexi = i;
			uint indexj = columnOriginId;
			if(columnNumSum + relInfo->columnsUsed[indexi * relInfo->relNum + indexj] < 1)
			{
				vmlog(ERROR, "getColumns -- the column is used but not selectde");
			}
			expr->content.element.data.column.columnid = columnNumSum + relInfo->columnsUsed[indexi * relInfo->relNum + indexj] - 1;
		}else
		{
			expr->content.element.data.column.columnid = columnNumSum + columnOriginId;
		}
	}else if(expr->op != OP_NONE)
	{
		rewrite_column_item(expr->content.operator[0], relInfo, startRel, firsttime);
		rewrite_column_item(expr->content.operator[1], relInfo, startRel, firsttime);
		rewrite_column_item(expr->content.operator[2], relInfo, startRel, firsttime);
	}
}

bool push_expr(State state, Expr expr)
{
	if(state == NULL)
	{
		return false;
	}
	if(expr == NULL)
	{
		return true;
	}
	if(checkStateIncludeExpr(state, expr))
	{
		if(state->expr == NULL)
		{
			state->expr = expr;
		}else
		{
			state->expr = ConstructAndExpr(state->expr, expr);
		}
		return true;
	}
	if(expr->op == OP_AND)
	{
		if(push_expr(state, expr->content.operator[0]))
		{
			expr->content.operator[0] = NULL;
		}
		if(push_expr(state, expr->content.operator[1]))
		{
			expr->content.operator[1] = NULL;
		}
	}
	return false;
}


Expr ConstructAndExpr(Expr expr1, Expr expr2)
{
	Expr expr = vmalloc0(sizeof(ExprData));
	expr->op = OP_AND;
	expr->content.operator[0] = expr1;
	expr->content.operator[1] = expr2;
	expr->relsOidArray = vmalloc(sizeof(Oid) * (expr1->relsNumber + expr2->relsNumber));
	uint i = 0;
	uint j = 0;
	for(i = 0; i < expr->content.operator[0]->relsNumber; i++)
	{
		 expr->relsOidArray[j] = expr->content.operator[0]->relsOidArray[i];
		 j ++;
	}
	for(i = 0; i < expr->content.operator[1]->relsNumber; i++)
	{
		uint k = 0;
		for(k = 0; k < j ; k ++)
		{
			if(expr->content.operator[1]->relsOidArray[i] == expr->relsOidArray[k])
			{
				break;
			}
		}
		if(k < j) //repeat
		{
			continue;
		}
		 expr->relsOidArray[j] = expr->content.operator[1]->relsOidArray[i];
		 j ++;
	}
	expr->relsNumber = j;
	return expr;
}

bool checkStateIncludeExpr(State state, Expr expr)
{
	uint i = 0, j = 0;
	if(state == NULL)
	{
		return false;
	}
	if(expr == NULL)
	{
		return true;
	}
	for(i = 0; i < expr->relsNumber; i ++)
	{
		for(j = 0; j < state->baseRelationNumber; j ++)
		{
			if(expr->relsOidArray[i] == state->baseRelation[j]->Rel_rid)
			{
				break;
			}
		}
		if(j >= state->baseRelationNumber) // can not find one relation; not include
		{
			return false;
		}
	}
	return true;
}


/**
 * analyze_handleJoin
 * produces join executors for a list of rels
 * the memory of the array baseRelation is shared with each,
 * that is all is pointing to the array of the select state,
 * as the order of the tables to join will not be changed now,
 * the relations used in the given executors is a continuous sub-array of the baseRelation of the selectState
 * */
State analyze_handleJoin(Relation rels[], uint relNum, Expr expr, RelationInfo* relInfo)
{
	ScanState scanState = ConstructScanState(SeqScan, rels[0], NULL); // scan state for the first tuple
	State state = (State)scanState;

	state->baseRelation = rels;
	state->baseRelationNumber = 1;
	Relation lastRel = rels[0];

	if(relInfo->columNum[0] < relInfo->rels[0]->Rel_attrs.item_num)  // need a projection
	{
		//now we will construct the last projection
		ProjectionState project = vmalloc0(sizeof(ProjectionStateData));
		Relation resultRel = Rel_CreateTempRelation();
		uint i = 0;
		vmlog(OPTIMIZER_LOG, "relation:%d,%s--using column:", relInfo->rels[0]->Rel_rid, relInfo->rels[0]->Rel_Name);
		for(i = 0; i < relInfo->rels[0]->Rel_attrs.item_num; i ++)
		{
			if(relInfo->columnsUsed[i] > 0)
			{
				uint index = relInfo->columnsUsed[i] - 1;
				project->indexMap[index] = i;
				resultRel->Rel_attrs.item[index] = relInfo->rels[0]->Rel_attrs.item[i];
				resultRel->Rel_attrs.item_num ++;
				vmlog(OPTIMIZER_LOG, "%d-%s", i, relInfo->rels[0]->Rel_attrs.item[i].key);
			}
		}
		resultRel->Rel_maxtuple_len = uGetMaxTupleLen(&(resultRel->Rel_attrs));
		project->desRelation = resultRel;
		project->desTuple = Tup_CreateTuple(resultRel);
		project->state.tag = State_Projection;
		project->state.baseRelation = scanState->state.baseRelation;
		project->state.baseRelationNumber =scanState->state.baseRelationNumber;
		project->state.child[0] = (State)scanState;
		project->state.child[1] = NULL;
		project->state.expr = NULL;

		state = (State)project;
		lastRel = project->desRelation;
	}
	uint i = 0;
	for(i = 1; i < relNum; i ++)
	{
		State newstate = (State)ConstructScanState(SeqScan, rels[i], NULL);
		newstate->baseRelation = rels + i;
		newstate->baseRelationNumber = 1;
		Relation newRel = rels[i];
		if(relInfo->columNum[i] < relInfo->rels[i]->Rel_attrs.item_num)  // need a projection
		{
			//now we will construct the projection
			ProjectionState project = vmalloc0(sizeof(ProjectionStateData));
			Relation resultRel = Rel_CreateTempRelation();
			uint j = 0;
			vmlog(OPTIMIZER_LOG, "relation:%d,%s--using column:", relInfo->rels[i]->Rel_rid, relInfo->rels[i]->Rel_Name);
			for(j = 0; j < relInfo->rels[i]->Rel_attrs.item_num; j ++)
			{
				if(relInfo->columnsUsed[i * relInfo->relNum + j] > 0)
				{
					uint index = relInfo->columnsUsed[i * relInfo->relNum + j] - 1;
					project->indexMap[index] = j;
					resultRel->Rel_attrs.item[index] = relInfo->rels[i]->Rel_attrs.item[j];
					resultRel->Rel_attrs.item_num ++;
					vmlog(OPTIMIZER_LOG, "%d-%s", j, relInfo->rels[i]->Rel_attrs.item[j].key);
				}
			}
			resultRel->Rel_maxtuple_len = uGetMaxTupleLen(&(resultRel->Rel_attrs));
			project->desRelation = resultRel;
			project->desTuple = Tup_CreateTuple(resultRel);
			project->state.tag = State_Projection;
			project->state.baseRelation = newstate->baseRelation;
			project->state.baseRelationNumber =newstate->baseRelationNumber;
			project->state.child[0] = newstate;
			project->state.child[1] = NULL;
			project->state.expr = NULL;

			newstate = (State)project;
			newRel = project->desRelation;
		}
		JoinState join = ConstructJoinState(NestLoop, lastRel, newRel, NULL);
		join->state.child[0] = state;
		join->state.child[1] = newstate;
		join->state.baseRelation = rels;
		join->state.baseRelationNumber = i + 1;
		state = (State)join;
		lastRel = join->resultTuple->TD_Relation;
	}
	state->expr = expr;
//	rewrite_column_item(expr, relInfo, rels[0], true);
	return state;
}


Expr analyze_getExpr(pWhereDefinition parsertree, Relation rels[], uint relNum, uint base[])
{
	if(parsertree == NULL) // no where condition
	{
		return NULL;
	}
	return getExpr(parsertree->expr, rels, relNum, base);
}

CreateTableState analyze_CreateTableAttrs(pAttrList attrList, Relation rel)
{
	List listnode;
	bool setkey = false,setauto = false;
	uint autoindex;
	uint attrNum = attrList->size;
	foreach(listnode, attrList->list)
	{
		uint i = attrNum - 1 - rel->Rel_attrs.item_num;
		pAttrDefinition attr = (pAttrDefinition)listnode->cell;
		rel->Rel_attrs.item[i].type = attr->type->type;
		rel->Rel_attrs.item[i].len = attr->type->size;
		strcpy(rel->Rel_attrs.item[i].key, attr->name);
		// analyze the constraint
		if(attr->constraint != NULL)
		{
			List constraints = attr->constraint->constraints->list;
			analyze_createTbaleConstraint(constraints, rel, i, &setkey, &setauto, &autoindex);
			vListDestroy(&(attr->constraint->constraints->list));
			vfree((void**)&(attr->constraint->constraints));
			vfree((void**)&(attr->constraint));
		}
		vfree((void**)&(attr->name));
		rel->Rel_attrs.item_num ++;
	}
	CreateTableState state = (CreateTableState)vmalloc(sizeof(CreateTableStateData));
	state->state.tag = State_CreateTable;
	state->state.child[0] = NULL;
	state->state.child[1] = NULL; //no child
	state->targetRel = rel;
	return state;
}

void analyze_createTbaleConstraint(List constraints, Relation rel, uint i, bool* setkey, bool* setauto, uint* autoindex)
{
	List constraint;
	bool  setnull = false,setuniqe = false,setdefault = false;
	foreach(constraint, constraints)
	{
		pChildConstraint constraintdata = (pChildConstraint)constraint->cell;
		switch(constraintdata->type)
		{
		case CONSTRAINT_KEY:
			if(*setkey)
			{
				vmlog(ERROR, "analyze_gramTree---Incorrect table definition;primary key defined repeatedly");
			}
			if(constraintdata->data->type == KEY_PRIMARY)
			{
				Attribute_set_PRIMARYKEY(rel->Rel_attrs.item[i]);
				rel->primary_key_attr_index = i;
				*setkey = true;
			}
			break;
		case CONSTRAINT_NULL:
			if(constraintdata->data->type != NULL_NONE )
			{
				if(setnull)
				{
					vmlog(ERROR, "analyze_gramTree---Incorrect table definition;null constraint repeatedly");
				}else
				{
					setnull = true;
				}
			}
			if(constraintdata->data->type == NULL_FALSE)
			{
				Attribute_set_NOTNULL(rel->Rel_attrs.item[i]);
			}
			break;
		case CONSTRAINT_UNIQUE:
			if(setuniqe)
			{
				vmlog(ERROR, "analyze_gramTree---Incorrect table definition;unique constraint repeatedly");
			}
			setuniqe = true;
			if(constraintdata->data->type == UNIQUE_TRUE)
			{
				Attribute_set_UNIQUE(rel->Rel_attrs.item[i]);
			}
			break;
		case CONSTRAINT_DEFAULT:
			if(constraintdata->data->type != NULL_NONE )
			{
				if(setdefault)
				{
					vmlog(ERROR, "analyze_gramTree---Incorrect table definition;default constraint repeatedly");
				}else
				{
					setdefault = true;
				}
			}
			if(constraintdata->data->type == DEFAULT_TRUE)
			{
				Attribute_set_DEFAULT(rel->Rel_attrs.item[i]);
				rel->Rel_attrs.item[i].default_value = vmalloc(strlen(constraintdata->data->data) + 1);
				strcpy(rel->Rel_attrs.item[i].default_value, constraintdata->data->data);
			}
			break;
		case CONSTRAINT_AUTOINCREMENT:
			if(*setauto)
			{
				vmlog(ERROR, "analyze_gramTree---Incorrect table definition; there can be only one auto column");
			}
			*setauto = true;
			if((constraintdata->data->type == AUTO_TRUE))
			{
				Attribute_set_AUTO(rel->Rel_attrs.item[i]);
				*autoindex = i;
			}
			break;
		}
		vfree((void**)&(constraintdata->data->data));
	}
	if(*setauto && (*autoindex != rel->primary_key_attr_index))
	{
		vmlog(ERROR, "analyze_gramTree---Incorrect table definition; auto column must be defined as a key");
	}
}

/**
 * checkTableNameExist : by jyh
 * check the table name exist or not.
 * return the tableoid if exist
 * else return 0;(0is the reserved oid of the system table,
 * 				so can not be the oid of any table)
 * **/
Oid checkTableNameExist(char* name)
{
	int tableOid = 0;
	vmlog(PARSER_LOG, "parser---checkTableNameExist:%s", name);
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
	Tuple tuple = execScan((ScanState)sysTableScan);
	if(tuple != NULL)
	{
		vmlog(GRAMMER_LOG, "table:%s exist!!!",name);
		HeapTupleItem item = Tup_getAttr(tuple, sysTale_tblOid);
		getHeapTupleItemData(&item, INTEGER, &tableOid);
	}
	vmlog(GRAMMER_LOG, "table:%s exist:%d",name, tableOid);
	DestoryExpr(expr);
	return tableOid;
}

void getRelationAttrs(Relation rel)
{
	Expr expr = vmalloc0(sizeof(ExprData));
	expr->op = OP_EQUAL;

	ExprElement elem;
	elem.tag = exprelem_column;
	elem.data.column.columnid = sysAttr_tblName;
	Expr expr2 = vmalloc0(sizeof(ExprData));
	expr2->op = OP_NONE;
	expr2->content.element = elem;

	Expr expr3 = vmalloc0(sizeof(ExprData));
	expr3->op = OP_NONE;
	ExprElement elem2;
	memset(&elem2, 0, sizeof(ExprElement));
	setHeapTupleItemByData(&(elem2.data.item), CHAR, rel->Rel_Name, strlen(rel->Rel_Name));
	elem2.tag = exprelem_item;
	expr3->content.element = elem2;

	expr->content.operator[0] = expr2;
	expr->content.operator[1] = expr3;

	sysAttrScan->scan.state.expr = expr;
	RESET_SEQSCAN(sysAttrScan);
	rel->Rel_attrs.item_num = 0;
	while(true)
	{
		Tuple tuple = execScan((ScanState)sysAttrScan);
		if(tuple == NULL)
		{
			break;
		}else
		{
			HeapTupleItem item = Tup_getAttr(tuple, sysAttr_attrName);
			getHeapTupleItemData(&item, CHAR, rel->Rel_attrs.item[rel->Rel_attrs.item_num].key);
			item = Tup_getAttr(tuple, sysAttr_attrType);
			getHeapTupleItemData(&item, INTEGER, &(rel->Rel_attrs.item[rel->Rel_attrs.item_num].type));
			item = Tup_getAttr(tuple, sysAttr_attrLength);
			getHeapTupleItemData(&item, INTEGER, &(rel->Rel_attrs.item[rel->Rel_attrs.item_num].len));
			rel->Rel_attrs.item_num ++;
		}
	}
	rel->Rel_maxtuple_len = uGetMaxTupleLen(&(rel->Rel_attrs));
	DestoryExpr(expr);
}

ScanState ConstructScanState(ScanMethod method, Relation relation, Expr expr)
{
	ScanState scanState = NULL;
	switch(method)
	{
	case SeqScan:
		scanState = (ScanState)ConstructSeqScanState();
		scanState->getnext = SeqScan_getNext;
		break;
	default:
		vmlog(ERROR, "parser--scan method error!");

	}
	scanState->method = method;
	scanState->state.tag = State_Scan;
	scanState->state.child[0] = NULL;
	scanState->state.child[1] = NULL;
	scanState->relation = relation;
	scanState->maxBlockNumbertoScan = relation->Rel_curMaxBlock;
	Tuple tuple = Tup_CreateTuple(relation);
	scanState->resultTuple = tuple;
	scanState->state.expr = expr;
	return scanState;
}

JoinState  ConstructJoinState(JoinMethod method, Relation innerRel, Relation outerRel, Expr expr)
{
	JoinState joinState = NULL;
	switch(method)
	{
	case NestLoop:
		joinState = (JoinState)ConstructNestLoopJoinState(innerRel, outerRel);
		joinState->getnext = NestLoopJoin_getNext;
		break;
	default:
			vmlog(ERROR, "parser--join method error!");
	}
	joinState->method = method;
	joinState->innerRel = innerRel;
	joinState->outerRel = outerRel;
	joinState->state.expr = expr;

	Relation resultRel = Rel_CreateTempRelation();
	resultRel->Rel_maxtuple_len = innerRel->Rel_maxtuple_len + outerRel->Rel_maxtuple_len;
	uint i = 0;
	for(i = 0; i < innerRel->Rel_attrs.item_num; i ++)
	{
		resultRel->Rel_attrs.item[resultRel->Rel_attrs.item_num] = innerRel->Rel_attrs.item[i];
		resultRel->Rel_attrs.item_num ++;
	}
	for(i = 0; i < outerRel->Rel_attrs.item_num; i ++)
	{
		resultRel->Rel_attrs.item[resultRel->Rel_attrs.item_num] = outerRel->Rel_attrs.item[i];
		resultRel->Rel_attrs.item_num ++;
	}
	joinState->resultTuple = Tup_CreateTuple(resultRel);
	joinState->state.tag = State_Join;
	return joinState;

}

SelectState ConstructSelectState(Expr expr)
{
	SelectState select = vmalloc0(sizeof(SelectStateData));
	select->state.expr = expr;
	select->state.tag = State_Select;
	select->state.child[0] = NULL;
	select->state.child[1] = NULL;
	return select;
}

NestLoopJoinState ConstructNestLoopJoinState(Relation innerRel, Relation outerRel)
{
	NestLoopJoinState nestloop = vmalloc0(sizeof(NestLoopJoinStateData));
	nestloop->innerArraysize = NestLoopInnerBufferSize / innerRel->Rel_maxtuple_len * 2;
	nestloop->innerPos = 0;
	nestloop->innerSize = 0;
	nestloop->innnerArray = vmalloc(nestloop->innerArraysize * sizeof(Tuple));
	uint i = 0;
	for(i = 0; i < nestloop->innerArraysize; i ++)
	{
		nestloop->innnerArray[i] = Tup_CreateTuple(innerRel);
	}
	nestloop->outerTuple = NULL;
	nestloop->isFirstLoop = true;
	nestloop->isLastLoop = false;
	nestloop->tmf = TMF_CreateFile();
	return nestloop;
}


SeqScanState ConstructSeqScanState()
{
	SeqScanState seqScan = vmalloc(sizeof(SeqScanStateData));
	seqScan->blkNumber = 0;
	seqScan->buffer = INVALID_BUFFER;
	seqScan->tupleIndex = 0;
	return seqScan;
}


ProjectionState ConstructProjectionState(pAttrList target, Relation rels[], uint relNum, uint base[])
{
	if(target == NULL)
	{
		return NULL;
	}
	ProjectionState project = vmalloc0(sizeof(ProjectionStateData));
	Relation resultRel = Rel_CreateTempRelation();
	List listnode;
	Attribute_Elem* attrs = vmalloc0(sizeof(Attribute_Elem) * target->size);
	uint i = 0;
	foreach(listnode, target->list)
	{
		uint index =  target->size - 1 - i;
		attrs[index] = getAttrElem((pAttrItem)listnode->cell, rels, relNum, base);
		uint j = 0;
		for(j = 0; j < relNum; j ++)
		{
			if(rels[j]->Rel_rid == attrs[index].tableOid)
			{
				resultRel->Rel_attrs.item[index] = rels[j]->Rel_attrs.item[
				                                attrs[index].columnid - base[j]
						                                                   ];
			}
		}

		project->indexMap[index] = attrs[index].columnid;
		resultRel->Rel_attrs.item_num ++;
		i ++;
	}
	resultRel->Rel_maxtuple_len = uGetMaxTupleLen(&(resultRel->Rel_attrs));
	project->desRelation = resultRel;
	project->desTuple = Tup_CreateTuple(resultRel);
	project->state.tag = State_Projection;
	project->state.baseRelation = rels;
	project->state.baseRelationNumber = relNum;
	project->state.child[0] = NULL;
	project->state.child[1] = NULL;
	return project;
}

void DestoryCreateTableState(CreateTableState state)
{
	Rel_vDestoryRelation(state->targetRel);
}

void DestoryDropTableState(DropTableState state)
{
	Rel_vDestoryRelation(state->targetRel);
}

void DestoryUpdateTableState(UpdateTableState state)
{
//	Rel_vDestoryRelation(state->targetRel);
	Tup_vDestroyTuple(state->newtuple);
	uint i = 0;
	for(i = 0; i < state->setColumnNum; i ++)
	{
		DestoryExpr(state->expr[i]);
	}
}

void DestoryInsertTableState(InsertTableState state)
{
	Rel_vDestoryRelation(state->targetRel);
	Tup_vDestroyTuple(state->tuple);
}

void DestoryDeleteTableState(DeleteTableState state)
{
	Rel_vDestoryRelation(state->targetRel);
}

void DestoryScanState(ScanState state)
{
	switch(state->method)
	{
	case SeqScan:
		DestorySeqScanState((SeqScanState)state);
		break;
	}
	Rel_vDestoryRelation(state->relation);
	Tup_vDestroyTuple(state->resultTuple);
}

void DestorySeqScanState(SeqScanState state)
{
	// nothing to release
}

void DestoryJoinState(JoinState state)
{
	switch (state->method)
	{
	case NestLoop:
		DestoryNestLoopJoinState((NestLoopJoinState)state);
		break;
	}
	Rel_vDestoryRelation(state->resultTuple->TD_Relation);
	Tup_vDestroyTuple(state->resultTuple);
}

void DestoryNestLoopJoinState(NestLoopJoinState state)
{
	uint i = 0;
	for(i = 0; i < state->innerArraysize; i ++)
	{
		Tup_vDestroyTuple(state->innnerArray[i]);
	}
	TMF_DestoryFile(state->tmf);
}

void DestoryProjectionState(ProjectionState state)
{
	//no need to destory srcRelation because it should belong to another operator
	// and it will be destoryed by its owner
	Rel_vDestoryRelation(state->desRelation);
	Tup_vDestroyTuple(state->desTuple);
}

void DestorySelectState(SelectState state)
{
	// nothing to relaease
}
