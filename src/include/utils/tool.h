/*
 * tool.h
 *
 *  Created on: Nov 14, 2017
 *      Author: jyh
 */

#ifndef TOOL_H_
#define TOOL_H_

#include<execinfo.h>
#include "utils/mlog.h"

typedef char int8;
typedef short int int16;
typedef int int32; //sizeof(int) = 4, for ubuntu 14.04 64bit
typedef long int int64; //sizeof(long) = sizeof(long long) = 4, for ubuntu 14.04 64bit
typedef unsigned char uint8;
typedef unsigned int uint;
typedef unsigned short int uint16;
typedef unsigned int uint32; //sizeof(int) = 4, for ubuntu 14.04 64bit
typedef unsigned long int uint64; //sizeof(long) = sizeof(long long) = 4, for ubuntu 14.04 64bit

typedef uint Oid;

typedef uint16 LocationIndex;

void* vmalloc(unsigned int size);
void* vmalloc0(unsigned int size);
void vfree(void** p);

int32 char2Int(char* str);
double char2Double(char* str);
float char2Float(char *buffer);
uint32 char2Uint(char *buffer);

#endif /* TOOL_H_ */
