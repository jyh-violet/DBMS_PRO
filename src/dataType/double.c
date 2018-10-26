/*
 * double.c
 *
 *  Created on: Nov 26, 2017
 *      Author: jyh
 */

#include "storage/dataType.h"



pDouble_type* iniTypeDouble(double Data)//初始化double类型：Data是需要写入的长整型数据，返回Double_type结构体指针，返回0则空间分配失败
{
    pDouble_type *double_instance=NULL;
    double_instance=(pDouble_type*)vmalloc(sizeof(pDouble_type));
    if(double_instance)
    {
        double_instance->data=Data;
        return double_instance;
    }
    return 0;
}

bool setTypeDoubleData(pDouble_type* double_instance, double Data)
{
	double_instance->data = Data;
	return true;
}

void destroyTypeDouble(pDouble_type* double_instance)
{
//	vfree((void**)&double_instance);
}


int doubleMem2Disk(pDouble_type* double_instance, char* buffer)//double类型：将内存中的结构序列化，写入buffer，返回写入的字节数
{
    char *dst=NULL,*src=NULL;
    memset(buffer,'\0',DOUBLE_SIZE+1);
    dst=buffer;
    src=(char*)(&(double_instance->data));
    memcpy(dst,src,DOUBLE_SIZE);
//    vfree((void**)&double_instance);
    return DOUBLE_SIZE;
}
void doubleDisk2Mem(pDouble_type *double_instance, char* buffer,uint32 *number)//double类型：从buffer中获得8个字节数据，转换为Double_type结构体，返回Double_type结构体指针，返回0则空间分配失败，参数number用于返回写入的字节数
{
	char *dst=NULL,*src=NULL;
	src=buffer;
	dst=(char*)(&(double_instance->data));
	memcpy(dst,src,DOUBLE_SIZE);
	*number=DOUBLE_SIZE;
}

int doubleCmp(pDouble_type* a, pDouble_type* b)
{
	return (a->data - b->data);
}
