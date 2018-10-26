/*
 * execScan.h
 *
 *  Created on: Nov 28, 2017
 *      Author: jyh
 */

#ifndef EXECSCAN_H_
#define EXECSCAN_H_

#include "storage/relation.h"
#include "mparser/analyze.h"
Tuple execScan(ScanState scan);
bool checkTuple(Expr expr, Tuple tuple);
Tuple SeqScan_getNext(ScanState scan);

#endif /* EXECSCAN_H_ */
