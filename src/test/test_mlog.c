/*
 * test_main.c
 *
 *  Created on: Oct 8, 2017
 *      Author: jyh
 *        desc: the test file for the funciotns in mlog.c
 */
#include "utils/mlog.h"
int main()
{
	vmlog(BASIC_LOG, "test for mlog");
	return 0;
}
