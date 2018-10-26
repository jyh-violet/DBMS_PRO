/*
 * index.h
 *
 *  Created on: Dec 9, 2017
 *      Author: jyh
 */

#ifndef INDEX_H_
#define INDEX_H_

#define INDEXCHAR_MAXLEN (8)


typedef union ItemIndexdata{
	int32	int_type;
	int64	long_type;
	char	char_type[8];
	float	float_type;
	double	double_type;
	uint32	date_type;
}ItemIndexdata;
typedef struct Index{
	ItemIndexdata data;
	union {
		BlockNumber childBlock;
		TupleAddr	tupAddr
	}pointer;

}IndexData , *Index;

typedef struct BPTreeBlock{
	BlockNumber	parent;
	BlockNumber left;//left brother;
	BlockNumber right;  //right brother;
	bool		idLeaf;
	uint32		nowNumber; // the number of indexes in this block
	IndexData 	indexarray[1]; // the block in indexarray[0].chlidBlock is data >=indexarray[0].data and data<indexarray[1].data
								//if indexarray[0].data = indexarray[1].data then ndexarray[1].childblock may have this data either.
}BPTreeBlockData, *BPTreeBlock;


#endif /* INDEX_H_ */
