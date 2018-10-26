/*
 * test_lex.c
 *
 *  Created on: Nov 30, 2017
 *      Author: jyh
 *     Compile:
 *     gcc -g -w src/lex/lex.yy.c src/lex/grammer.tab.c src/test/test_lex.c -o test_lex src/storage/*.c src/utils/*.c src/parser/*.c  src/server/*.c src/exectutor/*.c  src/dataType/*.c -I src/include/
 */

#include "grammer.h"
#include "mparser/analyze.h"
#include "server/sysinit.h"

void main()
{
	initSysTable();
	m_parse();
//	vPrintSql(sqlManager);
	vmlog(GRAMMER_LOG, "parse end !! analyze start!");
	State state = analyze_gramTree(sqlManager);
	exec(state);
	vmlog(PROMPT, "execute end! print the system table~~~~~~~~~~~~~~~~");
	vmlog(GRAMMER_LOG, "start scan the systable");
	Rel_vScanTable(sysTable);
	vmlog(DISP_LOG, "start scan the  sysAttr");
	Rel_vScanTable(sysAttr);
	destorySysTable();
	return;
}
