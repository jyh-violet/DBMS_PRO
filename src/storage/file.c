/*
 * file.c
 *
 *  Created on: Oct 16, 2017
 *      Author: jyh
 * */
#include "storage/file.h"

/**
 * CreateFile : by jyh
 * create the file of the given table
 * tid: the oid of the table
 * fid: the oid of the file
 * any file types added, please explain here
 * usually we define: fid = tid * MaxFileNumforRelation 		is the data file
 * 					fid = tid * MaxFileNumforRelation + 1 		is the free space management file
 *
 * */
File CreateFile(char* filename, Oid tid, Oid fid)
{
	vmlog(RELATION_LOG, "--file.c/CreateFile--create file start");
	File file = vmalloc(sizeof(FileData));
	file->tableId = tid;
	file->fd = -1;
	file->fileId = fid;
	memcpy(file->filename, filename, strlen(filename) + 1);
	vmlog(RELATION_LOG, "--file.c/CreateFile--create file end");
	return file;
}

void vDestoryFile(File file)
{
	if(file != NULL)
	{
		if(file->fd > 0)
		{
			File_vClose(file);
		}
		vfree((void**)&file);
	}
}

/**
 * File_vOpen : by jyh
 * openFlag: the parameter for 'open'
 * */
void File_vOpen(File file, int openFlag)
{
	file->fd = open(file->filename, openFlag, S_IRWXU);

	vmlog(FILE_LOG, "--file.c/File_vOpen--fd:%d", file->fd);
}

/**
 * File_vClose : by jyh
 * close the file and reset the fd
 * */
void File_vClose(File file)
{
	vmlog(FILE_LOG, "--file.c/File_vClose--fd:%d, filename:%s", file->fd, file->filename);
	if(file->fd > 0)
	{
		close(file->fd);
	}
	file->fd = -1;
}

ssize_t File_sWrite(File file, void* buffer, uint len)
{
	if(file->fd < 0)
	{
		File_vOpen(file, O_RDWR|O_CREAT);
	}
	ssize_t file_off = lseek(file->fd, 0, SEEK_CUR);
	ssize_t write_size = write(file->fd, buffer, len);
	fsync(file->fd);
	return write_size;
}

ssize_t File_sRead(File file, void* buffer, uint len)
{
	if(file->fd < 0)
	{
		File_vOpen(file, O_RDWR|O_CREAT);
	}
	return read(file->fd, buffer, len);
}

ssize_t File_sSeek(File file, ssize_t offset)
{
	if(file->fd < 0)
	{
		File_vOpen(file, O_RDWR|O_CREAT);
	}

	return lseek(file->fd, offset, SEEK_SET);
}

/**
 * File_vAddBlock : by jyh
 * add a new block for the file.
 * the content of the block will be initialised as 0xFF
 * */
void File_vAddBlock(File file, uint blockid)
{
	vmlog(FILE_LOG, "--file.c/File_vAddBlock--blockid:%u start", blockid);
	if(file->fd < 0)
	{
		File_vOpen(file, O_RDWR|O_CREAT);
	}
	File_sSeek(file, blockid * BLOCKSIZE);
	char buffer[BLOCKSIZE];
	memset(buffer, 0xFF , BLOCKSIZE);
	// initialize the block header
	BlockHeader bhd = (BlockHeader)buffer;
	bhd->bhd_lower = (uint16)sizeof(BlockHeaderData);
	bhd->bhd_upper = (uint16)BLOCKSIZE;
	bhd->bhd_blockHeaderLen = (uint16)sizeof(BlockHeaderData);
	bhd->bhd_offTableLen = 0;
	File_sWrite(file, buffer, BLOCKSIZE);
	vmlog(FILE_LOG, "--file.c/File_vAddBlock--blockid:%u end", blockid);
}

bool File_Exist(File file)
{
	return (access(file->filename, F_OK)==0);
}
