/*
 * exectutor.h
 *
 *  Created on: Nov 30, 2017
 *      Author: jyh
 */

#ifndef EXECTUTOR_H_
#define EXECTUTOR_H_
#include "exectutor/execTable.h"
#include "exectutor/execScan.h"
#include "exectutor/execJoin.h"
#include "mparser/analyze.h"

Tuple exec(State state);
Tuple execProjection(ProjectionState projection);
void execSelect(SelectState select);

#endif /* EXECTUTOR_H_ */
