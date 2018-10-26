/**
* name		: SwizzMap.c
* date		: 17/10/19
* author	: bsl
* description	: 硬盘和内存对应表
**/

#include "SwizzMap.h"

/**
* compare()		: 比较记录当中存储硬盘的地址是否和真实的硬盘地址相同
* @param (void* smd)	: 记录指针
* @param (void* virtual): 硬盘地址指针
**/
bool compare(void* smd, void* virtual)
{
    SwizzMapData *c_smd = smd;
    if(c_smd == NULL || virtual == NULL)
    {
    	return false;
    }

    if(c_smd->virtual == virtual)
    {
    	return true;
    }
} 


/**
* vSwizzMapInit()	: 初始化硬盘和内存对应表
**/
void vSwizzMapInit()
{
    vListInit(&(SwizzMap.list));
}

/**
* SwizzMapDataInit()	: 初始化对应表的一条记录
* @param (void* virtual): 硬盘地址
* @param (void* actual) : 内存地址
**/
SwizzMapData* pSwizzMapDataInit(void* virtual, void* actual)
{
    SwizzMapData* p = (SwizzMapData*)vmalloc(sizeof(SwizzMapData));
    if(p == NULL)
    {
    	vmlog(SWIZZMAP_LOG, "<SwizzMap.c>,error: SwizzMapDataInit() Failed!");
    	exit(-1);
    }

    p->virtual = virtual;
    p->actual  = actual;
    
    return p;
} 

/**
* vSwizzMapInsert()		: 增加一条记录
* @param (SwizzMapData*)	: 记录指针
**/
void vSwizzMapInsert(SwizzMapData* smd)
{
    vListInsert(&(SwizzMap.list), smd);
}

/**
* vSwizzMapPrint()	: 打印对应表
**/
void vSwizzMapPrint()
{
    vListPrint(&(SwizzMap.list));
}

/**
* vSwizzMapDestroy()	: 销毁对应表
**/
void vSwizzMapDestroy()
{
    vListDestroy(&(SwizzMap.list));
}

/**
* pSwizzMapFind()	: 地址查找
* (void* virtual)	: 硬盘地址
**/
void* pSwizzMapFind(void* virtual)
{
    SwizzMapData* smd = pListFind(&(SwizzMap.list), virtual, compare);
    if (smd == NULL)
    {
    	return NULL;
    }

    return smd->actual;
}

/**
* vSwizzMapDelete()	: 根据硬盘地址进行记录删除
* @param (void* virtual): 硬盘地址
**/
void vSwizzMapDelete(void* virtual)
{
    pListRemove(&(SwizzMap.list), virtual, compare);
}


