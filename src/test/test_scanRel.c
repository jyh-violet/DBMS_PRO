/*
 * test_scanRel.c
 *
 *  Created on: Oct 24, 2017
 *      Author: jyh
 *     compile:	gcc -g -w lex.yy.c grammer.tab.c src/test/test_scanRel.c -o test_scanRel src/storage/*.c src/utils/*.c src/parser/*.c  src/dataType/*.c src/server/*.c src/exectutor/*.c -I src/include/ -g
 *
 */

#include "storage/relation.h"
#include "mparser/analyze.h"
#include "test.h"
#include "server/sysinit.h"

int main()
{
	initSysTable();
	time_t t_start = time(NULL);
	char *dataName  = TEST_DATA_FILE;
	char *tableName = TEST_TABLE_FILE;
	Oid tid = (Oid) 257;
	Relation relation = NULL;
	relation = Rel_CreateRelation("t2", tid);
	getRelationAttrs(relation);
//	Rel_vInitRelationAttrs(relation, tableName);

	Rel_vScanTable(relation);
	time_t t_read = time(NULL);
	Rel_vDestoryRelation(relation);
	relation = NULL;
	time_t t_end = time(NULL);
	vmlog(WARNING, "total use time :%ld,  read time:%ld",
			(t_end - t_start), (t_read - t_start));

	destorySysTable();
	return 0;
}
