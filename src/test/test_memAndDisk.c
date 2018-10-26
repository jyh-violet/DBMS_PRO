/**
* name		: test_memAndDisk.c
* date		: 17/10/19
* author	: bsl
* description	: 测试 memory to disk 和 disk to memory.
**/
#include "MemAndDisk.h"

char *tableName = "../table/table";

int main()
{
    vmlog(PARSETUPLE_LOG, "------ <test_parseTuple.c> begining ------");
    AttributeData table;
    vParseTable(tableName, &table);
    vTablePrint(&table);

    char buffer[] = "2|Supplier#000000002|89eJ5ksX3ImxJQBvxObC,|5|15-679-861-2259|4032.68| slyly bold instructions. idle dependen";
    char result[FILE_LINE_LEN] = {0};
    List listItem = NULL;
    vListInit(&listItem);
    vParseHeapTupleList(buffer, &table, &listItem);

    uMem2Disk(result, &listItem);
    
    List newList = NULL;
    vListInit(&newList);
    vDisk2Mem(result, &newList, &table);
    vmlog(PARSETUPLE_LOG, "------ <test_parseTuple.c> ending ------");
    return 0;
}

