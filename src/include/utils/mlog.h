/*
 * mlog.h
 *
 *  Created on: Oct 7, 2017
 *      Author: jyh
 */
#ifndef MLOG_H_
#define MLOG_H_
#include "basic.h"

//used for log_level. when it is true, the log with this level will be print
typedef enum LOGLEVEL{
	MALLOC_LOG,
	PARSETUPLE_LOG,
	MALLOC_TRACE,
	BASIC_LOG,
	BUFFER_LOG,
	FILE_LOG,
	RELATION_LOG,
	TUPLE_LOG,
	BLOCK_LOG,
	STORE_LOG,
	LINKLIST_LOG,
	SWIZZMAP_LOG,
	FSM_LOG,
	PARSETABLE_LOG,
	MEMANDDISK_LOG,
	VACUUM_LOG,

	LOG_PRINT, //the divider. That is less than it not print, and larger than this , print

	OPTIMIZER_LOG,
	INIT_LOG,
	PARSER_LOG,
    GRAMMER_LOG,
	DISP_LOG,
	WARNING,
	PROMPT,
	ERROR
}LOGLEVEL;


#define MAX_LOG_SIZE 1024*1024
#define LOG_PATH "log"

void vmlog(LOGLEVEL log_level, char* fmat, ...);

void print_trace();
#endif /* MLOG_H_ */
