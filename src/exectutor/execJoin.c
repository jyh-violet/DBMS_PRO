/*
 * execJoin.c
 *
 *  Created on: Dec 1, 2017
 *      Author: jyh
 */
#include "exectutor/exectutor.h"

void initNestLoopInner(NestLoopJoinState nestloop);
void tupleJoin(Tuple result, Tuple tup1, Tuple tup2);
Tuple NestLoop_GetOuter(NestLoopJoinState nestloop);


Tuple execJoin(JoinState join)
{
	while(true)
	{
		Tuple tuple =  join->getnext(join);
		if(tuple == NULL)
		{
			return NULL; //scan end
		}
		if(checkTuple(join->state.expr, tuple))
		{
			return tuple;
		}
	}
}

Tuple NestLoopJoin_getNext(JoinState join)
{
	NestLoopJoinState nestloop = (NestLoopJoinState)join;
	if(nestloop->outerTuple == NULL) // the first time
	{
		nestloop->outerTuple = NestLoop_GetOuter(nestloop);
		if(nestloop->outerTuple == NULL) // no outer tuple
		{
			return NULL;
		}
		initNestLoopInner(nestloop);
	}
	if(nestloop->innerPos >= nestloop->innerSize) //an outer tuple join end, get another one
	{
		nestloop->outerTuple = NestLoop_GetOuter(nestloop);
		if(nestloop->outerTuple == NULL) // an innner array loop end ,get another array
		{
			if(nestloop->isLastLoop) // no more inner, join end
			{
				return NULL;
			}else
			{
				initNestLoopInner(nestloop);
			}
		}
		nestloop->innerPos = 0;
	}
	tupleJoin(nestloop->join.resultTuple, nestloop->innnerArray[nestloop->innerPos], nestloop->outerTuple);
	nestloop->innerPos ++;
	return nestloop->join.resultTuple;
}

void initNestLoopInner(NestLoopJoinState nestloop)
{
	uint i = 0;
	for(i = 0; i < nestloop->innerArraysize; i++)
	{
		Tup_vClearTuple(nestloop->innnerArray[i]);
		if(nestloop->innnerArray[i]->TD_memtuple == NULL)
		{
			Tup_vAllocateMemtuple(nestloop->innnerArray[i]);
		}
		Tuple tuple = exec(nestloop->join.state.child[0]);
		if(tuple == NULL)  //no more inner tuple
		{
			nestloop->isLastLoop = true;
			break;
		}
		if(!tuple->TD_memtuple_exist)
		{
			Tup_vGetMemTupleFromHeaptup(tuple);

		}
		memcpy(nestloop->innnerArray[i]->TD_memtuple, tuple->TD_memtuple, tuple->TD_memtuple_len);
		nestloop->innnerArray[i]->TD_memtuple_len = tuple->TD_memtuple_len;
		nestloop->innnerArray[i]->TD_memtuple_exist = true;
	}
	nestloop->innerSize = i;
	nestloop->innerPos = 0;
}

void tupleJoin(Tuple result, Tuple tup1, Tuple tup2)
{
	Tup_vClearTuple(result);
	if(result->TD_memtuple == NULL)
	{
		Tup_vAllocateMemtuple(result);
	}
	if(!tup1->TD_memtuple_exist)
	{
		Tup_vGetMemTupleFromHeaptup(tup1);

	}
	if(!tup2->TD_memtuple_exist)
	{
		Tup_vGetMemTupleFromHeaptup(tup2);

	}
	memcpy(result->TD_memtuple, tup1->TD_memtuple, tup1->TD_memtuple_len);
	memcpy(result->TD_memtuple + tup1->TD_memtuple_len, tup2->TD_memtuple, tup2->TD_memtuple_len);
	result->TD_memtuple_len = tup1->TD_memtuple_len + tup2->TD_memtuple_len;
	result->TD_memtuple_exist = true;
}

Tuple NestLoop_GetOuter(NestLoopJoinState nestloop)
{
	Tuple tuple;
	if(nestloop->isFirstLoop)
	{
		tuple = exec(nestloop->join.state.child[1]);
		if(tuple == NULL) // get outer end;
		{
			nestloop->isFirstLoop = false;
			nestloop->outerTuple = Tup_CreateTuple(nestloop->join.outerRel);
			return NULL;
		}
		TMF_Write(nestloop->tmf, &(tuple->TD_memtuple_len), sizeof(uint));
		TMF_Write(nestloop->tmf, tuple->TD_memtuple, tuple->TD_memtuple_len);
		nestloop->outerTuple = tuple;
	}else
	{
		tuple = nestloop->outerTuple;
		ssize_t size  = TMF_Read(nestloop->tmf, &(tuple->TD_memtuple_len), sizeof(uint));
		if(size < sizeof(uint))
		{
			return NULL; // read end
		}
		size = TMF_Read(nestloop->tmf, tuple->TD_memtuple, tuple->TD_memtuple_len);
		if(size < tuple->TD_memtuple_len)
		{
			return NULL; // read end
		}
	}
	return tuple;
}
