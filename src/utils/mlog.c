/*
 * mlog.c
 *
 *  Created on: Oct 7, 2017
 *      Author: jyh
 *        decs: the functions used to debug or log
 */
#include "utils/mlog.h"

/**
 * mlog : by jyh
 * 	used to print the necessary information to the log file
 * log_level: the switch to decide whether print this log or not
 * */
void vmlog(LOGLEVEL log_level, char* fmat, ...)
{
	if(log_level <= LOG_PRINT)  //there is no need to print this log
	{
		return;
	}
	//get the string passed by the caller through the format string
	va_list argptr;
	va_start(argptr, fmat);
	char buffer[MAX_LOG_SIZE]="\0";
	int count = vsprintf(buffer, fmat, argptr);
	va_end(argptr);
	if(count >= MAX_LOG_SIZE)
	{
		printf("ERROR:log message too many\n");
		return;
	}
	if(log_level == PROMPT)
	{
		printf("%s", buffer);
		printf("\n");
		return;
	}
	int fd = open(LOG_PATH, O_RDWR|O_APPEND|O_CREAT, S_IRWXU  );
	if(fd <= 0)
	{
		printf("ERROR:open the log file errror!!!!\n");
		return;
	}

	// get current time and print
	char time_str[1024]="\0";
	time_t	stamp_time;
	struct tm *tm;
	time(&stamp_time);
	tm = localtime(&stamp_time);
	snprintf(time_str, sizeof(time_str),"%s",asctime (tm));
	write(fd, time_str, strlen(time_str) - 1);
	// get the pid of this process and print
	char pid_str[100]="\0";
	if(log_level == ERROR)
	{
		snprintf(pid_str, 100, "(pid:%d)---ERROR:", (int)getpid());
	}else
	{
		snprintf(pid_str, 100, "(pid:%d)---", (int)getpid());
	}
	write(fd, pid_str, strlen(pid_str));
	// print the string passed by the caller
	write(fd, buffer, count);
	write(fd, "\n", 1);
	if(log_level == ERROR)
	{
		printf("%s", time_str);
		printf("%s", pid_str);
		printf("ERROR:%s", buffer);
		printf("\n");
		print_trace();
		exit(-1);
	}
	close(fd);
	return;
}


void print_trace()
{
	int j, nptrs;
	#define SIZE 100
	void *buffer[100];
	char **strings;
	nptrs = backtrace(buffer, SIZE);
	printf("backtrace() returned %d addresses\n", nptrs);
	/* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
	 *  would produce similar output to the following: */

	strings = backtrace_symbols(buffer, nptrs);
	if (strings == NULL)
	{
		perror("backtrace_symbols");
		exit(EXIT_FAILURE);
	}

	for (j = 0; j < nptrs; j++)
		vmlog(WARNING, "%s", strings[j]);
	vmlog(WARNING, "\n");
	free(strings);

}

