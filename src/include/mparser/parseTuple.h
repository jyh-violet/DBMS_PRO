/**
* name		: parseTuple.h
* date		: 17/10/22
* author	: bsl
* descriptiion	: 解析数据存储文件,一次解析一行数据
**/

#ifndef PARSEDATA_H_
#define PARSEDATA_H_

#include "mparser/parseTable.h"
#include "utils/LinkList.h"
#include "utils/node.h"

typedef struct HeapTupleItem
{
    DataType type;
    union itemdata{
    	pInt_type 		int_type;
    	pChar_type 		char_type;
    	pVarchar_type 	varchar_type;
    	pLongint_type 	long_type;
    	pFloat_type		float_type;
    	pDouble_type	double_type;
    	pDate_type		date_type;
    }itemdata;
}HeapTupleItem;

HeapTupleItem* vInitHeapTupleItem(DataType type, void* data, uint32 len);
void initHeapTupleItembyAttrData(HeapTupleItem* tuple, AttributeDataItem item);
void setHeapTupleItemByStr(HeapTupleItem* tuple, DataType type, void* data, uint32 len);
void setHeapTupleItemByData(HeapTupleItem* tuple, DataType type, void* data, uint32 len);
void vParseHeapTupleList(char* buffer, AttributeData* table, List* list);
void vParseHeapTuple(char* buffer, AttributeData* table, HeapTupleItem* tuple);
void vParseHeapTupleItem(char* name, AttributeData* table);
void vDestroyHeapTupleItem(HeapTupleItem* tuple);
void vPrintHeapTupleItem(HeapTupleItem* tuple);
HeapTupleItem calculateHeapTupleItem(HeapTupleItem* opr1, HeapTupleItem* opr2, OpType op);
void getHeapTupleItemData(HeapTupleItem* opr1, DataType targetype, void* res);
int cmpHeapTupleItem(HeapTupleItem* opr1, HeapTupleItem* opr2);
void copyHeapTupleItem(HeapTupleItem* src, HeapTupleItem* dest);

//expr.h
uint getOperatorNum(OpType op);
void copyHeapTupleItem(HeapTupleItem* des, HeapTupleItem* src);
#endif

