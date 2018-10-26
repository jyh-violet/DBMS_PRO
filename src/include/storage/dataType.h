#ifndef DATATYPE_H_
#define DATATYPE_H_

#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "utils/mlog.h"
#define INT_SIZE 4
#define LONGINT_SIZE 8
#define FLOAT_SIZE 4
#define DOUBLE_SIZE 8
#define YMD_SIZE 8
#define YEAR_SIZE 4
#define MONTH_SIZE 2
#define DAY_SIZE 2

#define YEAR_OFF 0
#define MONTH_OFF 4
#define DAY_OFF 6

#define YEAR_BASE 10000
#define MONTH_BASE 100
#define DAY_BASE	1

typedef  struct int_type{
	    int32 data;
	}pInt_type;
typedef  struct char_type{
	    char *data;
	    uint32 length;
	}pChar_type;
typedef  struct varchar_type{
	    char *data;
        uint32 max_length;
	    uint32 length;
	}pVarchar_type;
typedef  struct longint_type{
	    int64 data;
	}pLongint_type;
typedef  struct float_type{
	    float data;
	}pFloat_type;
typedef  struct double_type{
	    double data;
	}pDouble_type;
typedef  struct date_type{
	//    char date[YMD_SIZE + 1];
		uint32 date; // 2017-01-01 -> 20170101
	}pDate_type;


pInt_type *iniTypeInt(int32 Data);//初始化int类型：Data是需要写入的整型数据，返回pInt_type结构体指针，返回0则空间分配失败
int intMem2Disk(pInt_type* int_instance, char* buffer);//int类型：将内存中的结构序列化，写入buffer，返回写入的字节数
pChar_type* iniTypeChar(char* Data, uint32 n);//初始化char类型：Data是需要写入的字符串，n是字符串最大长度，返回Char_type结构体指针,返回0则空间分配失败
int charMem2Disk(pChar_type *char_instance, char *buffer);//char类型：将内存中的结构序列化，写入buffer，返回写入的字节数
pVarchar_type* iniTypeVarchar(char *Data, uint32 n);//初始化varchar类型：Data是需要写入的字符串，n是最大长度，返回Varchar_type结构体指针,返回0则空间分配失败
int varcharMem2Disk(pVarchar_type *varchar_instance, char *buffer);//varchar类型：将内存中的结构序列化，写入buffer，返回写入的字节数
pLongint_type* iniTypeLong(int64 Data);//初始化long类型：Data是需要写入的长整型数据，返回Longint_type结构体指针，返回0则空间分配失败
int longMem2Disk(pLongint_type *longint_instance, char* buffer);//long类型：将内存中的结构序列化，写入buffer，返回写入的字节数
pFloat_type* iniTypeFloat(float Data);//初始化float类型：Data是需要写入的长整型数据，返回Float_type结构体指针,返回0则空间分配失败
int floatMem2Disk(pFloat_type* float_instance, char* buffer);//float类型：将内存中的结构序列化，写入buffer，返回写入的字节数
pDouble_type* iniTypeDouble(double Data);//初始化double类型：Data是需要写入的长整型数据，返回Double_type结构体指针，返回0则空间分配失败
int doubleMem2Disk(pDouble_type* double_instance, char* buffer);//double类型：将内存中的结构序列化，写入buffer，返回写入的字节数
pDate_type* iniTypeDate(char* Data);//初始化date类型：Data是需要写入的长整型数据，返回Date_type结构体指针,返回0则空间分配失败或时间格式不支持
int dateMem2Disk(pDate_type* date_instance, char* buffer);//date类型：将内存中的结构序列化，写入buffer，返回0表示成功

//int类型：从buffer中获得4个字节数据，返回Int_type结构体指针，返回0则空间分配失败，参数number用于返回写入的字节数
void  intDisk2Mem(pInt_type *int_instance, char* buffer,uint32 *number );
void charDisk2Mem(pChar_type *char_instance, char *buffer, uint32 *number);
void varcharDisk2Mem(pVarchar_type *varchar_instance, char *buffer,uint32 *number);
void  longDisk2Mem(pLongint_type *longint_instance, char* buffer,uint32 *number);
void floatDisk2Mem(pFloat_type *float_instance, char* buffer,uint32 *number);
void doubleDisk2Mem(pDouble_type *double_instance, char* buffer,uint32 *number);
void dateDisk2Mem(pDate_type *date_instance,char* buffer,uint32 *number);

void destroyTypeInt(pInt_type* int_instance);
void destroyTypeChar(pChar_type* char_instance);
void destroyTypeVarchar(pVarchar_type* varchar_instance);
void destroyTypeLong(pLongint_type* longint_instance);
void destroyTypeFloat(pFloat_type* float_instance);
void destroyTypeDouble(pDouble_type* double_instance);
void destroyTypeDate(pDate_type* date_instance);

bool setTypeIntData(pInt_type* int_instance, int32 data);
bool setTypeDateData(pDate_type* date_instance, char* data);
bool setTypeDoubleData(pDouble_type* double_instance, double Data);
bool setTypeFloatData(pFloat_type* float_instance, float Data );
bool setTypeCharData(pChar_type* char_instance, char* data);
bool setTypeLongData(pLongint_type* longint_instance, int64 Data);
bool setTypeVarcharData(pVarchar_type* varchar_instance, char* data);

void initTypeCharData(pChar_type* char_instance, uint32 len);
void initTypeVarcharData(pVarchar_type* varchar_instance, uint32 len);

int dateCmp(pDate_type* a, pDate_type* b);

#endif
