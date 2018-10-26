/*
 * expr.c
 *
 *  Created on: Nov 28, 2017
 *      Author: jyh
 */

#include "utils/expr.h"

ExprElement Expr_Calculate(Expr expr, Tuple tuple)
{
	ExprElement result;
	if(expr == NULL)
	{
		result.tag = exprelem_bool;
		result.data.logic = true;
		return result;
	}
	ExprElement opr1, opr2, opr3;
	ExprElement elem;
	switch(expr->op)
	{
	case OP_NONE:
		elem = expr->content.element;
		switch ( expr->content.element.tag)
		{
		case exprelem_column:
			result.data.item = Tup_getAttr(tuple, elem.data.column.columnid);
			result.tag = exprelem_item;
			break;
		default:
			result = elem;
		}
		break;
	case OP_ADD:
	case OP_SUB:
	case OP_MUL:
	case OP_DIV:
	case OP_MOD:
		opr1 = Expr_Calculate(expr->content.operator[0], tuple);
		opr2 = Expr_Calculate(expr->content.operator[1], tuple);
		if((opr1.tag != exprelem_item) || (opr2.tag != exprelem_item))
		{
			vmlog(ERROR, "Expr_Calculate---expression operator not match");
		}
		result.tag = exprelem_item;
		result.data.item = calculateHeapTupleItem(&(opr1.data.item), &(opr2.data.item), expr->op);
		break;

	case OP_AND:
		opr1 = Expr_Calculate(expr->content.operator[0], tuple);
		opr2 = Expr_Calculate(expr->content.operator[1], tuple);
		if((opr1.tag != exprelem_bool) || (opr2.tag != exprelem_bool))
		{
			vmlog(ERROR, "Expr_Calculate---expression operator not match");
		}
		result.tag = exprelem_bool;
		result.data.logic = opr1.data.logic && opr2.data.logic;
		break;
	case OP_OR:
		opr1 = Expr_Calculate(expr->content.operator[0], tuple);
		opr2 = Expr_Calculate(expr->content.operator[1], tuple);
		if((opr1.tag != exprelem_bool) || (opr2.tag != exprelem_bool))
		{
			vmlog(ERROR, "Expr_Calculate---expression operator not match");
		}
		result.tag = exprelem_bool;
		result.data.logic = opr1.data.logic || opr2.data.logic;
		break;

	case OP_LESS:
		opr1 = Expr_Calculate(expr->content.operator[0], tuple);
		opr2 = Expr_Calculate(expr->content.operator[1], tuple);
		if((opr1.tag != exprelem_item) || (opr2.tag != exprelem_item))
		{
			vmlog(ERROR, "Expr_Calculate---expression operator not match");
		}
		result.tag = exprelem_bool;
		result.data.logic = cmpHeapTupleItem(&(opr1.data.item), &(opr2.data.item)) < 0;
		break;
	case OP_EQUAL:
		opr1 = Expr_Calculate(expr->content.operator[0], tuple);
		opr2 = Expr_Calculate(expr->content.operator[1], tuple);
		if((opr1.tag != exprelem_item) || (opr2.tag != exprelem_item))
		{
			vmlog(ERROR, "Expr_Calculate---expression operator not match");
		}
		result.tag = exprelem_bool;
		result.data.logic = cmpHeapTupleItem(&(opr1.data.item), &(opr2.data.item)) == 0;
		break;
	case OP_NOT_EQUAL:
		opr1 = Expr_Calculate(expr->content.operator[0], tuple);
		opr2 = Expr_Calculate(expr->content.operator[1], tuple);
		if((opr1.tag != exprelem_item) || (opr2.tag != exprelem_item))
		{
			vmlog(ERROR, "Expr_Calculate---expression operator not match");
		}
		result.tag = exprelem_bool;
		result.data.logic = cmpHeapTupleItem(&(opr1.data.item), &(opr2.data.item)) != 0;
		break;
	case OP_GREATER:
		opr1 = Expr_Calculate(expr->content.operator[0], tuple);
		opr2 = Expr_Calculate(expr->content.operator[1], tuple);
		if((opr1.tag != exprelem_item) || (opr2.tag != exprelem_item))
		{
			vmlog(ERROR, "Expr_Calculate---expression operator not match");
		}
		result.tag = exprelem_bool;
		result.data.logic = cmpHeapTupleItem(&(opr1.data.item), &(opr2.data.item)) > 0;
		break;
	case OP_LESS_EQUAL:
		opr1 = Expr_Calculate(expr->content.operator[0], tuple);
		opr2 = Expr_Calculate(expr->content.operator[1], tuple);
		if((opr1.tag != exprelem_item) || (opr2.tag != exprelem_item))
		{
			vmlog(ERROR, "Expr_Calculate---expression operator not match");
		}
		result.tag = exprelem_bool;
		result.data.logic = cmpHeapTupleItem(&(opr1.data.item), &(opr2.data.item)) <= 0;
		break;
	case OP_GREATER_EQUAL:
		opr1 = Expr_Calculate(expr->content.operator[0], tuple);
		opr2 = Expr_Calculate(expr->content.operator[1], tuple);
		if((opr1.tag != exprelem_item) || (opr2.tag != exprelem_item))
		{
			vmlog(ERROR, "Expr_Calculate---expression operator not match");
		}
		result.tag = exprelem_bool;
		result.data.logic = cmpHeapTupleItem(&(opr1.data.item), &(opr2.data.item)) >= 0;
		break;

	case OP_TRUE:
	case OP_NOT_FALSE:
		opr1 = Expr_Calculate(expr->content.operator[0], tuple);
		if(opr1.tag != exprelem_bool)
		{
			vmlog(ERROR, "Expr_Calculate---expression operator not match");
		}
		result.tag = exprelem_bool;
		result.data.logic = opr1.data.logic == true;
		break;
	case OP_NOT_TRUE:
	case OP_FALSE:
		opr1 = Expr_Calculate(expr->content.operator[0], tuple);
		if(opr1.tag != exprelem_bool)
		{
			vmlog(ERROR, "Expr_Calculate---expression operator not match");
		}
		result.tag = exprelem_bool;
		result.data.logic = opr1.data.logic == false;
		break;
	}
	return result;
}

Expr getExpr(pSimpleExpr simpleExpr, Relation rels[], uint relNum, uint base[])
{
	Expr expr;
	switch(simpleExpr->op_type)
	{
	case OP_NONE:
		expr = getExprfromBaseExpr((pBaseExpr)simpleExpr->left, rels, relNum, base);
		break;
	case OP_ADD:
	case OP_SUB:
	case OP_MUL:
	case OP_DIV:
	case OP_MOD:
	case OP_LESS:
	case OP_GREATER:
	case OP_EQUAL:
	case OP_LESS_EQUAL:
	case OP_GREATER_EQUAL:
	case OP_NOT_EQUAL:
	case OP_AND:
	case OP_OR:
	{
		expr = vmalloc0(sizeof(ExprData));
		expr->op = simpleExpr->op_type;
		expr->content.operator[0] = getExpr((pSimpleExpr)simpleExpr->left, rels, relNum, base);
		expr->content.operator[1] = getExpr((pSimpleExpr)simpleExpr->right, rels, relNum, base);
		expr->relsOidArray = vmalloc(sizeof(Oid) * relNum);
//		expr->relsNumber = expr->content.operator[0]->relsNumber + expr->content.operator[1]->relsNumber;
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
		break;
	}
	case OP_BETWEEN:
	case OP_NOT_BETWEEN:
	{
		expr = vmalloc0(sizeof(ExprData));
		expr->op = simpleExpr->op_type;
		expr->content.operator[0] = getExpr((pSimpleExpr)simpleExpr->left, rels,relNum, base);
		expr->content.operator[1] = getExpr((pSimpleExpr)simpleExpr->middle, rels, relNum, base);
		expr->content.operator[2] = getExpr((pSimpleExpr)simpleExpr->right, rels,relNum, base);
		expr->relsOidArray = vmalloc(sizeof(Oid) * relNum);
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
		for(i = 0; i < expr->content.operator[2]->relsNumber; i++)
		{
			uint k = 0;
			for(k = 0; k < j ; k ++)
			{
				if(expr->content.operator[2]->relsOidArray[i] == expr->relsOidArray[k])
				{
					break;
				}
			}
			if(k < j) //repeat
			{
				continue;
			}
			 expr->relsOidArray[j] = expr->content.operator[2]->relsOidArray[i];
			 j ++;
		}
		expr->relsNumber = j;
		break;
	}
	case OP_NULL:
	case OP_NOT_NULL:
	case OP_TRUE:
	case OP_NOT_TRUE:
	case OP_FALSE:
	case OP_NOT_FALSE:
	{
		expr = vmalloc0(sizeof(ExprData));
		expr->op = simpleExpr->op_type;
		expr->content.operator[0] = getExpr((pSimpleExpr)simpleExpr->left, rels, relNum, base);
		expr->relsOidArray = vmalloc(sizeof(Oid) * relNum);
		uint i = 0;
		uint j = 0;
		for(i = 0; i < expr->content.operator[0]->relsNumber; i++)
		{
			 expr->relsOidArray[j] = expr->content.operator[0]->relsOidArray[i];
			 j ++;
		}
		expr->relsNumber = j;
		break;
	}
	default:
		expr = NULL;
		vmlog(ERROR, "analyze --- getExpr invalid type");

	}
	vfree((void**)&(simpleExpr));
	return expr;
}

Expr getExprfromBaseExpr(pBaseExpr baseExpr, Relation rels[], uint relNum, uint base[])
{
	Expr expr;
	switch(baseExpr->type)
	{
	case EXPR_NONE:
	{
		expr = NULL;
		break;
	}
	case EXPR_NUMBER:
	{
		expr = vmalloc0(sizeof(ExprData));
		expr->content.element.tag = exprelem_item;
		setHeapTupleItemByStr(&(expr->content.element.data.item), INTEGER, baseExpr->data, 0);
		break;
	}
	case EXPR_REAL:
	{
		expr = vmalloc0(sizeof(ExprData));
		expr->content.element.tag = exprelem_item;
		setHeapTupleItemByStr(&(expr->content.element.data.item), DOUBLE, baseExpr->data, 0);
		break;
	}
	case EXPR_STRING:
	{
		expr = vmalloc0(sizeof(ExprData));
		expr->content.element.tag = exprelem_item;
		expr->content.element.data.item.itemdata.char_type.length = strlen((char*)(baseExpr->data)) + 1;
		setHeapTupleItemByStr(&(expr->content.element.data.item), CHAR, baseExpr->data, strlen((char*)(baseExpr->data)));
		break;
	}
	case EXPR_COLUME:
	{
		expr = vmalloc0(sizeof(ExprData));
		expr->content.element.tag = exprelem_column;
		expr->content.element.data.column = getAttrElem((pAttrItem)baseExpr->data, rels, relNum, base);
		expr->relsNumber = 1;
		expr->relsOidArray = vmalloc0(sizeof(Oid));
		expr->relsOidArray[0] = expr->content.element.data.column.tableOid;
		break;
	}
	case EXPR_SIMPLE:
	{
		expr = getExpr((pSimpleExpr)baseExpr->data, rels, relNum, base);
		break;
	}
	default:
		expr = NULL;
		vmlog(ERROR, "analyze --- getExprfromBaseExpr invalid type");
	}
	vfree((void**)&(baseExpr->data));
	vfree((void**)&(baseExpr));
	expr->op = OP_NONE;
	return expr;
}

/**
 * Attribute_Elem
 * get the clomunid and the table id of the
 * */
Attribute_Elem getAttrElem(pAttrItem attrItem, Relation rels[], uint relNum, uint base[])
{
	Attribute_Elem elem;
	bool found = false;
	uint i = 0;
	for(i = 0; i < relNum; i ++)
	{
		if((attrItem->table != NULL) && (strcmp(rels[i]->Rel_Name, attrItem->table) != 0))
		{
			continue;
		}
		uint j = 0;
		for(j = 0; j < rels[i]->Rel_attrs.item_num; j ++)
		{
			if(strcmp(rels[i]->Rel_attrs.item[j].key, attrItem->name) == 0)
			{
				if(found)
				{
					vmlog(ERROR, "more than one table hash the attribute:%s", attrItem->name);
				}else
				{
					found = true;
					elem.columnid = j + base[i];
					elem.tableOid = rels[i]->Rel_rid;
				}
			}
		}
	}
	if(!found)
	{
		vmlog(ERROR, "attribute:%s is not exist", attrItem->name);
	}
	return elem;
}

uint getOperatorNum(OpType op)
{
	switch(op)
	{
	case OP_NONE:
	case OP_NULL:
	case OP_NOT_NULL:
	case OP_TRUE:
	case OP_NOT_TRUE:
	case OP_FALSE:
	case OP_NOT_FALSE:
		return 1;
	case OP_ADD:
	case OP_SUB:
	case OP_MUL:
	case OP_DIV:
	case OP_MOD:
	case OP_LESS:
	case OP_EQUAL:
	case OP_NOT_EQUAL:
	case OP_GREATER:
	case OP_LESS_EQUAL:
	case OP_GREATER_EQUAL:
	case OP_AND:
	case OP_OR:
		return 2;
	case OP_BETWEEN:
	case OP_NOT_BETWEEN:
		return 3;
	}
}


void DestoryExpr(Expr expr)
{
	if(expr == NULL)
	{
		return;
	}
	if((expr->op == OP_NONE))
	{
		if((expr->content.element.tag == exprelem_item))
		{
			vDestroyHeapTupleItem(&(expr->content.element.data.item));
		}
	}else
	{
		uint i = 0;
		for (i = 0; i < getOperatorNum(expr->op); i++)
		{
			DestoryExpr(expr->content.operator[i]);
		}
	}
	vfree((void**)(&expr));
}
