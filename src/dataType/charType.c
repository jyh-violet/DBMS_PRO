/*
 * charType.c
 *
 *  Created on: Nov 26, 2017
 *      Author: jyh
 */

#include "storage/dataType.h"


pChar_type* iniTypeChar(char* Data, uint32 n)//初始化char类型：Data是需要写入的字符串，n是字符串最大长度，返回Char_type结构体指针,返回0则空间分配失败
{
    char *dst=NULL,*src=NULL;
    pChar_type *char_instance=NULL;
    char_instance=(pChar_type*)vmalloc(sizeof(pChar_type));
    if(char_instance)
    {
        char_instance->length=n;
        char_instance->data=(char*)vmalloc(sizeof(char)*(char_instance->length+1));
        if(char_instance->data)
        {
            memset(char_instance->data,'\0',char_instance->length+1);
            dst=char_instance->data;
            src=Data;
            memcpy(dst,src,strlen(Data));
            return char_instance;
        }
        return 0;
    }
    return 0;
}

void initTypeCharData(pChar_type* char_instance, uint32 len)
{
	char_instance->length = len;
	char_instance->data=(char*)vmalloc(sizeof(char)*(char_instance->length+1));
}

bool setTypeCharData(pChar_type* char_instance, char* data)
{
	bool result = true;
	if(strlen(data) > char_instance->length)
	{
		char_instance->length = strlen(data);
		result =  false;
		vfree((void**)&(char_instance->data));
		char_instance->data = vmalloc(strlen(data) * sizeof(char) + 1);
	}else if(char_instance->data == NULL)
	{
		char_instance->data = vmalloc(char_instance->length * sizeof(char) + 1);
	}
	memcpy(char_instance->data, data, strlen(data) + 1);
	return result;
}

void destroyTypeChar(pChar_type* char_instance)
{
	vfree((void**) &(char_instance->data));
//	vfree((void**)&char_instance);
}

int charMem2Disk(pChar_type *char_instance, char *buffer)//char类型：将内存中的结构序列化，写入buffer，返回写入的字节数
{
    char *dst=NULL,*src=NULL;
    int writelen = char_instance->length;
    if(writelen % alignByte != 0)
	{
    	writelen = writelen + alignByte -writelen % alignByte;
	}
    memset(buffer,'\0',writelen+INT_SIZE+1);
    dst=buffer;
    src=(char*)(&(writelen));
    memcpy(dst,src,INT_SIZE);
    src=char_instance->data;
    dst=dst+INT_SIZE;
    memcpy(dst,src,char_instance->length);
    uint len = INT_SIZE+writelen;
//    vfree((void**) &(char_instance->data));
//    vfree((void**)&char_instance);
    return len;
}
void charDisk2Mem(pChar_type *char_instance, char *buffer, uint32 *number)//char类型：从buffer中获得数据，转换为Char_type结构体，返回Char_type结构体指针,返回0则空间分配失败，参数number用于返回写入的字节数
{
    char *dst=NULL,*src=NULL;
	dst=(char*)(&(char_instance->length));
	src=buffer;
	memcpy(dst,src,INT_SIZE);
	src=src+INT_SIZE;
	char_instance->data=(char*)vmalloc(char_instance->length+1);
	dst=(char*)(char_instance->data);
	memset(dst,'\0',char_instance->length+1);
	memcpy(dst,src,char_instance->length);
	*number=INT_SIZE+char_instance->length;
}

int charCmp(pChar_type* a, pChar_type* b)
{
	return strcmp(a->data, b->data);
}
