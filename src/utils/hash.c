/*
 * hash.c
 *
 *  Created on: Oct 17, 2017
 *      Author: jyh
 */
#include "utils/hash.h"
HashValue Hash_GetHashValueforOid(Oid oid)
{
	HashValue hvalue = oid % BUFFER_MAX_COUNT;
	return hvalue;
}
HashValue Hash_GetHashDividerforOid(Oid oid)
{
	HashValue hvalue = oid / BUFFER_MAX_COUNT;
	return hvalue;
}
