/**
* name		: parseTable.c
* date		: 17/10/22
* author	: bsl
* description	: 解析table,提取table结构
**/

#include "mparser/parseTable.h"

/**
* uGetMaxTupleLen()		: 返回table元素最大所需空间
* @param (tableData* table)	: table指针
**/
uint32 uGetMaxTupleLen(AttributeData* table)
{
    uint32 len = 0;

    if(table == NULL)
    {
        vmlog(ERROR, "<parseTable.c>,error: uGetMaxTupleLen() <table=null>");
    }

    uint32 i = 0;

    for(i = 0; i < table->item_num; i ++)
    {
	len += table->item[i].len + 8;
    }
    if(len %4 != 0)
    {
    	len = len + 4 - (len % 4);
    }
    return len;
}

/**
* vDelSpace()		: 删除buffer[]内所有的空格
* @param (char* string)	: buffer数组（此处的参数不可为指针）
**/
void vDelSpace(char string[])
{
    uint32 i = 0, iNew = 0;
    for (i = 0, iNew = 0; i < strlen(string); i ++)
    {
        if((string[i] != ' ') && (string[i] != '\t') )
		{
			string[iNew] = tolower(string[i]);
			iNew ++;
		}
    }

    if(iNew > 0)
    {
        string[iNew] = '\0';
    }

    return;
}

/**
* bIsTable()		: 判断是否为table name
* @param (char buffer[]): 字符串参数
**/
bool bIsTable(char buffer[])
{
    char TABLE_NAME_FLAG[] = "tablename:";
    uint32 i = 0;
    if(strlen(buffer) < strlen(TABLE_NAME_FLAG))
    {
        return false;
    }

    for(i = 0; i < strlen(TABLE_NAME_FLAG); i ++)
    {
        if(TABLE_NAME_FLAG[i] != buffer[i])
		{
			return false;
		}
    }

    return true;
}

/**
* vParseTable()		: 解析table的结构
* @param (char *name)	: 存储table的文件名
**/
void vParseTable(char *name, AttributeData* table)
{
    char buffer[FILE_LINE_LEN];					// 文件读取缓冲区
    FILE *fp = NULL;						// 文件指针
    char TABLE_NAME_FLAG[] = "tablename:";			// 标记这个字符后面出现的字符串为“表名”
    uint32 iTableItemNum = 0; 				// 每一个表的第几个列

    if((fp=fopen(name, "r")) == NULL)				// 文件打开失败
    {
    	vmlog(ERROR, "open file(%s) Failed!", name);
    }

    while (fgets(buffer, FILE_LINE_LEN, fp) != NULL) // 循环读取文件按行
	{

		vDelSpace(buffer);
		if ((strlen(buffer) >= 1) && (buffer[0] != '\0')) // 读取内容不为空时
		{
			buffer[strlen(buffer) - 1] = '\0';

			if (bIsTable(buffer)) // 读取表名
			{
				memcpy(table->name, &buffer[strlen(TABLE_NAME_FLAG)],
						strlen(buffer) - strlen(TABLE_NAME_FLAG));
				table->name[strlen(buffer) - strlen(TABLE_NAME_FLAG)] = '\0';
			} else // 按行读取每一列
			{
				int i = 0;
				table->item[iTableItemNum].type = NONE;
				table->item[iTableItemNum].len = 0;

				char* pLineItem = strtok(buffer, ","); // 以","分割每一列的属性，并记录
				while (pLineItem != NULL)
				{
					if (i == 0)
					{
						strcpy(table->item[iTableItemNum].key, pLineItem);
					} else if (i == 1)
					{
						//table->item[iTableItemNum].value = (char*) vmalloc(
						//		sizeof(char) * (strlen(pLineItem) + 1));
						//strcpy(table->item[iTableItemNum].value, pLineItem);
						if(strcmp(pLineItem, "integer") == 0)
						{
						    table->item[iTableItemNum].type = INTEGER;
						}else if(strcmp(pLineItem, "char") == 0)
						{
						    table->item[iTableItemNum].type = CHAR;
						}else if(strcmp(pLineItem, "varchar") == 0)
						{
						    table->item[iTableItemNum].type = VARCHAR;
						}else if(strcmp(pLineItem, "float") == 0)
						{
						    table->item[iTableItemNum].type = FLOAT;
						}else if(strcmp(pLineItem, "long") == 0)
						{
						    table->item[iTableItemNum].type = LONG;
						}else if(strcmp(pLineItem, "date") == 0)
						{
						    table->item[iTableItemNum].type = DATE;
						}else if(strcmp(pLineItem, "double") == 0)
						{
						    table->item[iTableItemNum].type = DOUBLE;
						}else
						{
						    table->item[iTableItemNum].type = NONE;
						}

					} else if (i == 2)
					{
						table->item[iTableItemNum].len = char2Uint(pLineItem);
					}
					pLineItem = strtok(NULL, ",");
					i++;
				}

				iTableItemNum++; // 列记录增加
			}
		}
	}

    table->item_num = iTableItemNum;

    fclose(fp);
    return;
}

/**
* vTablePrint()			: 打印table内容，用于测试
* @param (AttributeData* table)	: table指针
**/
void vTablePrint(AttributeData* table)
{
    if(table == NULL)
    {
    	vmlog(PARSETABLE_LOG, "<parseTable.c>,tips: the table is null!");
    	return;
    }

    vmlog(PARSETABLE_LOG, "table->name:%s;table->items:%u", table->name, table->item_num);
    uint32 i = 0;
    while(i < table->item_num)
    {
        vmlog(PARSETABLE_LOG, "table->item[%d].key:%s", i, table->item[i].key);
        vmlog(PARSETABLE_LOG, "table->item[%d].type:%d", i, table->item[i].type);
        vmlog(PARSETABLE_LOG, "table->item[%d].len:%u",i, table->item[i].len);
        i ++;
    }
}


