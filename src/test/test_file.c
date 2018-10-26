/*
 * test_file.c
 *
 *  Created on: Oct 16, 2017
 *      Author: jyh
 *      desc: compile: gcc -std=c99 -Isrc/include src/utils/* src/storage/* src/test/test_file.c -o test_file
 *
 */

#include "storage/file.h"
int main()
{
	File file = CreateFile("test", 1, 1);
	File_vAddBlock(file, 0);
	struct stat statbuff;
	uint filesize = 0;
	if(stat(file->filename, &statbuff) < 0)
	{
		vmlog(FILE_LOG, "ERROR:getfile stat error:%s", file->filename);
	}else
	{
		filesize = statbuff.st_size;
	}
	vmlog(FILE_LOG, "file:%s, size:%u", file->filename, filesize);
	vDestoryFile(file);
	return 0;
}
