/*
 * execScan.c
 *
 *  Created on: Nov 28, 2017
 *      Author: jyh
 */


#include "exectutor/execScan.h"


Tuple execScan(ScanState scan)
{
	while(true)
	{
		Tuple tuple =  scan->getnext(scan);
		if(tuple == NULL)
		{
			return NULL; //scan end
		}
		if(checkTuple(scan->state.expr, tuple))
		{
			return tuple;
		}
	}

}
bool checkTuple(Expr expr, Tuple tuple)
{
	if(expr == NULL)
	{
		return true;
	}
	ExprElement result =  Expr_Calculate(expr, tuple);
	if(result.tag != exprelem_bool)
	{
		vmlog(ERROR, "Expr_Calculate---expression error");
	}
	return result.data.logic;
}
Tuple SeqScan_getNext(ScanState scan)
{
	if(!IsValidBlockNumber( scan->relation->Rel_curMaxBlock)) //no conetnt
	{
		return NULL;
	}
	SeqScanState seqScan = (SeqScanState)scan;
	while(true)
	{
		bool needLoad = false;
		if(!IsValidBuffer(seqScan->buffer))
		{
			seqScan->blkNumber = 0;
			seqScan->tupleIndex = 0;
			needLoad = true;
			if(!IsValidBlockNumber(seqScan->scan.relation->Rel_curMaxBlock)) // no data;
			{
				return NULL;
			}
		}else
		{
			BlockHeader bhd = (BlockHeader)BFM_GetBufferData(seqScan->buffer);
			if(seqScan->tupleIndex >= bhd->bhd_offTableLen)
			{
				if(seqScan->blkNumber < seqScan->scan.maxBlockNumbertoScan)
				{
					seqScan->blkNumber ++;
					seqScan->tupleIndex = 0;
					needLoad = true;
					BFM_ClearBufferPin(seqScan->buffer);
			//		seqScan->buffer = INVALID_BUFFER;
				}else // no tuple to be get
				{
					return NULL;
				}
			}
		}
		if(needLoad)
		{
			seqScan->buffer = BLK_LoadBlock(seqScan->blkNumber, seqScan->scan.relation, seqScan->scan.relation->Rel_file);
			BFM_SetBufferPin(seqScan->buffer);
		}
		Block block = BFM_GetBlockofBuffer(seqScan->buffer);
		BlockHeader bhd = BLK_GetBlockHeader(block);
		while(seqScan->tupleIndex <  bhd->bhd_offTableLen)
		{
			BLK_vGetTupleFromBlockwithIndex(block, seqScan->tupleIndex, seqScan->scan.resultTuple);
			seqScan->tupleIndex ++;
			if(!Tup_CheckDeleteFlag(seqScan->scan.resultTuple))
			{
				return seqScan->scan.resultTuple;
			}
		}
	}

}
