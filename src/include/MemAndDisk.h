/**
* name		: MemAndDisk.c
* date		: 17/10/23
* author	: bsl
* description	: 硬盘和内存数据存储结构的转换
**/

#ifndef MEMANDDISK_H_
#define MEMANDDISK_H_

#include "mparser/parseTuple.h"
int32 iTuple2Buffer(char* buffer, HeapTupleItem* tuple);
uint uMem2Disk(char* buffer, HeapTupleItem* tuple, uint item_num);
uint32 iBuffer2List(char* buffer, HeapTupleItem* tuple, AttributeDataItem item);
void vDisk2Mem(char* buffer, HeapTupleItem* tuple, AttributeData *table);

#endif

