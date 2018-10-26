/*
 * execJoin.h
 *
 *  Created on: Dec 1, 2017
 *      Author: jyh
 */

#ifndef EXECJOIN_H_
#define EXECJOIN_H_

#include "storage/relation.h"
#include "storage/tempFile.h"
#include "mparser/analyze.h"
Tuple execJoin(JoinState join);
Tuple NestLoopJoin_getNext(JoinState join);

#endif /* EXECJOIN_H_ */
