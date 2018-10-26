/*
 * floatType.c
 *
 *  Created on: Nov 26, 2017
 *      Author: jyh
 */

#include "storage/dataType.h"


pFloat_type* iniTypeFloat(float Data)//初始化float类型：Data是需要写入的长整型数据，返回Float_type结构体指针,返回0则空间分配失败
{
    pFloat_type *float_instance=NULL;
    float_instance=(pFloat_type*)vmalloc(sizeof(pFloat_type));
    if(float_instance)
    {
        float_instance->data=Data;
        return float_instance;
    }
    return 0;
}

bool setTypeFloatData(pFloat_type* float_instance, float Data )
{
	float_instance->data = Data;
	return true;
}

void destroyTypeFloat(pFloat_type* float_instance)
{
//	vfree((void**)&float_instance);
}

int floatMem2Disk(pFloat_type* float_instance, char* buffer)//float类型：将内存中的结构序列化，写入buffer，返回写入的字节数
{
    char *dst=NULL,*src=NULL;
    memset(buffer,'\0',FLOAT_SIZE+1);
    dst=buffer;
    src=(char*)(&(float_instance->data));
    memcpy(dst,src,FLOAT_SIZE);
//    vfree((void**)&float_instance);
    return FLOAT_SIZE;
}
void floatDisk2Mem(pFloat_type *float_instance, char* buffer,uint32 *number)//float类型：从buffer中获得4个字节数据，转换为Float_type结构体，返回Float_type结构体指针,返回0则空间分配失败，参数nu用于返回写入的字节数
{
    char *dst=NULL,*src=NULL;
	src=buffer;
	dst=(char*)(&(float_instance->data));
	memcpy(dst,src,FLOAT_SIZE);
	*number=FLOAT_SIZE;
}

int floatCmp(pFloat_type* a, pFloat_type* b)
{
	return (a->data - b->data);
}
