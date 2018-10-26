/*
 * tempFile.h
 *
 *  Created on: Dec 1, 2017
 *      Author: jyh
 */

#ifndef TEMPFILE_H_
#define TEMPFILE_H_

#include "storage/file.h"
#define TMF_BUFFERSIZE (16*1024)
typedef enum TMF_STATE{
	TMF_STATE_WRITE,
	TMF_STATE_READ,
	TMF_STATE_NOTHING
}TMF_STATE;
typedef struct tempFile{
	File 	file;
	char 	buffer[TMF_BUFFERSIZE];
	uint 	curPos;
	uint 	bufferLen; //the actual length of the content in buffer
	bool 	fileEnd; //the file read end
	TMF_STATE	state;

}tempFileData, *TMF_File;


TMF_File TMF_CreateFile();

void TMF_DestoryFile(TMF_File tmf);
ssize_t TMF_BufFlush(TMF_File tmf);
ssize_t TMF_BufRead(TMF_File tmf);
ssize_t TMF_Write(TMF_File tmf, void* buffer, uint len);
ssize_t TMF_Read(TMF_File tmf, void* buffer, uint len);
void TMF_Reset(TMF_File tmf);


#endif /* TEMPFILE_H_ */
