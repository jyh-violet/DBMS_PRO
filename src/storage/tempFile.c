/*
 * tempFile.c
 *
 *  Created on: Dec 1, 2017
 *      Author: jyh
 */

#include "storage/tempFile.h"

TMF_File TMF_CreateFile()
{
	static uint nameid = 0;
	nameid ++;
	char filename[10] = "\0";
	snprintf(filename, 10, "%d.tmp", nameid);
	TMF_File tmf = vmalloc0(sizeof(tempFileData));
	tmf->file = CreateFile(filename, 0, 0);
	tmf->curPos = 0;
	tmf->bufferLen = 0;
	tmf->fileEnd = false;
	tmf->state = TMF_STATE_NOTHING;
	return tmf;
}

void TMF_DestoryFile(TMF_File tmf)
{
	vDestoryFile(tmf->file);
	vfree((void**)(&tmf));
}

ssize_t TMF_BufFlush(TMF_File tmf)
{
	ssize_t len = 0;
	if(tmf->state ==TMF_STATE_WRITE && tmf->curPos > 0)
	{
		len = File_sWrite(tmf->file, tmf->buffer, tmf->curPos);
		tmf->curPos = 0;
	}
	return len;
}

ssize_t TMF_BufRead(TMF_File tmf)
{
	if(tmf->state != TMF_STATE_READ)
	{
		vmlog(ERROR, "TMF_BufRead --- file state error:file state is :%d", tmf->state);
	}
	ssize_t len = File_sRead(tmf->file, tmf->buffer, TMF_BUFFERSIZE);
	tmf->bufferLen = len;
	if(len < TMF_BUFFERSIZE)
	{
		tmf->fileEnd = true;
	}
	return len;
}

ssize_t TMF_Write(TMF_File tmf, void* buffer, uint len)
{
	if(tmf->state != TMF_STATE_WRITE)
	{
		TMF_Reset(tmf);
		tmf->state = TMF_STATE_WRITE;
	}
	uint needlen = len;
	uint pos = 0;
	while(needlen > 0)
	{
		uint writelen = min(TMF_BUFFERSIZE - tmf->curPos, needlen);
		memcpy(tmf->buffer + tmf->curPos, buffer + pos, writelen);
		pos += writelen;
		tmf->curPos += writelen;
		tmf->bufferLen += writelen;
		if(tmf->bufferLen >= TMF_BUFFERSIZE)
		{
			TMF_BufFlush(tmf);
		}
		needlen -= writelen;
	}
	return len;
}

ssize_t TMF_Read(TMF_File tmf, void* buffer, uint len)
{
	if(tmf->state != TMF_STATE_READ)
	{
		TMF_Reset(tmf);
		tmf->state = TMF_STATE_READ;
	}
	uint needlen = len;
	uint pos = 0;
	while(needlen > 0)
	{
		if(tmf->curPos >= tmf->bufferLen)
		{
			TMF_BufRead(tmf);
		}
		uint readlen = min(tmf->bufferLen - tmf->curPos, needlen);
		memcpy(buffer + pos, tmf->buffer + tmf->curPos, readlen);
		pos += readlen;
		tmf->curPos += readlen;
		needlen -= readlen;
	}
	return pos;
}

void TMF_Reset(TMF_File tmf)
{
	TMF_BufFlush(tmf);
	tmf->curPos = 0;
	tmf->bufferLen = 0;
	tmf->fileEnd = false;
	tmf->state = TMF_STATE_NOTHING;
	File_sSeek(tmf->file, 0);
}

