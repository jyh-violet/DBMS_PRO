/**
* name		: test_parserTable.c
* date		: 17/10/19
* author	: bsl
* description	: 测试 Table 的解析.
**/

#include "mparser/parseTable.h"

char *fileName="../table/table";

int main()
{
    vmlog(PARSETABLE_LOG, "------ <parseTable.c> begining ------");
    AttributeData table;
    vParseTable(fileName, &table);
    vTablePrint(&table);

    uint32 len = uGetMaxTupleLen(&table);
    vmlog(PARSETABLE_LOG, "<parseTable.c>,tips: <table.maxLen=%u>", len);
    vmlog(PARSETABLE_LOG, "------ <parseTable.c> ending ------");
    return 0;
}


