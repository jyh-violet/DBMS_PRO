%{
 /*
 * grammer.y
 *
 *  Created on: NOV 15, 2017
 *      Author: bsl
 *        desc: sematics analysis
 */
#include "grammer.h"

extern char *yytext;
extern FILE *yyin;
%}

%union{
    char*                          t_char;
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
}


%type <t_pDropTable> DropTableStmt

%type <t_pDropIndex> DropIndexStmt

%type <t_pCreateTable> CreateTableStmt
%type <t_pExists> exists_clause
%type <t_pAttrList> attr_clause attr_list colume_clause colume_list value_clause value_list target_clause from_clause table_list set_list
%type <t_pAttrType> attr_type_clause
%type <t_char> number value real table
%type <t_pConstraint> constraint_clause constraint_clauses
%type <t_pChildConstraintData> key_type_clause null_clause unique_clause auto_clause default_clause
%type <t_pChildConstraint> child_constraint

%type <t_pCreateIndex> CreateIndexStmt 
%type <t_Condition> s_unique_clause index_type_clause distinct_clause
%type <t_pAttrItem> colume

%type <t_pInsertTable> InsertTableStmt

%type <t_pSelectTable> SelectTableStmt
%type <t_pOrderItem> order_clause
%type <t_pLimitItem> limit_clause
%type <t_pWhereDefinition> where_clause
%type <t_pSimpleExpr> a_expr c_expr set_expr
%type <t_pBaseExpr> b_expr s_expr

%type <t_pDeleteTable> DeleteTableStmt

%type <t_pUpdateTable> UpdateTableStmt
%type <t_pSetDefinition> set_item


%token <t_char> NUMBER IDENTIFIER SVALUE DVALUE
%token <t_char> SELECT INSERT DELETE DROP CREATE UPDATE
%token <t_char> TABLE FROM WHERE DISTINCT IF EXISTS INTO ON VALUES SET
%token <t_char> INDEX USING BTREE HASH
%token <t_char> G_VARCHAR G_INTEGER G_CHAR G_LONG G_FLOAT G_DOUBLE G_DATE
%token <t_char> PRIMARY FOREIGN KEY NOT NULLS UNIQUE AUTOINCREMENT DEFAULT COMMIT
%token <t_char> BETWEEN AND OR LIKE IS TRUE FALSE ORDER BY LIMIT ASC DESC
%token <t_char> ADD_EQUAL SUB_EQUAL MUL_EQUAL DIV_EQUAL MOD_EQUAL NOT_EQUAL LESS_EQUAL GREATER_EQUAL
%token <t_char> ADD SUB MUL DIV MOD LESS EQUAL GREATER
%token <t_char> DOT SEM LEFT_BRA RIGHT_BRA COM

%left		OR
%left		AND
%right		NOT
%left		EQUAL LESS_EQUAL GREATER_EQUAL NOT_EQUAL LESS GREATER
%left		ADD  SUB MUL DIV MOD
%%

General: DropTableStmt
         {
            vmlog(GRAMMER_LOG, "DropTableStmt Successfully\n");
            sqlManager.tag = NODE_DROP_TABLE;
            sqlManager.sql = (void*)$1;
            vPrintSql(sqlManager);
            yyterminate();
    //        exit(0);
         }      
       | DropIndexStmt
         {
            vmlog(GRAMMER_LOG, "DropIndexStmt Successfully\n");
            sqlManager.tag = NODE_DROP_INDEX;
            sqlManager.sql = $1;
            vPrintSql(sqlManager);
            yyterminate();
    //        exit(0);
         }
       | CreateTableStmt
         {
            vmlog(GRAMMER_LOG, "CreateTableStmt Successfully\n");
            sqlManager.tag = NODE_CREATE_TABLE;
            sqlManager.sql = $1;
            vPrintSql(sqlManager);
            yyterminate();
    //        exit(0);
         }
       | CreateIndexStmt
         {
            vmlog(GRAMMER_LOG, "CreateIndexStmt Successfully\n");
            sqlManager.tag = NODE_CREATE_INDEX;
            sqlManager.sql = $1;
            vPrintSql(sqlManager);
            yyterminate();
    //        exit(0);
         }
       | InsertTableStmt
         {
            vmlog(GRAMMER_LOG, "InsertTableStmt Successfully\n");
            sqlManager.tag = NODE_INSERT_TABLE;
            sqlManager.sql = $1;
            vPrintSql(sqlManager);
            yyterminate();
    //        exit(0);
         }
       | SelectTableStmt
         {
            vmlog(GRAMMER_LOG, "SelectTableStmt Successfully\n");
            sqlManager.tag = NODE_SELECT_TABLE;
            sqlManager.sql = $1;
            vPrintSql(sqlManager);
            yyterminate();
    //        exit(0);
         }
       | DeleteTableStmt
         {
            vmlog(GRAMMER_LOG, "DeleteTableStmt Successfully\n"); 
            sqlManager.tag = NODE_DELETE_TABLE;
            sqlManager.sql = $1;
            vPrintSql(sqlManager);
            yyterminate();
    //        exit(0);
         }
       | UpdateTableStmt
         { 
            vmlog(GRAMMER_LOG, "UpdateTableStmt Successfully\n");
            sqlManager.tag = NODE_UPDATE_TABLE;
            sqlManager.sql = $1;
            vPrintSql(sqlManager);
            yyterminate();
    //      exit(0);
         }
       ;
    
DropTableStmt:  DROP TABLE IDENTIFIER SEM
                {
                    pDropTable drop     = (pDropTable) makeNode(NODE_DROP_TABLE);
                    drop->exists        = (pExists) makeNode(NODE_EXISTS);
                    drop->exists->flag  = false; 
                    drop->exists->name  = vmalloc(strlen($3) + 1);
                    strcpy(drop->exists->name, $3);
                    $$ = drop;
                }
             |  DROP TABLE IF EXISTS IDENTIFIER SEM
                {
                    pDropTable drop     = (pDropTable) makeNode(NODE_DROP_TABLE);
                    drop->exists        = (pExists) makeNode(NODE_EXISTS);
                    drop->exists->flag  = true;
                    drop->exists->name  = vmalloc(strlen($5) + 1);
                    strcpy(drop->exists->name, $5);
                    $$ = drop;
                }
             ;

DropIndexStmt: DROP INDEX IDENTIFIER ON IDENTIFIER SEM
               {
                    pDropIndex drop     = (pDropIndex) makeNode(NODE_DROP_INDEX);
                    drop->table         = vmalloc(strlen($5) + 1);
                    drop->name          = vmalloc(strlen($3) + 1);
                    strcpy(drop->table, $5);
                    strcpy(drop->name, $3);
                    $$ = drop;
               } 
             | DROP INDEX IDENTIFIER DOT IDENTIFIER SEM
               {
                    pDropIndex drop     = (pDropIndex) makeNode(NODE_DROP_INDEX);
                    drop->table         = vmalloc(strlen($3) + 1);
                    drop->name          = vmalloc(strlen($5) + 1);
                    strcpy(drop->table, $3);
                    strcpy(drop->name, $5);
                    $$ = drop;
               }
             ;

CreateTableStmt: CREATE TABLE exists_clause attr_clause SEM
                 {
                    pCreateTable create     = (pCreateTable) makeNode(NODE_CREATE_TABLE);
                    create->exists          = $3;
                    create->attrs           = $4;
                    $$ = create;
                 }
               ;
exists_clause: IDENTIFIER
               {
                    pExists exists          = (pExists) makeNode(NODE_EXISTS);
                    exists->flag            = false;
                    exists->name            = vmalloc(strlen($1) + 1);
                    strcpy(exists->name, $1);
                    $$ = exists;
               }
             | IF NOT EXISTS IDENTIFIER
               {
                    pExists exists          = (pExists) makeNode(NODE_EXISTS);
                    exists->flag            = true;
                    exists->name            = vmalloc(strlen($4) + 1);
                    strcpy(exists->name, $4);
                    $$ = exists;
               }
             ;
attr_clause: LEFT_BRA attr_list RIGHT_BRA
             {
                $$ = $2;
             }
           ;
attr_list: IDENTIFIER attr_type_clause constraint_clause
           {
                pAttrList list              = (pAttrList) makeNode(NODE_ATTR_LIST);
                list->type                  = NODE_ATTR_DEFINITION;
                vListInit(&(list->list));
                pAttrDefinition def         = (pAttrDefinition) makeNode(NODE_ATTR_DEFINITION);
                def->name                   = vmalloc(strlen($1) + 1);
                strcpy(def->name, $1);
                def->type                   = $2;
                def->constraint             = $3;
                vListInsert(&(list->list), def);
                list->size                  = 1;
                $$ = list;
           }
         | attr_list COM IDENTIFIER attr_type_clause constraint_clause
           {
                pAttrList list              = (pAttrList) $1;
                pAttrDefinition def         = (pAttrDefinition) makeNode(NODE_ATTR_DEFINITION);
                def->name                   = vmalloc(strlen($3) + 1);
                strcpy(def->name, $3);
                def->type                   = $4;
                def->constraint             = $5;
                vListInsert(&(list->list), def);
                list->size ++;
                $$ = list;
           }
         ;
attr_type_clause: G_VARCHAR LEFT_BRA number RIGHT_BRA
                  {
                      pAttrType attr_type   = (pAttrType) makeNode(NODE_ATTR_TYPE);
                      attr_type->type       = VARCHAR;
                      attr_type->size       = char2Int($3);
                      $$ = attr_type;
                  }
                | G_INTEGER
                  {
                      pAttrType attr_type   = (pAttrType) makeNode(NODE_ATTR_TYPE);
                      attr_type->type       = INTEGER;
                      attr_type->size       = ZERO;
                      $$ = attr_type;
                  }
                | G_CHAR LEFT_BRA number RIGHT_BRA
                  {
                      pAttrType attr_type   = (pAttrType) makeNode(NODE_ATTR_TYPE);
                      attr_type->type       = CHAR;
                      attr_type->size       = char2Int($3);
                      $$ = attr_type;
                  }
                | G_LONG
                  {
                      pAttrType attr_type   = (pAttrType) makeNode(NODE_ATTR_TYPE);
                      attr_type->type       = LONG;
                      attr_type->size       = ZERO;
                      $$ = attr_type;
                  }
                | G_FLOAT
                  {
                      pAttrType attr_type   = (pAttrType) makeNode(NODE_ATTR_TYPE);
                      attr_type->type       = FLOAT;
                      attr_type->size       = ZERO;
                      $$ = attr_type;
                  }
                | G_DOUBLE
                  {
                      pAttrType attr_type   = (pAttrType) makeNode(NODE_ATTR_TYPE);
                      attr_type->type       = DOUBLE;
                      attr_type->size       = ZERO;
                      $$ = attr_type;
                  }
                | G_DATE
                  {
                      pAttrType attr_type   = (pAttrType) makeNode(NODE_ATTR_TYPE);
                      attr_type->type       = DATE;
                      attr_type->size       = ZERO;
                      $$ = attr_type;
                  }
                ;
number: NUMBER
        {
            $$ = $1;
        }
      | ADD NUMBER
        {
            $$ = $2;
        }
      | SUB NUMBER
        {
            char *p = vmalloc(strlen($2) + 1 + 1);
            strcpy(p, "-");
            strcat(p, $2);
            $$ = p;
        }
      ;
     
constraint_clause: 
                   {
                       $$ = NULL;
                   }
                 | constraint_clauses
                   {
                       $$ = $1;
                   }
                 ;
constraint_clauses: child_constraint
                    {
                        pConstraint p    = (pConstraint) makeNode(NODE_CONSTRAINT);
                        p->constraints   = (pAttrList) makeNode(NODE_ATTR_LIST);
                        p->constraints->type = NODE_CONSTRAINT;
                        vListInit(&(p->constraints->list));
                        vListInsert(&(p->constraints->list), $1);
                        p->constraints->size = 1;
                        $$ = p;
                    }
                  | constraint_clauses child_constraint
                    {
                        pConstraint p    = (pConstraint) $1;
                        vListInsert(&(p->constraints->list), $2);
                        $$ = p;
                    }
                  ;
child_constraint: key_type_clause
                  {
                    pChildConstraint child  = (pChildConstraint) makeNode(NODE_CHILD_CONSTRAINT);
                    child->type             = CONSTRAINT_KEY;
                    child->data             = $1;
                    $$ = child;
                  }
                | null_clause
                  {
                    pChildConstraint child  = (pChildConstraint) makeNode(NODE_CHILD_CONSTRAINT);
                    child->type             = CONSTRAINT_NULL;
                    child->data             = $1;
                    $$ = child;
                  }
                | default_clause
                  {
                    pChildConstraint child  = (pChildConstraint) makeNode(NODE_CHILD_CONSTRAINT);
                    child->type             = CONSTRAINT_DEFAULT;
                    child->data             = $1;
                    $$ = child;
                  }
                | unique_clause
                  {
                    pChildConstraint child  = (pChildConstraint) makeNode(NODE_CHILD_CONSTRAINT);
                    child->type             = CONSTRAINT_UNIQUE;
                    child->data             = $1;
                    $$ = child;
                  }
                | auto_clause
                  {
                    pChildConstraint child  = (pChildConstraint) makeNode(NODE_CHILD_CONSTRAINT);
                    child->type             = CONSTRAINT_AUTOINCREMENT;
                    child->data             = $1;
                    $$ = child;
                  }
                ;
key_type_clause: PRIMARY KEY
                 {
                    pChildConstraintData p  = (pChildConstraintData) makeNode(NODE_CHILD_CONSTRAINT_DATA);
                    p->type = KEY_PRIMARY;
                    p->data = NULL;
                    $$ = p; 
                 }
               | FOREIGN KEY
                 { 
                    pChildConstraintData p  = (pChildConstraintData) makeNode(NODE_CHILD_CONSTRAINT_DATA);
                    p->type = KEY_FOREIGN;
                    p->data = NULL;
                    $$ = p; 
                 }
               ;
null_clause: NOT NULLS
             {
                pChildConstraintData p  = (pChildConstraintData) makeNode(NODE_CHILD_CONSTRAINT_DATA);
                p->type = NULL_FALSE;
                p->data = NULL;
                $$ = p; 
             }
           | NULLS
             {
                pChildConstraintData p  = (pChildConstraintData) makeNode(NODE_CHILD_CONSTRAINT_DATA);
                p->type = NULL_TRUE;
                p->data = NULL;
                $$ = p; 
             }
           ;
unique_clause: UNIQUE
               { 
                   pChildConstraintData p  = (pChildConstraintData) makeNode(NODE_CHILD_CONSTRAINT_DATA);
                   p->type = UNIQUE_TRUE;
                   p->data = NULL;
                   $$ = p; 
               }
             ;
auto_clause: AUTOINCREMENT
             {
                 pChildConstraintData p  = (pChildConstraintData) makeNode(NODE_CHILD_CONSTRAINT_DATA);
                 p->type = AUTO_TRUE;
                 p->data = NULL;
                 $$ = p; 
             }
           ;
default_clause: DEFAULT NULLS
               {
                    pChildConstraintData p  = (pChildConstraintData) makeNode(NODE_CHILD_CONSTRAINT_DATA);
                    p->type = DEFAULT_NULL;
                    p->data = NULL;
                    $$ = p; 
                }
              | DEFAULT value
                {
                    pChildConstraintData p  = (pChildConstraintData) makeNode(NODE_CHILD_CONSTRAINT_DATA);
                    p->type = DEFAULT_TRUE;
                    p->data = vmalloc(strlen($2) + 1);
                    strcpy(p->data, $2);
                    $$ = p;
                }
              ;
value: SVALUE
       {
           $$ = $1;
       }
     | DVALUE
       {
           $$ = $1;
       }
     | number
       {
           $$ = $1;
       }
     | real
       {
           $$ = $1;
       }
     ;
real: number DOT NUMBER
      {
         char *p    = vmalloc(strlen($1) + strlen($3) + 1 + 1);
         strcpy(p, $1);
         strcat(p, ".");
         strcat(p, $3);
         $$ = p;
      }
    ;

CreateIndexStmt: CREATE s_unique_clause INDEX IDENTIFIER index_type_clause ON IDENTIFIER colume_clause SEM
                 {
                     pCreateIndex p = (pCreateIndex) makeNode(NODE_CREATE_INDEX);
                     p->unique      = $2;
                     p->name        = vmalloc(strlen($4) + 1);
                     strcpy(p->name, $4);
                     p->type        = $5;
                     p->table       = vmalloc(strlen($7) + 1);
                     strcpy(p->table, $7);
                     p->attrs       = $8;
                     $$ = p;
                 }
               ;
s_unique_clause:
                 {
                    $$ = UNIQUE_FALSE;
                 }
               | UNIQUE
                 {
                    $$ = UNIQUE_TRUE;
                 }
               ;
index_type_clause:
                   {
                       $$ = INDEX_NONE;
                   }
                 | USING BTREE
                   {
                       $$ = INDEX_BTREE;
                   }
                 | USING HASH
                   {
                       $$ = INDEX_HASH;
                   }
                 ;

colume_clause:
               {
                   $$ = NULL;
               }
             | LEFT_BRA colume_list RIGHT_BRA
               {
                   $$ = $2;
               }
             ;
colume_list: colume
             {
                pAttrList list  = (pAttrList) makeNode(NODE_ATTR_LIST);
                list->type      = NODE_ATTR_ITEM;
                vListInit(&(list->list));
                vListInsert(&(list->list), $1);
                list->size = 1;
                $$ = list;
             }
           | colume_list COM colume
             {
                pAttrList list = (pAttrList) $1;
                vListInsert(&(list->list), $3);
                list->size++ ;
                $$ = list;
             }
           ;
colume: IDENTIFIER
        {
            pAttrItem p = (pAttrItem) makeNode(NODE_ATTR_ITEM);
            p->name     = vmalloc(strlen($1) + 1);
            strcpy(p->name, $1);
            $$ = p;
        }
      | IDENTIFIER DOT IDENTIFIER
        {
            pAttrItem p = (pAttrItem) makeNode(NODE_ATTR_ITEM);
            p->table    = vmalloc(strlen($1) + 1);
            strcpy(p->table, $1);
            p->name     = vmalloc(strlen($3) + 1);
            strcpy(p->name, $3);
            $$ = p;
        }
      ;

InsertTableStmt: INSERT INTO IDENTIFIER colume_clause VALUES value_clause SEM
                 {
                    pInsertTable p  = (pInsertTable) makeNode(NODE_INSERT_TABLE);
                    p->name         = vmalloc(strlen($3) + 1);
                    strcpy(p->name, $3);
                    p->attrs        = $4;
                    p->values       = $6;
                    $$ = p;
                 }
               ;
value_clause: LEFT_BRA value_list RIGHT_BRA
              {
                  $$ = $2;
              }
            ;
value_list: value
            {
                pAttrList list  = (pAttrList) makeNode(NODE_ATTR_LIST);
                list->type      = NODE_CHAR;
                vListInit(&(list->list));
                vListInsert(&(list->list), $1);
                list->size = 1;
                $$ = list;
            }
          | value_list COM value
            {
                pAttrList list = (pAttrList) $1;
                vListInsert(&(list->list), $3);
                list->size++;
                $$ = list;
            }
          ;

SelectTableStmt:  SELECT distinct_clause target_clause from_clause where_clause order_clause limit_clause SEM
                  {
                      pSelectTable p = (pSelectTable) makeNode(NODE_SELECT_TABLE);
                      p->distinct   = $2;
                      p->targets    = $3;
                      p->froms      = $4;
                      p->where      = $5;
                      p->order      = $6;
                      p->limit      = $7;
                      $$ = p;
                  }
               ;
distinct_clause:
                 {
                    $$ = DISTINCT_FALSE;
                 }
               | DISTINCT
                 {
                    $$ = DISTINCT_TRUE;
                 }
               ;
target_clause:
               {
                   $$ = NULL;
               }
             | MUL
               {
                   $$ = NULL;
               }
             | colume_list
               {
                   $$ = $1;
               }
             ;
from_clause: FROM table_list
             {
                $$ = $2;
             }
           ;
table_list: table
             {
                pAttrList list  = (pAttrList) makeNode(NODE_ATTR_LIST);
                list->type      = NODE_CHAR;
                vListInit(&(list->list));
                vListInsert(&(list->list), $1);
                list->size = 1;
                $$ = list;
             }
           | table_list COM table
             {
                pAttrList list = (pAttrList) $1;
                vListInsert(&(list->list), $3);
                list->size++ ;
                $$ = list;
             }
           ;
table: IDENTIFIER
        	{
            	char*p     = vmalloc(strlen($1) + 1);
            	strcpy(p, $1);
            	$$ = p;
        	}
      		;
order_clause:
              {
                  $$ = NULL;
              }
            | ORDER BY colume ASC
              {
                  pOrderItem order  = (pOrderItem) makeNode(NODE_ORDER_ITEM);
                  order->type       = ORDER_ASC;
                  order->attr       = $3;
                  $$ = order;
              }
            | ORDER BY colume DESC
              {
                  pOrderItem order  = (pOrderItem) makeNode(NODE_ORDER_ITEM);
                  order->type       = ORDER_DESC;
                  order->attr       = $3;
                  $$ = order;
              }
            ; 
limit_clause:
              {
                  $$ = NULL;
              }
            | LIMIT number COM number
              {
                  pLimitItem limit  = (pLimitItem) makeNode(NODE_LIMIT_ITEM);
                  limit->type       = LIMIT_DOUBLE;
                  limit->begin      = char2Int($2);
                  limit->end        = char2Int($4);
                  $$ = limit;
              }
            | LIMIT number
              {
                  pLimitItem limit  = (pLimitItem) makeNode(NODE_LIMIT_ITEM);
                  limit->type       = LIMIT_SINGLE;
                  limit->begin      = char2Int($2);
                  $$ = limit;
              }
            ;
where_clause:
              {
                  $$ = NULL;
              }
            | WHERE a_expr
              {
                  pWhereDefinition where    = (pWhereDefinition)makeNode(NODE_WHERE_DEFINITION);
                  where->expr               = $2;
                  where->flag				= true;
                  $$ = where;
              }
            ;
a_expr: b_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_NONE;
            p->op_num       = 1;
            p->left         = $1;
            $$ = p;
        }
      | a_expr ADD a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_ADD;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr SUB a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_SUB;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr MUL a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_MUL;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr DIV a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_DIV;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr MOD a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_MOD;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr ADD_EQUAL a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_SELF_ADD;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr SUB_EQUAL a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_SELF_SUB;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr MUL_EQUAL a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_SELF_MUL;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr DIV_EQUAL a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_SELF_DIV;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr MOD_EQUAL a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_SELF_MOD;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr LESS a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_LESS;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr GREATER a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_GREATER;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr EQUAL a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_EQUAL;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr LESS_EQUAL a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_LESS_EQUAL;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr GREATER_EQUAL a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_GREATER_EQUAL;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr NOT_EQUAL a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_NOT_EQUAL;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr AND a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_AND;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr OR a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_OR;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr BETWEEN c_expr AND a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_BETWEEN;
            p->op_num       = 3;
            p->left         = $1;
            p->middle       = $3;
            p->right        = $5;
            $$ = p;
        }
      | a_expr NOT BETWEEN c_expr AND a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_BETWEEN;
            p->op_num       = 3;
            p->left         = $1;
            p->middle       = $4;
            p->right        = $6;
            $$ = p;
        }
      | a_expr LIKE a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_LIKE;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | a_expr NOT LIKE a_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_NOT_LIKE;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $4;
            $$ = p;
        }
      | a_expr IS NULLS
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_NULL;
            p->op_num       = 1;
            p->left         = $1;
            $$ = p;
        }
      | a_expr IS NOT NULLS
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_NOT_NULL;
            p->op_num       = 1;
            p->left         = $1;
            $$ = p;
        }
      | a_expr IS TRUE
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_TRUE;
            p->op_num       = 1;
            p->left         = $1;
            $$ = p;
        }
      | a_expr IS NOT TRUE
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_NOT_TRUE;
            p->op_num       = 1;
            p->left         = $1;
            $$ = p;
        }
      | a_expr IS FALSE
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_FALSE;
            p->op_num       = 1;
            p->left         = $1;
            $$ = p;
        }
      | a_expr IS NOT FALSE
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_NOT_FALSE;
            p->op_num       = 1;
            p->left         = $1;
            $$ = p;
        }
      ;

b_expr: number
        {
            pBaseExpr expr  = (pBaseExpr) makeNode(NODE_BASE_EXPR);
            expr->type      = EXPR_NUMBER;
            expr->data      = vmalloc(strlen($1) + 1);
            strcpy(expr->data, $1);
            $$ = expr;
        }
      | real
        { 
            pBaseExpr expr  = (pBaseExpr) makeNode(NODE_BASE_EXPR);
            expr->type      = EXPR_REAL;
            expr->data      = vmalloc(strlen($1) + 1);
            strcpy(expr->data, $1);
            $$ = expr;
        }
      | colume
        { 
            pBaseExpr expr  = (pBaseExpr) makeNode(NODE_BASE_EXPR);
            expr->type      = EXPR_COLUME;
            expr->data      = $1;
            $$ = expr;
        }
      | SVALUE
        {
            pBaseExpr expr  = (pBaseExpr) makeNode(NODE_BASE_EXPR);
            expr->type      = EXPR_STRING;
            expr->data      = vmalloc(strlen($1) + 1);
            strcpy(expr->data, $1);
            $$ = expr;
        }
      | DVALUE
        {
            pBaseExpr expr  = (pBaseExpr) makeNode(NODE_BASE_EXPR);
            expr->type      = EXPR_STRING;
            expr->data      = vmalloc(strlen($1) + 1);
            strcpy(expr->data, $1);
            $$ = expr;
         }
      | LEFT_BRA a_expr RIGHT_BRA
        {
            pBaseExpr expr  = (pBaseExpr) makeNode(NODE_BASE_EXPR);
            expr->type      = EXPR_SIMPLE;
            expr->data      = $2;
            $$ = expr;
        }
      ;
      
c_expr: b_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_NONE;
            p->op_num       = 1;
            p->left         = $1;
            $$ = p;
        }
      | c_expr ADD c_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_ADD;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | c_expr SUB c_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_SUB;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | c_expr MUL c_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_MUL;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | c_expr DIV c_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_DIV;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | c_expr MOD c_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_MOD;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | c_expr ADD_EQUAL c_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_SELF_ADD;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | c_expr SUB_EQUAL c_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_SELF_SUB;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | c_expr MUL_EQUAL c_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_SELF_MUL;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | c_expr DIV_EQUAL c_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_SELF_DIV;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | c_expr MOD_EQUAL c_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_SELF_MOD;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | c_expr LESS c_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_LESS;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | c_expr EQUAL c_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_EQUAL;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | c_expr GREATER c_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_GREATER;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | c_expr LESS_EQUAL c_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_LESS_EQUAL;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | c_expr GREATER_EQUAL c_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_GREATER_EQUAL;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      | c_expr NOT_EQUAL c_expr
        {
            pSimpleExpr p   = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            p->op_type      = OP_NOT_EQUAL;
            p->op_num       = 2;
            p->left         = $1;
            p->right        = $3;
            $$ = p;
        }
      ;

DeleteTableStmt: DELETE FROM IDENTIFIER where_clause SEM
                 {
                    pDeleteTable del    = (pDeleteTable) makeNode(NODE_DELETE_TABLE);
                    del->name           = vmalloc(strlen($3) + 1);
                    strcpy(del->name, $3);
                    del->where          = $4;
                    $$ = del;
                 }
               ;
UpdateTableStmt: UPDATE IDENTIFIER SET set_list where_clause SEM
                 {
                    pUpdateTable update = (pUpdateTable) makeNode(NODE_UPDATE_TABLE);
                    update->name        = vmalloc(strlen($2) + 1);
                    strcpy(update->name, $2);
                    update->list        = $4;
                    update->where       = $5;
                    $$ = update;
                 }
                ;
set_list: set_item
          {
            pAttrList list      = (pAttrList) makeNode(NODE_ATTR_LIST);
            list->type          = NODE_SET_DEFINITION;
            vListInit(&(list->list));
            vListInsert(&(list->list), $1);
            list->size  = 1;
            $$ = list;
          }
        | set_list COM set_item
          {
            pAttrList list = (pAttrList) $1;
            vListInsert(&(list->list), $3);
            list->size ++;
            $$ = list;
          }
        ;

set_item: colume EQUAL set_expr
          {
            pSetDefinition set  = (pSetDefinition) makeNode(NODE_SET_DEFINITION);
            set->attr           = $1;
            set->expr           = $3;
            $$ = set;
          }
        ;

set_expr: s_expr
          {
            pSimpleExpr expr    = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            expr->op_type       = OP_NONE;
            expr->left          = $1;
            $$ = expr;
          }
        | set_expr ADD set_expr
          {
            pSimpleExpr expr    = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            expr->op_type       = OP_ADD;
            expr->op_num        = 2;
            expr->left          = $1;
            expr->right         = $3;
            $$ = expr;
          }
        | set_expr SUB set_expr
          {
            pSimpleExpr expr    = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            expr->op_type       = OP_SUB;
            expr->op_num        = 2;
            expr->left          = $1;
            expr->right         = $3;
            $$ = expr;
          }
        | set_expr MUL set_expr
          {
            pSimpleExpr expr    = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            expr->op_type       = OP_MUL;
            expr->op_num        = 2;
            expr->left          = $1;
            expr->right         = $3;
            $$ = expr;
          }
        | set_expr DIV set_expr
          {
            pSimpleExpr expr    = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            expr->op_type       = OP_DIV;
            expr->op_num        = 2;
            expr->left          = $1;
            expr->right         = $3;
            $$ = expr;
          }
        | set_expr MOD set_expr
          {
            pSimpleExpr expr    = (pSimpleExpr) makeNode(NODE_SIMPLE_EXPR);
            expr->op_type       = OP_MOD;
            expr->op_num        = 2;
            expr->left          = $1;
            expr->right         = $3;
            $$ = expr;
          }
        ;

s_expr: number
        {
            pBaseExpr expr = (pBaseExpr) makeNode(NODE_BASE_EXPR);
            expr->type = EXPR_NUMBER;
            expr->data = vmalloc(strlen($1) + 1);
            strcpy(expr->data, $1);
            $$ = expr;
        }
      | real
        {
            pBaseExpr expr = (pBaseExpr) makeNode(NODE_BASE_EXPR);
            expr->type = EXPR_REAL;
            expr->data = vmalloc(strlen($1) + 1);
            strcpy(expr->data, $1);
            $$ = expr; 
        }
      | colume
        {
            pBaseExpr expr = (pBaseExpr) makeNode(NODE_BASE_EXPR);
            expr->type = EXPR_COLUME;
            expr->data = $1;
            $$ = expr;
        }
      | SVALUE
        {
            pBaseExpr expr = (pBaseExpr) makeNode(NODE_BASE_EXPR);
            expr->type = EXPR_STRING;
            expr->data = vmalloc(strlen($1) + 1);
            strcpy(expr->data, $1);
            $$ = expr;
        }
      | DVALUE
        {
            pBaseExpr expr = (pBaseExpr) makeNode(NODE_BASE_EXPR);
            expr->type = EXPR_STRING;
            expr->data = vmalloc(strlen($1) + 1);
            strcpy(expr->data, $1);
            $$ = expr;
        }
      | LEFT_BRA set_expr RIGHT_BRA
        {
            pBaseExpr expr = (pBaseExpr) makeNode(NODE_BASE_EXPR);
            expr->type = EXPR_SIMPLE;
            expr->data = $2;
            $$ = expr;
        }
      ;

%%

void m_parse()
{
   /* yyin = fopen(argv[1], "r");
    if(!yyin){
        vmlog(GRAMMER_LOG, "ERROR: open <%s> failed", argv[1]);
        return 0;
    }*/

    yyparse();
    //vPrintSql(sqlManager);
   // yylex();
   // fclose(yyin);
}

yyerror(char *s)
{
    vmlog(GRAMMER_LOG, "ERROR: %s", s);
}


