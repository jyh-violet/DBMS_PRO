/*
 * hash.h
 *
 *  Created on: Oct 17, 2017
 *      Author: jyh
 */

#ifndef HASH_H_
#define HASH_H_
#include "utils/mlog.h"
#include "storage/block.h"
#include "storage/buffer.h"
typedef uint HashValue;
HashValue Hash_GetHashValueforOid(Oid oid);
HashValue Hash_GetHashDividerforOid(Oid oid);
#endif /* HASH_H_ */
