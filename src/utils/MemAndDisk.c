/**
* name		: MemAndDisk.c
* date		: 17/10/23
* author	: bsl
* description	: 硬盘和内存存储结构的转换
**/

#include "MemAndDisk.h"

/**
* iTuple2Buffer()		: 将tupleData数据转为buffer
* @param (char* buffer)		: 硬盘存储结构buffer
* @param (tupleData* tuple)	: 内存存储结构tupleData
**/
int32 iTuple2Buffer(char* buffer, HeapTupleItem* tuple)
{
    int32 size = 0;
    if(buffer == NULL || tuple == NULL)				// 参数检查
    {
        vmlog(ERROR, "<MemAndDisk.c>,error: iTuple2Buffer() <buffer=%s> <tuple=%p>", buffer, tuple);
    }
    if(tuple->type == INTEGER)
    {
        size = intMem2Disk(&(tuple->itemdata.int_type), buffer);
    }else if(tuple->type == CHAR)
    {
        size = charMem2Disk(&(tuple->itemdata.char_type), buffer);
    }else if(tuple->type == VARCHAR)
    {
        size = varcharMem2Disk(&(tuple->itemdata.varchar_type), buffer);
    }else if(tuple->type == FLOAT)
    {
        size = floatMem2Disk(&(tuple->itemdata.float_type), buffer);
    }else if(tuple->type == LONG)
    {
        size = longMem2Disk(&(tuple->itemdata.long_type), buffer);
    }else if(tuple->type == DATE)
    {
        size = dateMem2Disk(&(tuple->itemdata.date_type), buffer);
    }else if(tuple->type == DOUBLE)
    {
        size = doubleMem2Disk(&(tuple->itemdata.double_type), buffer);
    }
    return size;
}

/**
* vMem2Disk()		: 将内存中一条记录的存储结构list转为硬盘存储结构buffer
* @param (char* buffer)	: 硬盘存储结构buffer
* @param (List* list)	: 内存存储结构list
**/
uint uMem2Disk(char* buffer, HeapTupleItem* tuple, uint item_num)
{
    char *str = buffer;
    int32_t offset = 0;

    uint i = 0;
    for(i = 0;i < item_num; i ++)
    {
    	offset += iTuple2Buffer(buffer, tuple + i);
    	if(offset >= (FILE_LINE_LEN - 1))			// 缓冲区长度检查
		{
			vmlog(ERROR, "<MemAndDisk.c>,error: vMem2Disk() <offset=%d>", offset);
		}
		buffer = str + offset;
    }

    buffer = str;
    if(offset % alignByte != 0)
    {
    	offset = offset + alignByte -offset % alignByte;
    }
    return offset;
}

/**
* vBuffer2List()		: 将一条记录buffer转为一个list
* @param (char* buffer)		: 记录指针buffer
* @param (List* list)		: 链表头指针list
* @param (tableItem item)	: table结构之数据元素
**/
uint32 iBuffer2List(char* buffer, HeapTupleItem* tuple, AttributeDataItem item)
{
	vmlog(MEMANDDISK_LOG, "--MemAndDisk.c/iBuffer2List--start");
    uint32 size = 0;
    void *cell = NULL;

    if(item.type == INTEGER)
    {
        intDisk2Mem(&(tuple->itemdata.int_type), buffer, &size);
    }else if(item.type == CHAR)
    {
        charDisk2Mem(&(tuple->itemdata.char_type), buffer, &size);
    }else if(item.type == VARCHAR)
    {
        varcharDisk2Mem(&(tuple->itemdata.varchar_type),buffer, &size);
    }else if(item.type == FLOAT)
    {
        floatDisk2Mem(&(tuple->itemdata.float_type),buffer, &size);
    }else if(item.type == LONG)
    {
        longDisk2Mem(&(tuple->itemdata.long_type),buffer, &size);
    }else if(item.type == DATE)
    {
        dateDisk2Mem(&(tuple->itemdata.date_type),buffer, &size);
    }else if(item.type == DOUBLE)
    {
        doubleDisk2Mem(&(tuple->itemdata.double_type),buffer, &size);
    }

    tuple->type = item.type;				// 链表元素插入
    vmlog(MEMANDDISK_LOG, "--MemAndDisk.c/iBuffer2List--end");
    return size;
}


/**
* vDisk2Mem()			: 将硬盘中一条记录的存储结构buffer转为硬盘中存储结构list
* @param (char* buffer)		: 硬盘存储结构buffer
* @param (List* list)		: 内存存储结构list
* @param (tableData* table)	: 数据表结构指针table
**/
void vDisk2Mem(char* buffer, HeapTupleItem* tuple, AttributeData *table)
{
	vmlog(MEMANDDISK_LOG, "--MemAndDisk.c/vDisk2Mem--start");
    if(buffer == NULL || table == NULL)			// 参数检查
    {
        vmlog(ERROR, "<MemAndDisk.c>,error: vDisk2Mem() the param <buffer=%s> <table=%p>", buffer, table);
    }
    vmlog(MEMANDDISK_LOG, "<MemAndDisk.c>,tips: vDisk2Mem() <table.name=%s> <table.items=%d>", table->name, table->item_num);
    int32  i = 0;
    uint32 offset = 0;
    char* str = buffer;

 //   for(i = table->item_num - 1; i >= 0; i --)
    for(i = 0; i < table->item_num; i ++)
    {
        offset += iBuffer2List(buffer, tuple + i, table->item[i]);					// 将buffer转为list
        if(offset >= (FILE_LINE_LEN - 1))
        {
        	vmlog(MEMANDDISK_LOG, "<MemAndDisk.c>,error: vDisk2Mem() <offset=%d>", offset);
	    	exit(-1);
        }
        buffer = str + offset;
    }

    buffer = str;
    vmlog(MEMANDDISK_LOG, "--MemAndDisk.c/vDisk2Mem--end");
    return;
}

