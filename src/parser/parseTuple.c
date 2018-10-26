/**
* name		: parseTuple.c
* date		: 17/10/22
* author	: bsl
* description	: 根据表格结构解析元组数据
**/

#include "mparser/parseTuple.h"
DataType judgeDataType(HeapTupleItem* opr1, HeapTupleItem* opr2);
bool IsNumericType(DataType type);
bool IsCharType(DataType type);
bool IsDateType(DataType type);

/**
* vInitHeapTupleItem()		: 初始化一条元组的一个元素
* @param (HeapTupleItem* tuple)	: 元组的元素指针
* @param (DataType type)	: 数据类型
* @param (char* data)		: 数据内容
* @param (uint32 len)		: 长度限制
**/
HeapTupleItem* vInitHeapTupleItem(DataType type, void* data, uint32 len)
{
	HeapTupleItem* tuple = NULL;
    if(type == NONE || data == NULL)
    {
        vmlog(ERROR, "<parseTuple.c>,error: vInitHeapTupleItem() <type=%d> <data=%s>", NONE, data);
    }

    tuple = (HeapTupleItem*)vmalloc0(sizeof(HeapTupleItem));                                 		// 初始化一个元素空间

//    (tuple)->type = (char*)vmalloc(sizeof(char) * (strlen(type) + 1));       			// 初始化一个元素类型空间
//    strcpy((tuple)->type, type);
    tuple->type = type;
    switch(type)
    {
    case CHAR:
    	initTypeCharData(&(tuple->itemdata.char_type), len);
    	break;
    case VARCHAR:
    	initTypeVarcharData(&(tuple->itemdata.varchar_type), len);
    }
  //  vmlog(MALLOC_TRACE, "vInitHeapTupleItem -- malloc (tuple)->data:%p", (tuple)->data);
    return tuple;
}

void initHeapTupleItembyAttrData(HeapTupleItem* tuple, AttributeDataItem item)
{
	tuple->type = item.type;
	switch(item.type)
	{
	case CHAR:
		tuple->itemdata.char_type.length = item.len;
		break;
	case VARCHAR:
		tuple->itemdata.varchar_type.max_length = item.len;
		break;
	}
}

/**
 * setHeapTupleItemByStr : by jyh
 * all types is passed through the str
 * e.g if type = INTEGER then, *data should be "1" instead of 1
 * */
void setHeapTupleItemByStr(HeapTupleItem* tuple, DataType type, void* data, uint32 len)
{
	if(type == NONE || data == NULL)
    {
        vmlog(ERROR, "<parseTuple.c>,error: setHeapTupleItemByStr() <type=%d> <data=%s>", NONE, data);
    }
//    (tuple)->type = (char*)vmalloc(sizeof(char) * (strlen(type) + 1));       			// 初始化一个元素类型空间
//    strcpy((tuple)->type, type);
    tuple->type = type;

    if(type ==  INTEGER)
    {
    	setTypeIntData(&(tuple->itemdata.int_type), char2Int(data));
    }else if(type == CHAR)
    {
    	setTypeCharData(&(tuple->itemdata.char_type), data);
    }else if(type == VARCHAR)
    {
    	setTypeVarcharData(&(tuple->itemdata.varchar_type), data);
    }else if(type == FLOAT)
    {
    	setTypeFloatData(&(tuple->itemdata.float_type), char2Float(data));
    }else if(type == LONG)
    {
    	setTypeLongData(&(tuple->itemdata.long_type), (long)char2Int(data));
    }else if(type == DATE)
    {
    	setTypeDateData(&(tuple->itemdata.date_type), data);
    }else if(type == DOUBLE)
    {
    	setTypeDoubleData(&(tuple->itemdata.double_type), char2Double(data));
    }else
    {
        vmlog(ERROR, "setHeapTupleItemByStr -- type error!");
    }
}

/**
 * setHeapTupleItemByData : by jyh
 * number can be set through the num itself instad of str
 * e.g if type = INTEGER, then *data = 1 instead of "1"
 *
 * */
void setHeapTupleItemByData(HeapTupleItem* tuple, DataType type, void* data, uint32 len)
{
	if(type == NONE || data == NULL)
	{
        vmlog(ERROR, "<parseTuple.c>,error: setHeapTupleItemByData() <type=%d> <data=%s>", NONE, data);
	}
//    (tuple)->type = (char*)vmalloc(sizeof(char) * (strlen(type) + 1));       			// 初始化一个元素类型空间
//    strcpy((tuple)->type, type);
	tuple->type = type;
	if(type ==  INTEGER)
	{
		setTypeIntData(&(tuple->itemdata.int_type), *(int32*)data);
	}else if(type == CHAR)
	{
		setTypeCharData(&(tuple->itemdata.char_type), data);
	}else if(type == VARCHAR)
	{
		setTypeVarcharData(&(tuple->itemdata.varchar_type), data);
	}else if(type == FLOAT)
	{
		setTypeFloatData(&(tuple->itemdata.float_type), *(float*)data);
	}else if(type == LONG)
	{
		setTypeLongData(&(tuple->itemdata.long_type), *(int64*)data);
	}else if(type == DATE)
	{
		setTypeDateData(&(tuple->itemdata.date_type), data);
	}else if(type == DOUBLE)
	{
		setTypeDoubleData(&(tuple->itemdata.double_type), *(double*)data);
	}else
	{
		vmlog(ERROR, "setHeapTupleItemByData -- type error!");
	}
}

void copyHeapTupleItem(HeapTupleItem* des, HeapTupleItem* src)
{
	if(src == NULL  || des == NULL)
	{
		vmlog(ERROR, "copyHeapTupleItem-- NULL pointer");
	}
	des->type = src->type;
	switch(src->type)
	{
	case INTEGER:
	case LONG:
	case DOUBLE:
	case FLOAT:
	case DATE:
		*des = *src;
		break;
	case CHAR:
		setTypeCharData(&(des->itemdata.char_type), src->itemdata.char_type.data);
		break;
	case VARCHAR:
		setTypeVarcharData(&(des->itemdata.varchar_type), src->itemdata.varchar_type.data);
		break;
	}
}

void vDestroyHeapTupleItem(HeapTupleItem* tuple)
{
    if(tuple == NULL )
    {
        return;
    }
    DataType type = tuple->type;

    if(type ==  INTEGER)
    {
    	destroyTypeInt(&(tuple->itemdata.int_type));
    }else if(type == CHAR)
    {
    	destroyTypeChar(&(tuple->itemdata.char_type));
    }else if(type == VARCHAR)
    {
    	destroyTypeVarchar(&(tuple->itemdata.varchar_type));
    }else if(type == FLOAT)
    {
    	destroyTypeFloat(&(tuple->itemdata.float_type));
    }else if(type == LONG)
    {
    	destroyTypeLong(&(tuple->itemdata.long_type));
    }else if(type == DATE)
    {
    	destroyTypeDate(&(tuple->itemdata.date_type));
    }else if(type == DOUBLE)
    {
    	destroyTypeDouble(&(tuple->itemdata.double_type));
    }
}

void vPrintHeapTupleItem(HeapTupleItem* tuple)
{
    if(tuple == NULL)
    {
        return;
    }
    DataType type = tuple->type;

    if(type == INTEGER)
	{
		printf("%d\t|", (tuple->itemdata.int_type.data));
	}else if(type  == CHAR)
	{
		printf("%s\t|", tuple->itemdata.char_type.data);
	}else if(type  == VARCHAR)
	{
		printf("%s\t|", tuple->itemdata.varchar_type.data);
	}else if(type == FLOAT)
	{
		printf("%f\t|", tuple->itemdata.float_type.data);
	}else if(type == LONG)
	{
		printf("%ld\t|", tuple->itemdata.long_type.data);
	}else if(type == DATE)
	{
		printf("%d-%d-%d\t|", tuple->itemdata.date_type.date / YEAR_BASE,
				tuple->itemdata.date_type.date % YEAR_BASE / MONTH_BASE,
				tuple->itemdata.date_type.date % MONTH_BASE / DAY_BASE);
//			uint i = 0;
//			for(i = 0; i < YEAR_SIZE; i ++)
//			{
//				printf("%c", tuple->itemdata.date_type.date[i + YEAR_OFF]);
//			}
//			printf("-");
//			for(i = 0; i < MONTH_SIZE; i ++)
//			{
//				printf("%c", tuple->itemdata.date_type.date[i + MONTH_OFF]);
//			}
//			printf("-");
//			for(i = 0; i < DAY_SIZE; i ++)
//			{
//				printf("%c", tuple->itemdata.date_type.date[i + DAY_OFF]);
//			}
//			printf("\t|");
	}else if(type == DOUBLE)
	{
			printf("%lf\t|", tuple->itemdata.double_type.data);
	}
}

/**
* vParseTupleList()	: 初始化一个元组（一行数据被初始化为一个链表）
* @param (char* buffer)	: 一行数据
* @param (List* list)	: 链表头指针
**/
void vParseHeapTupleList(char* buffer, AttributeData* table, List* list)
{
    uint32 iItem = 0;
    char* pLineItem = strtok(buffer, "|");
    while(pLineItem != NULL)
    {
        HeapTupleItem *tmpData = NULL;

        tmpData = vInitHeapTupleItem(table->item[iItem].type, pLineItem, (uint32)table->item[iItem].len);   // 初始化一个元素

        vmlog(MALLOC_TRACE, "vParseHeapTupleList HeapTupleItem-- malloc:%p", tmpData);
		if(tmpData == NULL)
		{
			printf("tmpData is null!\n");
		}

		vListInsert(list, tmpData);
		pLineItem = strtok(NULL, "|");
		iItem ++;
    }

    return;
}

/**
* vParseTuple: 初始化一个元组（一行数据被初始化为一个链表）
* @param (char* buffer)	: 一行数据
* @param (List* list)	:  the base pointer of the array
**/
void vParseHeapTuple(char* buffer, AttributeData* table, HeapTupleItem* tuple)
{
    uint32 iItem = 0;
    char* pLineItem = strtok(buffer, "|");
    while(pLineItem != NULL)
    {
  //      HeapTupleItem *tmpData = NULL;

//        tmpData = vInitHeapTupleItem(table->item[iItem].type, pLineItem, (uint32)table->item[iItem].len);   // 初始化一个元素
//
//        vmlog(MALLOC_TRACE, "vParseHeapTupleList HeapTupleItem-- malloc:%p", tmpData);
//		if(tmpData == NULL)
//		{
//			printf("tmpData is null!\n");
//		}
//
//		vListInsert(list, tmpData);
    	vmlog(PARSETUPLE_LOG, "%d， %s\n", table->item[iItem].type, pLineItem);
    	setHeapTupleItemByStr(tuple + iItem, table->item[iItem].type, pLineItem, table->item[iItem].len);
		pLineItem = strtok(NULL, "|");
		iItem ++;
    }

    return;
}

/**
* vParseHeapTupleItem()		: 按元组解析文件数据
* @param (char* name)		: 文件名称
* @param (tableData* table)	: 表格结构指针
**/
void vParseHeapTupleItem(char* name, AttributeData* table)
{
    if(table == NULL || name == NULL)			// 参数检查处理
    {
        vmlog(PARSETUPLE_LOG, "<parseTuple.c>,tips: <table=%p> <name=%s>", table, name);
        return;
    }

    char buffer[FILE_LINE_LEN];
    FILE *fp = NULL;
    if((fp=fopen(name,"r")) == NULL)			// 文件是否被打开
    {
        vmlog(ERROR, "<parseTuple.c>,error: file(%s) open Failed!", name);
    }

    while(fgets(buffer, FILE_LINE_LEN, fp) != NULL)	// 循环读取文件内容
    {
        List listItem = NULL;
        vListInit(&listItem);
        vParseHeapTupleList(buffer, table, &listItem);
        vListPrint(&listItem);
    }

    fclose(fp);						// 关闭文件标示符
    return;
}

void getHeapTupleItemData(HeapTupleItem* opr1, DataType targetype, void* res)
{
	switch(opr1->type)
	{
	case LONG:
		switch(targetype)
		{
		case INTEGER:
			*(int32*)res =  opr1->itemdata.long_type.data;
			break;
		case LONG:
			*(int64*)res =  opr1->itemdata.long_type.data;
			break;
		case FLOAT:
			*(float*)res =  opr1->itemdata.long_type.data;
			break;
		case DOUBLE:
			*(double*)res =  opr1->itemdata.long_type.data;
			break;
		default:
			vmlog(ERROR, "getHeapTupleItemData---type conversion err");
		}
		break;
	case INTEGER:
		switch(targetype)
		{
		case INTEGER:
			*(int32*)res =  opr1->itemdata.int_type.data;
			break;
		case LONG:
			*(int64*)res =  opr1->itemdata.int_type.data;
			break;
		case FLOAT:
			*(float*)res =  opr1->itemdata.int_type.data;
			break;
		case DOUBLE:
			*(double*)res =  opr1->itemdata.int_type.data;
			break;
		default:
			vmlog(ERROR, "getHeapTupleItemData---type conversion err");
		}
		break;
	case FLOAT:
		switch(targetype)
		{
		case INTEGER:
			*(int32*)res =  opr1->itemdata.float_type.data;
			break;
		case LONG:
			*(int64*)res =  opr1->itemdata.float_type.data;
			break;
		case FLOAT:
			*(float*)res =  opr1->itemdata.float_type.data;
			break;
		case DOUBLE:
			*(double*)res =  opr1->itemdata.float_type.data;
			break;
		default:
			vmlog(ERROR, "getHeapTupleItemData---type conversion err");
		}
		break;
	case DOUBLE:
		switch(targetype)
		{
		case INTEGER:
			*(int32*)res =  opr1->itemdata.double_type.data;
			break;
		case LONG:
			*(int64*)res =  opr1->itemdata.double_type.data;
			break;
		case FLOAT:
			*(float*)res =  opr1->itemdata.double_type.data;
			break;
		case DOUBLE:
			*(double*)res =  opr1->itemdata.double_type.data;
			break;
		default:
			vmlog(ERROR, "getHeapTupleItemData---type conversion err");
		}
		break;
	case DATE:

	case CHAR:
		strcpy(res, opr1->itemdata.char_type.data);
		break;
	case VARCHAR:
		strcpy(res, opr1->itemdata.varchar_type.data);
		break;
	default:
		 vmlog(ERROR, "getHeapTupleItemData---item type unknown");
	}

}

/**
 * calculateHeapTupleItem ： by jyh
 * Arithmetic operation for numerical type
 * */
HeapTupleItem calculateHeapTupleItem(HeapTupleItem* opr1, HeapTupleItem* opr2, OpType op)
{
	DataType type;
	HeapTupleItem result;
	if(getOperatorNum(op) != 2)
	{
		 vmlog(ERROR, "calculateHeapTupleItem---the number of operators != 2");
	}
	type = judgeDataType(opr1, opr2);
	if(!IsNumericType(type))
	{
		vmlog(ERROR, "calculateHeapTupleItem--operator type error!");
	}

	switch(type)
	{
	case DOUBLE:
	{
		double res, data1, data2;
		getHeapTupleItemData(opr1, type, &data1);
		getHeapTupleItemData(opr2, type, &data2);
		switch(op)
		{
		case OP_ADD:
			res = data1 + data2;
			break;
		case OP_SUB:
			res  = data1 - data2;
			break;
		case OP_MUL:
			res = data1 * data2;
			break;
		case OP_DIV:
			res = data1 / data2;
			break;
		default:
			vmlog(ERROR, "calculateHeapTupleItem--operator error!");
		}
		setHeapTupleItemByData(&result, type, &res, 0);
		break;
	}
	case FLOAT:
	{
		float res, data1, data2;
		getHeapTupleItemData(opr1, type, &data1);
		getHeapTupleItemData(opr2, type, &data2);
		switch(op)
		{
		case OP_ADD:
			res = data1 + data2;
			break;
		case OP_SUB:
			res  = data1 - data2;
			break;
		case OP_MUL:
			res = data1 * data2;
			break;
		case OP_DIV:
			res = data1 / data2;
			break;
		default:
			vmlog(ERROR, "calculateHeapTupleItem--operator error!");
		}
		setHeapTupleItemByData(&result, type, &res, 0);
		break;
	}
	case LONG:
	{
		int64 res, data1, data2;
		getHeapTupleItemData(opr1, type, &data1);
		getHeapTupleItemData(opr2, type, &data2);
		switch(op)
		{
		case OP_ADD:
			res = data1 + data2;
			break;
		case OP_SUB:
			res  = data1 - data2;
			break;
		case OP_MUL:
			res = data1 * data2;
			break;
		case OP_DIV:
			res = data1 / data2;
			break;
		case OP_MOD:
			res = data1 % data2;
			break;
		default:
			vmlog(ERROR, "calculateHeapTupleItem--operator error!");
		}
		setHeapTupleItemByData(&result, type, &res, 0);
		break;
	}
	case INTEGER:
	{
		int32 res, data1, data2;
		getHeapTupleItemData(opr1, type, &data1);
		getHeapTupleItemData(opr2, type, &data2);
		switch(op)
		{
		case OP_ADD:
			res = data1 + data2;
			break;
		case OP_SUB:
			res  = data1 - data2;
			break;
		case OP_MUL:
			res = data1 * data2;
			break;
		case OP_DIV:
			res = data1 / data2;
			break;
		case OP_MOD:
			res = data1 % data2;
			break;
		default:
			vmlog(ERROR, "calculateHeapTupleItem--operator error!");
		}
		setHeapTupleItemByData(&result, type, &res, 0);
		break;
	}
	}
	return result;
}

DataType judgeDataType(HeapTupleItem* opr1, HeapTupleItem* opr2)
{
	DataType type = NONE;
	if(opr1->type != opr2->type)
	{
		if((IsNumericType(opr1->type)) && (IsNumericType(opr2->type)) &&((opr1->type == DOUBLE) || (opr2->type == DOUBLE)))
		{
			type = DOUBLE;
		}else if ((IsNumericType(opr1->type)) && (IsNumericType(opr2->type)) && ((opr1->type == FLOAT) || (opr2->type == FLOAT)))
		{
			type = FLOAT;
		}else if ((IsNumericType(opr1->type)) && (IsNumericType(opr2->type)) && ((opr1->type == LONG) || (opr2->type == LONG)))
		{
			type = LONG;
		}else if((IsNumericType(opr1->type)) && (IsNumericType(opr2->type)))
		{
			type = INTEGER;
		}else if((IsCharType(opr1->type)) && (IsCharType(opr2->type)))
		{
			type = VARCHAR;
		}else if((IsCharType(opr1->type) || IsDateType(opr1->type)) && (IsCharType(opr2->type) || IsDateType(opr2->type)))
		{
			type = DATE;
		}
	}else
	{
		type = opr1->type;
	}
	return type;
}

bool IsNumericType(DataType type)
{
	switch(type)
	{
	case INTEGER:
	case FLOAT:
	case LONG:
	case DOUBLE:
		return true;
	default:
		return false;
	}
}

bool IsCharType(DataType type)
{
	switch(type)
	{
	case CHAR:
	case VARCHAR:
		return true;
	default:
		return false;
	}
}

bool IsDateType(DataType type)
{
	switch(type)
	{
	case DATE:
		return true;
	default:
		return false;
	}
}

int cmpHeapTupleItem(HeapTupleItem* opr1, HeapTupleItem* opr2)
{
	DataType type;
	type = judgeDataType(opr1, opr2);
	if(type == NONE)
	{
		vmlog(ERROR, "calculateHeapTupleItem--operator type error!");
	}
	switch(type)
	{
	case DOUBLE:
	{
		double data1, data2;
		getHeapTupleItemData(opr1, type, &data1);
		getHeapTupleItemData(opr2, type, &data2);
		return (int)(data1 - data2);
	}
	case FLOAT:
	{
		float data1, data2;
		getHeapTupleItemData(opr1, type, &data1);
		getHeapTupleItemData(opr2, type, &data2);
		return (int)(data1 - data2);
	}
	case LONG:
	{
		int64 data1, data2;
		getHeapTupleItemData(opr1, type, &data1);
		getHeapTupleItemData(opr2, type, &data2);
		return (int)(data1 - data2);
	}
	case INTEGER:
	{
		int32  data1, data2;
		getHeapTupleItemData(opr1, type, &data1);
		getHeapTupleItemData(opr2, type, &data2);
		return (int)(data1 - data2);
	}
	case VARCHAR:
	case CHAR:
		return strcmp(opr1->itemdata.char_type.data, opr2->itemdata.char_type.data);
	case DATE:
	{
		pDate_type* date1;
		pDate_type* date2;
		if(IsCharType(opr1->type))
		{
			date1 = iniTypeDate(opr1->itemdata.char_type.data);
		}else
		{
			date1 = &(opr1->itemdata.date_type);
		}
		if(IsCharType(opr2->type))
		{
			date2 = iniTypeDate(opr2->itemdata.char_type.data);
		}else
		{
			date2 =  &(opr2->itemdata.date_type);
		}
		int result = dateCmp(date1, date2);
		if(IsCharType(opr1->type))
		{
			vfree((void**)(&date1));
		}
		if(IsCharType(opr2->type))
		{
			vfree((void**)(&date2));
		}
		return result;
	}
	}
}

