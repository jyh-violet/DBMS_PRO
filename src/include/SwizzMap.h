/**
* name		: SwizzMap.h
* date		: 17/10/19
* author	: bsl
* description	: 对应表
**/

#ifndef SWIZZMAP_H_
#define SWIZZMAP_H_

#include "utils/LinkList.h"

typedef struct SwizzMapData
{
    void* virtual;
    void* actual;
}SwizzMapData;

struct SwizzMap{
    List list;
}SwizzMap;


SwizzMapData* pSwizzMapDataInit(void* virtual, void* actual);
void vSwizzMapInit();
void vSwizzMapInsert(SwizzMapData* smd);
void vSwizzMapPrint();
void vSwizzMapDestroy();
void* pSwizzMapFind(void* virtual);
void vSwizzMapDelete(void* virtual);


#endif

