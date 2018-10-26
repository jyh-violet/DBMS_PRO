/*
 * file.h
 *
 *  Created on: Oct 16, 2017
 *      Author: jyh
 */

#ifndef FILE_H_
#define FILE_H_

#include "basic.h"
#include "utils/mlog.h"
#include "storage/storage.h"
#include "storage/relation.h"
#include "storage/buffer.h"
#include "storage/block.h"
#include "storage/tuple.h"

#define FILE_PATH_MAX_LEN 1024

typedef struct File{
	int fd;
	char filename[FILE_PATH_MAX_LEN];
	Oid tableId;
	Oid fileId;
	M_type fileType;
}FileData, *File;

File CreateFile(char* filename, Oid tid, Oid fid);
void vDestoryFile(File file);
void File_vOpen(File file, int openFlag);
void File_vClose(File file);
ssize_t File_sWrite(File file, void* buffer, uint len);
ssize_t File_sRead(File file, void* buffer, uint len);
ssize_t File_sSeek(File file, ssize_t offset);
void File_vAddBlock(File file, uint blockid);
bool File_Exist(File file);
#endif /* FILE_H_ */
