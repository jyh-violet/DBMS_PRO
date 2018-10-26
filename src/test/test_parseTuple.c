#include "mparser/parseTuple.h"

char *dataName  = "../data/supplier.tbl";
char *tableName = "../table/table";
int main()
{
    vmlog(PARSETUPLE_LOG, "------ <test_parseTuple.c> begining ------");
    AttributeData table;
    vParseTable(tableName, &table);
    vTablePrint(&table);

    vParseHeapTupleItem(dataName, &table);
    vmlog(PARSETUPLE_LOG, "------ <test_parseTuple.c> ending ------");
    return 0;
}

