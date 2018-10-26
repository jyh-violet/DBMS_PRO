/*
 * intType.c
 *
 *  Created on: Nov 26, 2017
 *      Author: jyh
 */
#include "storage/dataType.h"

pInt_type *iniTypeInt(int32 Data)//初始化int类型：Data是需要写入的整型数据，返回Int_type结构体指针，返回0则空间分配失败
{
    pInt_type *int_instance=NULL;
    int_instance=(pInt_type*)vmalloc(sizeof(pInt_type));
    if(int_instance)
    {
        int_instance->data=Data;
        return int_instance;
    }
    return 0;
}

bool setTypeIntData(pInt_type* int_instance, int32 data)
{
	int_instance->data = data;
	return true;
}

void destroyTypeInt(pInt_type* int_instance)
{
//	vfree((void**) &int_instance);
}

int intMem2Disk(pInt_type* int_instance, char* buffer)//int类型：将内存中的结构序列化，写入buffer，返回写入的字节数
{
    char *dst=NULL,*src=NULL;
    memset(buffer,'\0',INT_SIZE+1);
    src=(char*)(&int_instance->data);
    dst=buffer;
    memcpy(dst,src,INT_SIZE);
//    vfree((void**)&int_instance);
    return INT_SIZE;
}

void  intDisk2Mem(pInt_type *int_instance, char* buffer,uint32 *number )//int类型：从buffer中获得4个字节数据，返回Int_type结构体指针，返回0则空间分配失败，参数number用于返回写入的字节数
{
    char *dst=NULL,*src=NULL;
    dst=(char*)(&(int_instance->data));
	src=buffer;
	memcpy(dst,src,INT_SIZE);
	*number=INT_SIZE;
}

int intCmp(pInt_type* a, pInt_type* b)
{
	return a->data - b->data;
}
