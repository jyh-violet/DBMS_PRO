/*
 * storage.h
 *
 *  Created on: Oct 18, 2017
 *      Author: jyh
 */

#ifndef STORAGE_H_
#define STORAGE_H_

typedef struct TupleData *Tuple;
typedef struct RelationData *Relation;
typedef struct BlockData *Block;
typedef struct freeSpaceHeap *freeSpaceHeap;
typedef uint32 BlockNumber; //start from 0
typedef uint BufferPage; // the index of the buffer in the manager
typedef struct File *File;

#endif /* STORAGE_H_ */
