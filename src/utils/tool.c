/*
 * tool.c
 *
 *  Created on: Nov 14, 2017
 *      Author: jyh
 */

#include "utils/tool.h"
#include "utils/mlog.h"
#include "utils/LinkList.h"

uint32 uiCalPower(uint32 base, uint32 index);

void* vmalloc(unsigned int size)
{
	void* p = malloc(size );
	vmlog(MALLOC_LOG, "vmalloc-- malloc:%p", p);
//	print_trace();
	if(p == NULL)
	{
		vmlog(ERROR, "malloc error! malloc size:%d!", size);
		print_trace();
		exit(-1);
	}
	return p;
}
/**
 * vmalloc0 : by jyh
 * maclloc the space and set to 0;
 * */
void* vmalloc0(unsigned int size)
{
	void* p = vmalloc(size);
	memset(p, 0, size);
	return p;
}

/**
 * vfree : by jyh
 * NOTE: in order to change the pointer to  NULL, the parameter is the pointer of the pointer variable
 * */
void vfree(void** p)
{
	if(*p == NULL)
	{
	//	vmlog(MALLOC_LOG, "vfree-- NULL");
		return;
	}
	vmlog(MALLOC_LOG, "vfree-- free:%p", *p);
	free(*p);
	*p = NULL;
//	print_trace();
//	vmlog(MALLOC_LOG, "vfree-- end");
}


/**
* char2Int()		: char类型转为32位有符号的int
* @param (char* str)	: 字符参数
**/
int32 char2Int(char* str)
{
    return atoi(str);
}


/**
* char2Double()		: char类型转为double
* @param (char* str)	: 字符参数
**/
double char2Double(char* str)
{
    return atof(str);
}



/**
* char2Float()          : char类型转为float
* @param (char* buffer) : 字符参数
* need to be updated
**/
float char2Float(char *buffer)
{
	return atof(buffer);
}


uint32 uiCalPower(uint32 base, uint32 index)//计算base^index
{
	uint32 i=0,uiRes=1;
	for(i=0;i<index;i++)
	{
		uiRes=uiRes*base;
	}
	return uiRes;
}

/**
* char2Uint()		: char类型转为uint32
* @param (char* buffer) : 字符参数
* need to be updated
**/
uint32 char2Uint(char *buffer)
{
    uint32 uLen=0,uRes=0,i=0;
    uLen=strlen(buffer);//计算位数
    for(i=0;i<uLen;i++)//Eg: 123=1*(10^2)+2*(10^1)+3*(10^0)
    {
        uRes=uRes+(buffer[i]-'0')*uiCalPower(10,uLen-1-i);
    }

    return uRes;
}

