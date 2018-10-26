/*
 * execTable.h
 *
 *  Created on: Nov 25, 2017
 *      Author: jyh and bsl
 */

#include "basic.h"
#include "storage/relation.h"
#include "server/sysinit.h"
#include "mparser/analyze.h"

void execCreateIndex(CreateIndexState state);
void execCreateTable(CreateTableState state);
void execDropTable(DropTableState state);
void execDropIndex(DropIndexState state);
bool execInsertTable(InsertTableState state);
bool execDeleteTable(DeleteTableState state);
