/**
* name		: parseTable.c
* date		: 17/10/22
* author	: bsl
* description	: 解析table结构
**/


#ifndef PARSETABLE_H_
#define PARSETABLE_H_

#include "basic.h"
#include "utils/mlog.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "storage/dataType.h"
#define MAX_TABLE_ITEM 128
#define INVALID_ATTR_INDEX (MAX_TABLE_ITEM + 1)
#define IS_VALLID_ATTR_INDEX(index)  ((index) < MAX_TABLE_ITEM)
#define MAX_ATTR_NAME_LEN 100
#define MAX_TABLE_NAME_LEN 100
#define MAX_ATTR_NUM 100

#define FILE_LINE_LEN 1024

typedef enum
{
	INTEGER, CHAR, VARCHAR, FLOAT, LONG, DATE, DOUBLE, NONE
}DataType;

#define  Attribute_PRIMARYKEY_FLAG 0x01
#define  Attribute_set_PRIMARYKEY(item)  {(item).flag |= Attribute_PRIMARYKEY_FLAG;}
#define  Attribute_clear_PRIMARYKEY(item)  {(item).flag &= ~Attribute_PRIMARYKEY_FLAG;}
#define  Attribute_check_PRIMARYKEY(item)  ((item).flag &Attribute_PRIMARYKEY_FLAG)

#define Attribute_NOTNULL_FLAG 0x02
#define Attribute_set_NOTNULL(item)  {(item).flag |= Attribute_NOTNULL_FLAG;}
#define Attribute_clear_NOTNULL(item)  {(item).flag &= ~Attribute_NOTNULL_FLAG;}
#define  Attribute_check_NOTNULL(item)  ((item).flag &Attribute_NOTNULL_FLAG)

#define Attribute_UNIQUE_FLAG 0x04
#define Attribute_set_UNIQUE(item)  {(item).flag |= Attribute_UNIQUE_FLAG;}
#define Attribute_clear_UNIQUE(item)  {(item).flag &= ~Attribute_UNIQUE_FLAG;}
#define  Attribute_check_UNIQUE(item)  ((item).flag & Attribute_UNIQUE_FLAG)

#define Attribute_AUTO_FLAG 0x08
#define Attribute_set_AUTO(item)  {(item).flag |= Attribute_AUTO_FLAG;}
#define Attribute_clear_AUTO(item)  {(item).flag &= ~Attribute_AUTO_FLAG;}
#define  Attribute_check_AUTO(item)  ((item).flag & Attribute_AUTO_FLAG)

#define Attribute_DEFAULT_FLAG 0x10
#define Attribute_set_DEFAULT(item)  {(item).flag |= Attribute_DEFAULT_FLAG;}
#define Attribute_clear_DEFAULT(item)  {(item).flag &= ~Attribute_DEFAULT_FLAG;}
#define  Attribute_check_DEFAULT(item)  ((item).flag & Attribute_DEFAULT_FLAG)

#define Attribute_ClearFlags(item)  {(item).flag &= 0;}

typedef struct AttributeDataItem{
    char key[MAX_ATTR_NAME_LEN];
//    char *value;
    DataType type;
    uint32 len;
    uint8 flag;
    char* default_value; // the default value of the this attribute
    					// NOTE:for number type, this is a string
    					// that is for int this is “12” instead of 12
    					// in default, default value is NULL
}AttributeDataItem;

typedef struct AttributeData{
    char name[MAX_TABLE_NAME_LEN];
	uint32 item_num;
    AttributeDataItem item[MAX_TABLE_ITEM];
}AttributeData, *Attribute;

void vDelSpace(char str[]);
bool bIsTable(char buffer[]);
void vParseTable(char *name, AttributeData* table);
void vTablePrint(AttributeData* table);
uint32 uGetMaxTupleLen(AttributeData* table);

#endif

