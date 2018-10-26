/*
 * grammer.h
 *
 *  Created on: NOV 15, 2017
 *      Author: bsl
 *        desc: sematics analysis data structure and function declaration
 */

#ifndef GRAMMER_H_
#define GRAMMER_H_

#include "utils/LinkList.h"
#include "basic.h"
#include "mparser/parseTable.h"
#include "utils/node.h"
#include "utils/tool.h"


// data structure of exists_clause
typedef struct Exists{
    NodeTag tag;
    bool flag;
    char* name;
}Exists, *pExists;

// data structure of single attribute
typedef struct AttrItem{
    NodeTag tag;
    char* table;            // table name
    char* name;             // attribute name
}AttrItem, *pAttrItem;

// data structure of single value
typedef struct ValueItem{
    NodeTag tag;
    DataType type;
    char* value;
}ValueItem, *pValueItem;

// data structure of limit
typedef struct LimitItem{
    NodeTag tag;
    Condition type;
    int32 begin;
    int32 end;
}LimitItem, *pLimitItem;

// data structure of attribute list
typedef struct AttrList{
    NodeTag tag;
    NodeTag type;
    List list;             // the list
    uint32 size;          // size of list
}AttrList, *pAttrList;

// data strucrture of order by
typedef struct OrderItem{
    NodeTag tag;
    Condition type;
    pAttrItem attr;
}OrderItem, *pOrderItem;

// data structure of drop table
typedef struct DropTable{
    NodeTag tag;
    pExists exists;
}DropTable, *pDropTable;

// data structure of drop index
typedef struct DropIndex{
    NodeTag tag;
    char* table;            // the table name
    char* name;             // the colume name
}DropIndex, *pDropIndex;

// data structure of table definition
typedef struct CreateTable{
    NodeTag tag;
    pExists exists;
    pAttrList attrs;                        // the attribute list 
}CreateTable, *pCreateTable;

// data structure of attribute type
typedef struct AttrType{
    NodeTag tag;
    DataType type;
    int32 size;
}AttrType, *pAttrType;

// data child constraint of value 
typedef struct ChildConstraintData{
    NodeTag tag;
    Condition type;
    char* data;
}ChildConstraintData, *pChildConstraintData;

// data constraint of child Constraint
typedef struct ChildConstraint{
    NodeTag tag;
    Condition type;
    pChildConstraintData data;
}ChildConstraint, *pChildConstraint;

// data constraint of Constraint
typedef struct Constraint{
    NodeTag tag;
    pAttrList constraints;
}Constraint, *pConstraint;

// data structure of attribute list
typedef struct AttrDefinition{
    NodeTag tag;
    char* name;                         // the attribute name
    pAttrType type;
    pConstraint constraint;              // the contraint of attribute
}AttrDefinition, *pAttrDefinition;

// data structure of index definition
typedef struct CreateIndex{
    NodeTag tag;
    Condition unique;
    char* name;
    Condition type;
    char* table;
    pAttrList attrs;
}CreateIndex, *pCreateIndex; 

// data structure of insert
typedef struct InsertTable{
    NodeTag tag;
    char* name;
    pAttrList attrs;                // attribute list
    pAttrList values;               // value list -> use the same list structure with attribute
}InsertTable, *pInsertTable;

// data structure of base expr
typedef struct BaseExpr{
    NodeTag tag;
    ExprType type;
    void* data;
}BaseExpr, *pBaseExpr;

// data structure of Simple expr
typedef struct SimpleExpr{
    NodeTag tag;
    OpType op_type;
    uint32 op_num;
    void* left;
    void* middle;
    void* right;
}SimpleExpr, *pSimpleExpr;

// data structure of where clause
typedef struct WhereDefinition{
    NodeTag tag;
    bool flag;
    pSimpleExpr expr;
}WhereDefinition, *pWhereDefinition;

// data structure of set list
typedef struct SetDefinition{
    NodeTag tag;
    pAttrItem attr;
    pSimpleExpr expr;
}SetDefinition, *pSetDefinition;

// data structure of select
typedef struct SelectTable{
    NodeTag tag;
    Condition distinct;
    pAttrList targets;
    pAttrList froms;
    pAttrList tables;
    pOrderItem order;
    pLimitItem limit;
    pWhereDefinition where;
}SelectTable, *pSelectTable;

// data structure of delete
typedef struct DeleteTable{
    NodeTag tag;
    char* name;
    pWhereDefinition where;
}DeleteTable, *pDeleteTable;

// data structure of update
typedef struct UpdateTable{
    NodeTag tag;
    char* name;
    pAttrList list;
    pWhereDefinition where;
}UpdateTable, *pUpdateTable;

// data structure of sql
typedef struct SqlManager{
    NodeTag tag;
    void* sql;
}SqlManager;
SqlManager sqlManager;


void *makeNode(NodeTag tag);


void vPrintExists(pExists exists);
void vPrintAttrItem(pAttrItem attrItem);
void vPrintValueItem(pValueItem valueItem);
void vPrintLimitItem(pLimitItem limitItem);
void vPrintGrammerList(List *L, NodeTag type);
void vPrintAttrList(pAttrList attrList);
void vPrintOrderItem(pOrderItem orderItem);
void vPrintAttrType(pAttrType attrType);
void vPrintChildConstraintData(pChildConstraintData childConstraintData);
void vPrintChildConstraint(pChildConstraint childConstraint);
void vPrintConstraint(pConstraint constraint);
void vPrintAttrDefinition(pAttrDefinition attrDefinition);
void vPrintBaseExpr(pBaseExpr baseExpr);
void vPrintSimpleExpr(pSimpleExpr simpleExpr);
void vPrintWhereDefinition(pWhereDefinition whereDefinition);
void vPrintSetDefinition(pSetDefinition setDefinition);

void vPrintUpdateTable(pUpdateTable);
void vPrintDeleteTable(pDeleteTable);
void vPrintSelectTable(pSelectTable);
void vPrintInsertTable(pInsertTable);
void vPrintCreateIndex(pCreateIndex);
void vPrintCreateTable(pCreateTable);
void vPrintDropIndex(pDropIndex);
void vPrintDropTable(pDropTable);
void vPrintSql(SqlManager);


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE{
    char*			   t_char;	
    int                            t_Condition;
    struct Exists*                 t_pExists;
    struct AttrItem*               t_pAttrItem;
    struct ValueItem*              t_pValueItem;
    struct LimitItem*              t_pLimitItem;
    struct AttrList*               t_pAttrList;
    struct OrderItem*              t_pOrderItem;
    struct DropTable*              t_pDropTable;
    struct DropIndex*              t_pDropIndex;
    struct CreateTable*            t_pCreateTable;
    struct AttrType*               t_pAttrType;
    struct ChildConstraintData*    t_pChildConstraintData;
    struct ChildConstraint*        t_pChildConstraint;
    struct Constraint*             t_pConstraint;
    struct AttrDefinition*         t_pAttrDefinition;
    struct CreateIndex*            t_pCreateIndex;
    struct InsertTable*            t_pInsertTable;
    struct BaseExpr*               t_pBaseExpr;
    struct SimpleExpr*             t_pSimpleExpr;
    struct WhereDefinition*        t_pWhereDefinition;
    struct SetDefinition*          t_pSetDefinition;
    struct SelectTable*            t_pSelectTable;
    struct DeleteTable*            t_pDeleteTable;
    struct UpdateTable*            t_pUpdateTable;
};

# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
void m_parse();
#define yyterminate() return 0
#endif

#endif

