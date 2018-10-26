/*
 * expr.h
 *
 *  Created on: Nov 28, 2017
 *      Author: jyh
 */

#ifndef EXPR_H_
#define EXPR_H_

#include "basic.h"
#include "utils/node.h"
#include "utils/tool.h"
#include "mparser/parseTuple.h"
#include "storage/tuple.h"
#include "grammer.h"

typedef enum ExprElementTag{
	exprelem_none,
	exprelem_item,
	exprelem_column,
	exprelem_bool
}ExprElementTag;

typedef struct Attribute_Elem{
	Oid tableOid;  //the id in the base table
	uint columnid; // the id in the result temp table
}Attribute_Elem;

typedef struct ExprElement{
	ExprElementTag tag;
	union data{
		HeapTupleItem  	item;
		Attribute_Elem 	column;
		bool			logic;
	}data;
}ExprElement;

typedef struct Expr{
	OpType op; //operation
	union content{
		struct Expr* operator[3];
		ExprElement element;
	}content;
	Oid*		relsOidArray;//the oid of the relations the expression used
	uint 		relsNumber;
//	ExprElement result;

}ExprData, *Expr;

ExprElement Expr_Calculate(Expr expr, Tuple tuple);
Expr getExpr(pSimpleExpr simpleExpr, Relation rels[], uint relNum, uint base[]);
Expr getExprfromBaseExpr(pBaseExpr baseExpr, Relation rels[], uint relNum, uint base[]);
Attribute_Elem getAttrElem(pAttrItem attrItem, Relation rels[], uint relNum, uint base[]);
void DestoryExpr(Expr expr);
#endif /* EXPR_H_ */
