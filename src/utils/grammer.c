/*
 * grammer.c
 *
 *  Created on: NOV 15, 2017
 *      Author: bsl
 *        desc: sematics analysis function impletation
 */

#include "grammer.h"
char* tagStr[]={
		"NODE_NONE",
		"NODE_CHAR",
		"NODE_SELECT_TABLE",
		"NODE_INSERT_TABLE",
		"NODE_DELETE_TABLE",
		"NODE_CREATE_INDEX",
		"NODE_CREATE_TABLE",
		"NODE_DROP_INDEX",
		"NODE_DROP_TABLE",
		"NODE_EXISTS",
		"NODE_CONSTRAINT",
		"NODE_ATTR_TYPE",
		"NODE_ATTR_DEFINITION",
		"NODE_CHILD_CONSTRAINT",
		"NODE_CHILD_CONSTRAINT_DATA",
		"NODE_ATTR_LIST",
		"NODE_ATTR_ITEM",
		"NODE_VALUE_ITEM",
		"NODE_LIMIT_ITEM",
		"NODE_BASE_EXPR",
		"NODE_SIMPLE_EXPR",
		"NODE_WHERE_DEFINITION",
		"NODE_ORDER_ITEM"
};
// 根据传入的节点类型，初始化不同的节点
void *makeNode(NodeTag tag)
{
//	vmlog(GRAMMER_LOG, "makeNode(%s)", tagStr[tag]);
    switch(tag){
        case NODE_SELECT_TABLE:
            {
                pSelectTable p  = (pSelectTable) vmalloc(sizeof(SelectTable));
                p->tag          = tag;
                p->distinct     = DISTINCT_FALSE;
                p->targets      = NULL;
                p->froms        = NULL;
                p->tables       = NULL;
                p->order        = NULL;
                p->limit        = NULL;
                p->where        = NULL;
                return (void *)p;
            }
        case NODE_INSERT_TABLE:
            {
                pInsertTable p  = (pInsertTable) vmalloc(sizeof(InsertTable));
                p->tag          = tag;
                p->name         = NULL;
                p->attrs        = NULL;
                p->values       = NULL;
                return p;
            }
        case NODE_DELETE_TABLE:
            {
                pDeleteTable p  = (pDeleteTable) vmalloc(sizeof(DeleteTable));
                p->tag          = tag;
                p->name         = NULL;
                p->where        = NULL;
                return (void *)p;       
            }
        case NODE_CREATE_INDEX:
            {
                pCreateIndex p  = (pCreateIndex) vmalloc(sizeof(CreateIndex));
                p->tag          = tag;
                p->unique       = UNIQUE_FALSE;
                p->name         = NULL;
                p->type         = INDEX_NONE;
                p->table        = NULL;
                p->attrs        = NULL;
                return (void *)p;       
            }
        case NODE_CREATE_TABLE:
            {
                pCreateTable p  = (pCreateTable) vmalloc(sizeof(CreateTable));
                p->tag          = tag;
                p->exists       = NULL;
                p->attrs        = NULL;
                return (void *)p;       
            }
        case NODE_DROP_INDEX:
            {
                pDropIndex p    = (pDropIndex) vmalloc(sizeof(DropIndex));
                p->tag          = tag;
                p->table        = NULL;
                p->name         = NULL;
                return (void *)p;       
            }
        case NODE_DROP_TABLE:
            {
                pDropTable p    = (pDropTable) vmalloc(sizeof(DropTable));
                p->tag          = tag;
                p->exists       = NULL;
                return (void *)p;       
            }
        case NODE_EXISTS:
            {
                pExists p    = (pExists) vmalloc(sizeof(Exists));
                p->tag       = tag;
                p->flag      = false;
                p->name      = NULL;
                return (void *)p;       
            }
        case NODE_CONSTRAINT:
            {
                pConstraint p   = (pConstraint) vmalloc(sizeof(Constraint));
                p->tag          = tag;
                p->constraints  = NULL;
                return (void *)p;      
            }
        case NODE_ATTR_TYPE:
            {
                pAttrType p     = (pAttrType) vmalloc(sizeof(AttrType));
                p->tag          = tag;
                p->type         = NONE;
                p->size         = ZERO;
                return (void *)p;
            }
        case NODE_ATTR_DEFINITION:
            {
                pAttrDefinition p   = (pAttrDefinition) vmalloc(sizeof(AttrDefinition));
                p->tag              = tag;
                p->name             = NULL;
                p->type             = NULL;
                p->constraint       = NULL;
                return (void *)p;      
            }
        case NODE_CHILD_CONSTRAINT_DATA:
            {
                pChildConstraintData p  = (pChildConstraintData) vmalloc(sizeof(ChildConstraintData));
                p->tag                  = tag;
                p->type                 = CONSTRAINT_NONE;
                p->data                 = NULL;
                return (void *)p;
            }
        case NODE_CHILD_CONSTRAINT:
            {
                pChildConstraint p  = (pChildConstraint) vmalloc(sizeof(ChildConstraint));
                p->tag              = tag;
                p->type             = CONSTRAINT_NONE;
                p->data             = NULL;
                return (void *)p;       
            }
        case NODE_ATTR_LIST:
            {
                pAttrList p         = (pAttrList) vmalloc(sizeof(AttrList));
                p->tag              = tag;
                p->type             = NODE_NONE;
                p->list             = NULL;
                p->size             = ZERO;
                return (void *)p;       
            }
        case NODE_ATTR_ITEM:
            {
                pAttrItem p         = (pAttrItem) vmalloc(sizeof(AttrItem));
                p->tag              = tag;
                p->table            = NULL;
                p->name             = NULL;
                return (void *)p;       
            }
        case NODE_VALUE_ITEM:
            {
                pValueItem p        = (pValueItem) vmalloc(sizeof(ValueItem));
                p->tag              = tag;
                p->type             = NONE;
                p->value            = NULL;
                return (void *)p;       
            }
        case NODE_LIMIT_ITEM:
            {
                pLimitItem p        = (pLimitItem) vmalloc(sizeof(LimitItem));
                p->tag              = tag;
                p->type             = LIMIT_NONE;
                p->begin            = ZERO;
                p->end              = ZERO;
                return (void *)p;
            }
        case NODE_BASE_EXPR:
            {
                pBaseExpr p         = (pBaseExpr) vmalloc(sizeof(BaseExpr));
                p->tag              = tag;
                p->type             = EXPR_NONE;
                p->data             = NULL;
                return (void *)p;       
            }
        case NODE_SIMPLE_EXPR:
            {
                pSimpleExpr p       = (pSimpleExpr) vmalloc(sizeof(SimpleExpr));
                p->tag              = tag;
                p->op_type          = OP_NONE;
                p->op_num           = ZERO;
                p->left             = NULL;
                p->right            = NULL;
                p->middle           = NULL;
                return (void *)p;       
            }
        case NODE_WHERE_DEFINITION:
            {
                pWhereDefinition p  = (pWhereDefinition) vmalloc(sizeof(WhereDefinition));
                p->tag              = tag;
                p->flag             = true;
                p->expr             = NULL;
                return (void *)p;       
            }
        case NODE_SET_DEFINITION:
            {
                pSetDefinition p    = (pSetDefinition) vmalloc(sizeof(SetDefinition));
                p->tag              = tag;
                p->attr             = NULL;
                p->expr             = NULL;
                return (void *)p;
            }
        case NODE_ORDER_ITEM:
            {
                pOrderItem p        = (pOrderItem) vmalloc(sizeof(OrderItem));
                p->tag              = tag;
                p->type             = ORDER_NONE;
                p->attr             = NULL;
                return (void *)p;
            }
        case NODE_UPDATE_TABLE:
            {
                pUpdateTable p      = (pUpdateTable) vmalloc(sizeof(UpdateTable));
                p->name             = NULL;
                p->list             = NULL;
                p->where            = NULL;
                return (void *)p;
            }
        case NODE_NONE:
        default: 
            vmlog(GRAMMER_LOG, "ERROR: makeNode(%d)", tag);
            exit(FAILURE);
    }
}

// 输出 stucct Exists
void vPrintExists(pExists exists)
{
    if(exists == NULL)
    {
        vmlog(GRAMMER_LOG, "Exists: {flag=false}");
    }else
    {
        vmlog(GRAMMER_LOG, "Exists: {flag=%d, name=%s}", exists->flag, exists->name);
    }
}

// 输出 struct AttrItem
void vPrintAttrItem(pAttrItem attrItem)
{
    if(attrItem == NULL)
    {
        vmlog(GRAMMER_LOG, "AttrItem: {tableName=NULL, columeName=NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "AttrItem: {tableName=%s, columeName=%s}", attrItem->table, attrItem->name);
    }
}

// struct ValueItem
void vPrintValueItem(pValueItem valueItem){
}

// struct LimitItem
void vPrintLimitItem(pLimitItem limitItem)
{
    if(limitItem == NULL)
    {
        vmlog(GRAMMER_LOG, "LimitItem: {NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "LimitItem: {begin=%d, end=%d}", limitItem->begin, limitItem->end);
    }
}

// list
void vPrintGrammerList(List *L, NodeTag type)
{
    if(*L == NULL)
    {
        vmlog(GRAMMER_LOG, "List: {NULL}");
    }else{
        List p = *L;
        List q = p->next;
        uint index = 0;

        while(q != NULL)
        {
            vmlog(GRAMMER_LOG, "List<%d>", index);
            switch(type){
                case NODE_CHAR:
                    vmlog(GRAMMER_LOG, "cell=%s", q->cell);
                    break;
                case NODE_CONSTRAINT:
                    vPrintChildConstraint((pChildConstraint)(q->cell));
                    break;
                case NODE_ATTR_ITEM:
                    vPrintAttrItem((pAttrItem)(q->cell));
                    break;
                case NODE_ATTR_DEFINITION:
                    vPrintAttrDefinition((pAttrDefinition)(q->cell));
                    break;
                case NODE_SET_DEFINITION:
                	vPrintSetDefinition((pSetDefinition)(q->cell));
                	break;
                default:
                    vmlog(GRAMMER_LOG, "List: {type=%d}", type);
                    break;
            }
            p = q;
            q = p->next;
            index ++;
        }
    }
}

// struct AttrList
void vPrintAttrList(pAttrList attrList)
{
   if(attrList == NULL)
   {
       vmlog(GRAMMER_LOG, "AttrList: {NULL}");
   }else
   {
       vmlog(GRAMMER_LOG, "AttrList: {size=%d, type=%d}", attrList->size, attrList->type);
       vPrintGrammerList(&(attrList->list), attrList->type);
   }
}

// struct OrderItem
void vPrintOrderItem(pOrderItem orderItem)
{
    if(orderItem == NULL)
    {
        vmlog(GRAMMER_LOG, "OrderItem: {NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "OrderItem: {type=%d}", orderItem->type);
        vmlog(GRAMMER_LOG, "OrderItem: Colume: ---");
        vPrintAttrItem(orderItem->attr);
    }
}

// struct AttrType
void vPrintAttrType(pAttrType attrType)
{
    if(attrType == NULL)
    {
        vmlog(GRAMMER_LOG, "AttrType: {NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "AttrType: {type=%d, size=%d}", attrType->type, attrType->size);
    }
}

// struct ChildConstraintData
void vPrintChildConstraintData(pChildConstraintData childConstraintData)
{
    if(childConstraintData == NULL)
    {
        vmlog(GRAMMER_LOG, "ChildConstraintData: {NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "ChildConstraintData: {type=%d, data=%s}", childConstraintData->type, childConstraintData->data);
    }
}

// struct ChildConstraint
void vPrintChildConstraint(pChildConstraint childConstraint)
{
    if(childConstraint == NULL)
    {
        vmlog(GRAMMER_LOG, "ChildConstraint: {NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "ChildConstraint: {type=%d}", childConstraint->type);
        vmlog(GRAMMER_LOG, "ChildConstraint: ChildConstraintData: ---");
        vPrintChildConstraintData(childConstraint->data);
    }
}

// struct Constraint
void vPrintConstraint(pConstraint constraint)
{
    if(constraint == NULL)
    {
        vmlog(GRAMMER_LOG, "Constraint: {NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "Constraint: AttrList: ---");
        vPrintAttrList(constraint->constraints);
    }
}

// struct AttrDefinition
void vPrintAttrDefinition(pAttrDefinition attrDefinition)
{
    if(attrDefinition == NULL)
    {
        vmlog(GRAMMER_LOG, "AttrDefinition: {NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "AttrDefinition: {name=%s}", attrDefinition->name);
        vmlog(GRAMMER_LOG, "AttrDefinition: AttrType: ---");
        vPrintAttrType(attrDefinition->type);
        vmlog(GRAMMER_LOG, "AttrDefinition: Constraint: ---");
        vPrintConstraint(attrDefinition->constraint);
    }
}

// struct BaseExpr
void vPrintBaseExpr(pBaseExpr baseExpr)
{
    if(baseExpr == NULL)
    {
        vmlog(GRAMMER_LOG, "BaseExpr: {NULL}");
    }else
    {
        if(baseExpr->type == EXPR_SIMPLE)
        {
            vmlog(GRAMMER_LOG, "BaseExpr: SimpleExpr: ---");
            vPrintSimpleExpr(baseExpr->data);
        }else if(baseExpr->type == EXPR_COLUME)
        {
            vmlog(GRAMMER_LOG, "BaseExpr: {type=%d}", baseExpr->type);
            vmlog(GRAMMER_LOG, "BaseExpr: AttrItem: ---");
            vPrintAttrItem(baseExpr->data);
        }else
        {
            vmlog(GRAMMER_LOG, "BaseExpr: {type=%d, data=%s}", baseExpr->type, baseExpr->data);
        }
    }
}

// struct WhereDefinition
void vPrintSimpleExpr(pSimpleExpr simpleExpr)
{
    if(simpleExpr == NULL)
    {
        vmlog(GRAMMER_LOG, "SimpleExpr: {NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "SimpleExpr: {op_type=%d, op_num=%d}", simpleExpr->op_type, simpleExpr->op_num);
        
        if(simpleExpr->op_type == OP_NONE)
        {
            vmlog(GRAMMER_LOG, "SimpleExpr: Base: ---");
            vPrintBaseExpr(simpleExpr->left);
        }else
        {
            vmlog(GRAMMER_LOG, "SimpleExpr: Left: ---");
            vPrintSimpleExpr(simpleExpr->left);
            vmlog(GRAMMER_LOG, "SimpleExpr: Middle: ---");
            vPrintSimpleExpr(simpleExpr->middle);
            vmlog(GRAMMER_LOG, "SimpleExpr: Right: ---");
            vPrintSimpleExpr(simpleExpr->right);
        }
    }
}

// struct WhereDefinition
void vPrintWhereDefinition(pWhereDefinition whereDefinition)
{
    if(whereDefinition == NULL || whereDefinition->flag == false)
    {
        vmlog(GRAMMER_LOG, "WhereDefinition: {NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "WhereDefinition: expr: ---");
        vPrintSimpleExpr(whereDefinition->expr);
    }
}

// struct SetDefinition
void vPrintSetDefinition(pSetDefinition setDefinition)
{
	if(setDefinition == NULL)
	{
		vmlog(GRAMMER_LOG, "SetDefiniton: {NULL}");
	}else
	{
		vmlog(GRAMMER_LOG, "SetDefinition: attr: ---");
		vPrintAttrItem(setDefinition->attr);
		vmlog(GRAMMER_LOG, "SetDefinition: expr: ---");
		vPrintSimpleExpr(setDefinition->expr);
	}
}

// print update table
void vPrintUpdateTable(pUpdateTable updateTable)
{
	if(updateTable == NULL)
	{
		vmlog(GRAMMER_LOG, "UpdateTable: {NULL}");
	}else
	{
		vmlog(GRAMMER_LOG, "UpdateTable: {name=%s}", updateTable->name);

		vmlog(GRAMMER_LOG, "UpdateTable: list: ---");
		vPrintAttrList(updateTable->list);

		vmlog(GRAMMER_LOG, "UpdateTable: Where: ---");
		vPrintWhereDefinition(updateTable->where);
	}
}

// 输出 delete
void vPrintDeleteTable(pDeleteTable deleteTable)
{
    if(deleteTable == NULL)
    {
        vmlog(GRAMMER_LOG, "DeleteTable: {NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "DeleteTable: {name=%s}", deleteTable->name);
        vmlog(GRAMMER_LOG, "DeleteTable: Where: ---");
        vPrintWhereDefinition(deleteTable->where);
    }
}

// 输出 select
void vPrintSelectTable(pSelectTable selectTable)
{
    if(selectTable == NULL)
    {
        vmlog(GRAMMER_LOG, "SelectTable: {NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "SelectTable: {distinct=%d}", selectTable->distinct);
        vmlog(GRAMMER_LOG, "SelectTable: Targets: ---");
        vPrintAttrList(selectTable->targets);
        vmlog(GRAMMER_LOG, "SelectTable: Froms: ---");
        vPrintAttrList(selectTable->froms);
        vmlog(GRAMMER_LOG, "SelectTable: Where: ---");
        vPrintWhereDefinition(selectTable->where);
        vmlog(GRAMMER_LOG, "SelectTable: Order: ---");
        vPrintOrderItem(selectTable->order);
        vmlog(GRAMMER_LOG, "SelectTable: Limit: ---");
        vPrintLimitItem(selectTable->limit);
    }
}

// 输出 insert
void vPrintInsertTable(pInsertTable insertTable)
{
    if(insertTable == NULL)
    {
        vmlog(GRAMMER_LOG, "InsertTable: {NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "InsertTable: {name=%s}", insertTable->name);
        vmlog(GRAMMER_LOG, "InsertTable: Attrs: ---");
        vPrintAttrList(insertTable->attrs);
        vmlog(GRAMMER_LOG, "InsertTable: Values: ---");
        vPrintAttrList(insertTable->values);
    }
}

// 输出 create index
void vPrintCreateIndex(pCreateIndex createIndex)
{
    if(createIndex == NULL)
    {
        vmlog(GRAMMER_LOG, "CreateIndex: {NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "CreateIndex: {unique=%d, name=%s, type=%d, table=%s}", createIndex->unique, createIndex->name, createIndex->type, createIndex->table);
        vmlog(GRAMMER_LOG, "CreateIndex: Attrs: ---");
        vPrintAttrList(createIndex->attrs);
    }
}

// 输出 create table
void vPrintCreateTable(pCreateTable createTable)
{
    if(createTable == NULL)
    {
        vmlog(GRAMMER_LOG, "CreateTable: {NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "CreateTable: Exists: ---");
        vPrintExists(createTable->exists);
        vmlog(GRAMMER_LOG, "CreateTable: Attrs: ---");
        vPrintAttrList(createTable->attrs);
    }
}

// 输出 drop index
void vPrintDropIndex(pDropIndex dropIndex)
{
    if(dropIndex == NULL)
    {
        vmlog(GRAMMER_LOG, "DropIndex: {NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "DropIndex: {table=%s, name=%s}", dropIndex->table,dropIndex->name);
    }
}

// 输出 drop table
void vPrintDropTable(pDropTable dropTable)
{
    if(dropTable == NULL)
    {
        vmlog(GRAMMER_LOG, "DropTable: {NULL}");
    }else
    {
        vmlog(GRAMMER_LOG, "DropTable: Exists: ---");
        vPrintExists(dropTable->exists);
    }
}

// 根据 sql 语句类型，分别调用不同语句的输出函数
void vPrintSql(SqlManager sql)
{
    switch(sql.tag)
    {
        case NODE_DROP_TABLE:
            vmlog(GRAMMER_LOG, "--- Sql: Drop Table ---");
            vPrintDropTable((pDropTable)(sql.sql));
            break;
        case NODE_DROP_INDEX: 
            vmlog(GRAMMER_LOG, "--- Sql: Drop Index ---");
            vPrintDropIndex((pDropIndex)(sql.sql));
            break;
        case NODE_CREATE_TABLE:
            vmlog(GRAMMER_LOG, "--- Sql: Create Table ---");
            vPrintCreateTable((pCreateTable)(sql.sql));
            break;
        case NODE_CREATE_INDEX:
            vmlog(GRAMMER_LOG, "--- Sql: Create Index ---");
            vPrintCreateIndex((pCreateIndex)(sql.sql));
            break;
        case NODE_INSERT_TABLE:
            vmlog(GRAMMER_LOG, "--- Sql: Insert Table ---");
            vPrintInsertTable((pInsertTable)(sql.sql));
            break;
        case NODE_SELECT_TABLE:
            vmlog(GRAMMER_LOG, "--- Sql: Select Table ---");
            vPrintSelectTable((pSelectTable)(sql.sql));
            break;
        case NODE_DELETE_TABLE:
            vmlog(GRAMMER_LOG, "--- Sql: Delete Table ---");
            vPrintDeleteTable((pDeleteTable)(sql.sql));
            break;
        case NODE_UPDATE_TABLE:
        	vmlog(GRAMMER_LOG, "--- Sql: Update Table ---");
        	vPrintUpdateTable((pUpdateTable)(sql.sql));
        	break;
        default:
            vmlog(GRAMMER_LOG, "--- Sql: %d---", sql.tag);
            break;
    }
}
