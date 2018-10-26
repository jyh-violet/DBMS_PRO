/*
 * vacuum.c
 *
 *  Created on: Oct 24, 2017
 *      Author: jyh
 *     compile: gcc -std=c99 -Isrc/include src/utils/* src/storage/* src/parser/* src/vacuum/vacuum.c -o vacuum
 */
#include "storage/relation.h"

void VAC_vArrangeRelationData(Relation rel);
int main()
{
	Oid tid = (Oid) 2;
	Relation relation = Rel_CreateRelation("test", tid);
	FSM_printHeap(relation->Rel_fsh);
	VAC_vArrangeRelationData(relation);
	Rel_vDestoryRelation(relation);
}

void VAC_vArrangeRelationData(Relation rel)
{
	vmlog(VACUUM_LOG, "--vacuum.c/VAC_vArrangeRelationData--start");
	char buffer[BLOCKSIZE]; //use as the buffer to load a block of data
	Block block = BLK_CreateBlock(INValidBlockNumber, rel, rel->Rel_file);
	block->blk_buffer = buffer;
	FSM_clearHeap(rel->Rel_fsh);
	for(uint bnum = 0; bnum < rel->Rel_curMaxBlock; bnum ++)
	{
		block->blk_bnumber = bnum;
		BLK_sReadBlock(block);
		BlockHeader bhd = BLK_GetBlockHeader(block);
		uint handleindex = BLOCKSIZE - 1;
		for(uint indexInBlock = 0; indexInBlock < bhd->bhd_offTableLen; indexInBlock ++)
		{
			// tuple is invalid and ignore it
			if(BD_CheckItemInValid(bhd, indexInBlock))
			{
				BD_SetItemOffsetZero(bhd, indexInBlock);
				continue;
			}
			uint tupleLen = BD_GetItemLen(bhd, indexInBlock);
			uint tupleOff = BD_GetItemOffset(bhd, indexInBlock);
			for(uint j = 0; j < tupleLen; j ++)
			{
				block->blk_buffer[handleindex] = block->blk_buffer[tupleOff + tupleLen - 1 - j];
				handleindex --;
			}
			BD_SetItemOffset(bhd, indexInBlock, handleindex + 1);
		}
		bhd->bhd_upper = handleindex + 1;
		BLK_sWriteBlock(block);
		freeSpaceItem fsitem = {(uint16)bhd->bhd_upper - (uint16)bhd->bhd_lower, bnum};
		FSM_vAddNodeToHeap(rel->Rel_fsh, fsitem);

	}
	BLK_vDestoryBlock(block);
	vmlog(VACUUM_LOG, "--vacuum.c/VAC_vArrangeRelationData--end");

}
