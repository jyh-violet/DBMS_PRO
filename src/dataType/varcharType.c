/*
 * varcharType.c
 *
 *  Created on: Nov 26, 2017
 *      Author: jyh
 */
#include "storage/dataType.h"

pVarchar_type* iniTypeVarchar(char *Data, uint32 n)//初始化varchar类型：Data是需要写入的字符串，n是最大长度，返回Varchar_type结构体指针,返回0则空间分配失败
{
    pVarchar_type *varchar_instance=NULL;
    char *dst=NULL,*src=NULL;
    varchar_instance=(pVarchar_type*)vmalloc(sizeof(pVarchar_type));
    varchar_instance->max_length=n;
	varchar_instance->length=strlen(Data);
	varchar_instance->data=(char*)vmalloc0(sizeof(char)*(varchar_instance->length+1));
	src=Data;
	dst=varchar_instance->data;
	memcpy(dst,src,varchar_instance->length);
	return varchar_instance;
}

void initTypeVarcharData(pVarchar_type* varchar_instance, uint32 len)
{
	varchar_instance->max_length = len;
	varchar_instance->length = 0;
	varchar_instance->data=(char*)vmalloc(sizeof(char)*(varchar_instance->max_length+1));
}


bool setTypeVarcharData(pVarchar_type* varchar_instance, char* data)
{
	bool result = true;
	if(strlen(data) > varchar_instance->max_length)
	{
		varchar_instance->max_length = strlen(data);
		result =  false;
		vfree((void**)&(varchar_instance->data));
		varchar_instance->data = vmalloc(strlen(data) * sizeof(char) + 1);
	}else if(varchar_instance->data == NULL)
	{
		varchar_instance->data = vmalloc(varchar_instance->max_length * sizeof(char) + 1);
	}
	memcpy(varchar_instance->data, data, strlen(data) + 1);
	varchar_instance->length =  strlen(data);
	return result;
}
void destroyTypeVarchar(pVarchar_type* varchar_instance)
{
	vfree((void**) &(varchar_instance->data));
//	vfree((void**)&varchar_instance);
}

int varcharMem2Disk(pVarchar_type *varchar_instance, char *buffer)//varchar类型：将内存中的结构序列化，写入buffer，返回写入的字节数
{
    char *dst=NULL,*src=NULL;
    int writelen = varchar_instance->length;
	if(writelen % alignByte != 0)
	{
		writelen = writelen + alignByte -writelen % alignByte;
	}
    memset(buffer,'\0',writelen+2*INT_SIZE+1);
    dst=buffer;
    src=(char*)(&(writelen));
    memcpy(dst,src,INT_SIZE);
    src=varchar_instance->data;
    dst=dst+INT_SIZE;
    memcpy(dst,src,varchar_instance->length);
    uint len = writelen+INT_SIZE;
//    vfree((void**)&(varchar_instance->data));
//    vfree((void**)&varchar_instance);
    return len;
}
void varcharDisk2Mem(pVarchar_type *varchar_instance, char *buffer,uint32 *number)//varchar类型：从buffer中获得数据，转换为Char_type结构体，返回Varchar_type结构体指针,返回0则空间分配失败，参数number用于返回写入的字节数
{
    char *dst=NULL,*src=NULL;
    src=buffer;
	dst=(char*)(&(varchar_instance->length));
	memcpy(dst,src,INT_SIZE);
	varchar_instance->data=(char*)vmalloc0(sizeof(char)*(varchar_instance->length+1));
	dst=varchar_instance->data;
	src=src+INT_SIZE;
	memcpy(dst,src,varchar_instance->length);
	*number=INT_SIZE+varchar_instance->length;
}

int varcharCmp(pVarchar_type* a, pVarchar_type* b)
{
	return strcmp(a->data, b->data);
}
