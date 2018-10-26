/*
 * longType.c
 *
 *  Created on: Nov 26, 2017
 *      Author: jyh
 */

#include "storage/dataType.h"


pLongint_type* iniTypeLong(int64 Data)//初始化long类型：Data是需要写入的长整型数据，返回Longint_type结构体指针，返回0则空间分配失败
{
    pLongint_type *longint_instance=NULL;
    longint_instance=(pLongint_type*)vmalloc(sizeof(pLongint_type));
    if(longint_instance)
    {
        longint_instance->data=Data;
	//printf("%ld\n",longint_instance->data);
        return longint_instance;
    }
    return 0;
}

bool setTypeLongData(pLongint_type* longint_instance, int64 Data)
{
	longint_instance->data = Data;
	return true;
}

void destroyTypeLong(pLongint_type* longint_instance)
{
//	vfree((void**)&longint_instance);
}

int longMem2Disk(pLongint_type *longint_instance, char* buffer)//long类型：将内存中的结构序列化，写入buffer，返回写入的字节数
{
    char *dst=NULL,*src=NULL;
    memset(buffer,'\0',LONGINT_SIZE+1);
    dst=buffer;
    src=(char*)(&(longint_instance->data));
    memcpy(dst,src,LONGINT_SIZE);
//    vfree((void**)&longint_instance);
    return LONGINT_SIZE;
}
void  longDisk2Mem(pLongint_type *longint_instance, char* buffer,uint32 *number)//long类型：从buffer中获得8个字节数据，转换为Longint_type结构体，返回Longint_type结构体指针，返回0则空间分配失败，参数nu用于返回写入的字节数
{
    char *dst=NULL,*src=NULL;
    src=buffer;
    dst=(char*)(&(longint_instance->data));
	memcpy(dst,src,LONGINT_SIZE);
	*number=LONGINT_SIZE ;
}

int longCmp(pLongint_type* a, pLongint_type* b)
{
	return a->data - b->data;
}
