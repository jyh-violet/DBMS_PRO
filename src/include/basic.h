/*
 * basic.h
 *
 *  Created on: Oct 8, 2017
 *      Author: jyh
 *        desc: some basic setting and define that could be used by the whole project
 */
#ifndef BASIC_H_
#define BASIC_H_

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "utils/tool.h"
#include "utils/node.h"


#ifndef bool
typedef enum bool{
	false,
	true
}bool;
#endif
#define alignByte 4
#define NOTableName ""
#define BLOCKSIZE (8 * 1024)
#define MaxFileNumforRelation 10 //the number of files that is allowed to create for a relation

#define ZERO 0
#define FAILURE -1

#define FLEXIBLE_ARRAY_MEMBER 1 //used for flexible array

#define min(a, b)  ((a) < (b) ? (a) : (b))

typedef enum M_type{

	//buffer type
	B_block,
	F_dataFile,
	F_indexFile
}M_type;

#endif /* BASIC_H_ */
