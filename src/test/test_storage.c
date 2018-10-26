/*
 * test_storage.c
 *
 *  Created on: Oct 18, 2017
 *      Author: jyh
 *     compile:gcc -Isrc/include src/utils/* src/storage/* src/parser/* src/dataType/* src/exectutor/*.c src/test/test_storage.c -o test_storage
 */
#include "storage/relation.h"
#include "test.h"

int main()
{
	time_t t_start = time(NULL);
	char *dataName  = TEST_DATA_FILE;
	char *tableName = TEST_TABLE_FILE;
	Oid tid = (Oid) 263;
	BFM_manager = BFM_CreateBufferManager();
	Relation relation = NULL;
	relation = Rel_CreateRelation("test", tid);
	Rel_vInitRelationAttrs(relation, tableName);
	//insert
	Tuple tuple = Tup_CreateTuple(relation);
	char buffer[FILE_LINE_LEN];
	FILE *fp = NULL;
	if((fp=fopen(dataName,"r")) == NULL)			// 文件是否被打开
	{
		vmlog(PARSETUPLE_LOG, "<parseTuple.c>,error: file(%s) open Failed!", dataName);
		exit(-1);
	}
	uint i = 0;
	while(fgets(buffer, FILE_LINE_LEN, fp) != NULL)	// 循环读取文件内容
	{
		vmlog(STORE_LOG, "--test_storage--index:%u", i);
		Tup_vConstructHeaptup(tuple, buffer);
		i ++;
		Tup_vInsert(tuple);
//		break;
	}

	Tup_vDestroyTuple(tuple);
	fclose(fp);
	BFM_vDestoryBufferManager(BFM_manager);
	Rel_vDestoryRelation(relation);
	time_t t_insert = time(NULL);
	vmlog(DISP_LOG, "test--start read");
	relation = Rel_CreateRelation("test", tid);
	Rel_vInitRelationAttrs(relation, tableName);
	BFM_manager = BFM_CreateBufferManager();
	Rel_vScanTable(relation);
	time_t t_read = time(NULL);
//	Rel_vDeleteTuple(relation);
//	vmlog(DISP_LOG, "test--after delete tuple with even index");
//	Rel_vScanTable(relation);
//	Rel_vDestoryRelation(relation);
//	relation = NULL;
//	BFM_vDestoryBufferManager(BFM_manager);
	time_t t_end = time(NULL);
	vmlog(WARNING, "total use time :%ld, insert time:%ld, read time:%ld",
			(t_end - t_start), (t_insert - t_start), (t_read - t_insert));
	return 0;
}
