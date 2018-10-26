/*
 * init.c
 *
 *  Created on: Nov 24, 2017
 *      Author: jyh
 *     Compile: gcc -std=c99 -Isrc/include src/utils/* src/storage/* src/parser/* src/dataType/* src/exectutor/* src/server/* -o init
 */
#include "server/sysinit.h"
#include "test.h"
#include "exectutor/execTable.h"

/**
 * main : by jyh
 * the server process running background
 * */
int main()
{
	initSysTable();
	Relation relation = Rel_CreateRelation("test", 0);
	Rel_vInitRelationAttrs(relation, TEST_TABLE_FILE);
	vmlog(DISP_LOG, "init---create table start");
	execCreateTable(relation);
	Rel_vInitRelationAttrs(relation, TEST_TABLE_LINE_FILE);
	execCreateTable(relation);
	vmlog(DISP_LOG, "init---scan systable");
	Rel_vScanTable(sysTable);
	vmlog(DISP_LOG, "init---scan sysAttr");
	Rel_vScanTable(sysAttr);
	destorySysTable();
}
