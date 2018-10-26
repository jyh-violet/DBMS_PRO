/*
 * dateType.c
 *
 *  Created on: Nov 26, 2017
 *      Author: jyh
 */

#include "storage/dataType.h"



char monthBox[12][4]={"Jan\0","Feb\0","Mar\0","Apr\0","May\0","Jun\0","Jul\0","Aug\0","Sep\0","Oct\0","Nov\0","Dec\0"};



unsigned char getKindNumber(char* string)//分析日期字符串，获知时间格式
{
    char *p=NULL,*q=NULL;
    unsigned int i=0;
    p=strchr(string,'-');//检查是否含有-连接符
    if(p)//若有，则为2 3 4中的一种
    {
        for(i=0;i<12;i++)
        {
            q=strstr(string,monthBox[i]);//检查月份的书写方式，q为月份单词出现的位置
            if(q)break;
        }
        if(i==12)return 2;//月份未用单词表示，为2
        else//月份用单词表示
        {
            if(q==string)return 4;//月份单词出现在字符串首部，为MDY模式，为4
            else return 3;//月份单词出现在字符串中间，为YMD模式，为3
        }
    }
    else//若无-连接符
    {
        q=strchr(string,'/');//检查是否有/连接符
        if(q)return 1;//若有，则为1
        else return 0;//若无，则为0
    }
}
int checkYMD(char* year,char* month,char *date)//检查并处理年、月、日字符串
{
    unsigned int i=0,j=0,monthFlag=0,len=0;
    len=strlen(year);
    for(i=0;i<len;i++)
    {
        if((year[i]>'9')||(year[i]<'0'))return -1;//检查年份字符串是否全是数字字符
    }
    len=strlen(month);
    for(i=0;i<len;i++)
    {
        if((month[i]>'9')||(month[i]<'0'))//检查月份字符串是否全是数字字符
        {
            monthFlag=1;
            break;
        }
    }
    if(monthFlag)//若月份字符串中有非数字字符，再检查是否是由英文表示的月份
    {
        for(j=0;j<12;j++)
        {
            if(strcmp(month,monthBox[j])==0)break;
        }
        if(j==12)return -1;//若不是，则格式错误
        else//若是，则将用英文表示的月份转换为用数字表示的月份
        {
            if(j>=10)month[0]='1';
            else month[0]='0';
            month[1]=((j+1)%10)+'0';
        }
    }
    else
    {
            if(month[1]=='\0')//将一位数字的月份字符串转换为两位
            {
                month[1]=month[0];
                month[0]='0';
             }
    }
    len=strlen(date);
    for(i=0;i<len;i++)
    {
        if((date[i]>'9')||(date[i]<'0'))return -1;//检查日期字符串是否全是数字字符
    }
    if(date[1]=='\0')//将一位数字的日期字符串转换为两位
    {
        date[1]=date[0];
        date[0]='0';
    }
    if(month[0]>'1')return -1;//限制月份的第一位
    if((month[0]=='1')&&(month[1]>'2'))return -1;//以1开头的月份只有10、11、12
    if(date[0]>'3')return -1;//限制日期的第一位
    if((date[0]=='3')&&(date[1]>'1'))return -1;//以3开头的日期只有30、31
    if((month[0]=='0')&&(month[1]=='2')&&(date[0]>'2'))return -1;//2月最多只有29天
    if((month[1]=='4')&&(date[0]=='3')&&(date[1]>'0'))return -1;//4月只有30天
    if((month[1]=='6')&&(date[0]=='3')&&(date[1]>'0'))return -1;//6月只有30天
    if((month[1]=='9')&&(date[0]=='3')&&(date[1]>'0'))return -1;//9月只有30天
    if((month[0]=='1')&&(month[1]=='1')&&(date[0]=='3')&&(date[1]>'0'))return -1;//11月只有30天
    return 0;
}
int processDateString(pDate_type* date_instance, char* sourceData)//根据时间格式处理日期字符串，使其转换为标准格式
{
    unsigned int len=0,i=0,j=0;
    char *dst=NULL,
    		*src=NULL,
    		dateTemp[10] ,
    		monthTemp[10],
    		yearTemp[10],
    		*p=NULL;
    uint kind_number=getKindNumber(sourceData);
    memset(dateTemp,'\0',10);
    memset(monthTemp,'\0',10);
    memset(yearTemp,'\0',10);
    int res = 0;
    if(kind_number==0)//like 19991223 YMD
    {
        len=strlen(sourceData);
        if(len!=8)return -1;
        memcpy(yearTemp,sourceData,4);
        //yearTemp[4]='\0';
        memcpy(monthTemp,sourceData + 4,2);
        //monthTemp[2]='\0';
        memcpy(dateTemp,sourceData + 6,2);
        //dateTemp[2]='\0';
        res = checkYMD(yearTemp,monthTemp,dateTemp);
    }
    if(kind_number==1)//like 1/8/1999 MDY
    {
        len=strlen(sourceData);
        p=sourceData;
        for(i=0;i<len;i++)
        {
            if(p[i]=='/')break;
        }
        src=p;
        dst=monthTemp;
        memcpy(dst,src,i);
        //monthTemp[i]='0';
        for(j=i+1;j<len;j++)
        {
            if(p[j]=='/')break;
        }
        src=p+i+1;
        dst=dateTemp;
        memcpy(dst,src,j-i-1);
        //dateTemp[j-i-1]='0';
        src=p+j+1;
        dst=yearTemp;
        memcpy(dst,src,len-i-j);
        //yearTemp[len-i-j]='\0';
        res = checkYMD(yearTemp,monthTemp,dateTemp);
    }
    if(kind_number==2)//like 1999-01-08 YMD
    {
        len=strlen(sourceData);
        if(len!=10)return -1;
        memcpy(yearTemp,sourceData ,4);
        //yearTemp[4]='\0';
        memcpy(monthTemp,sourceData + 5,2);
        //monthTemp[2]='\0';
        memcpy(dateTemp,sourceData + 8,2);
        //dateTemp[2]='\0';
        res = checkYMD(yearTemp,monthTemp,dateTemp);
    }
    if(kind_number==3)//like 1999-Jan-08 YMD
    {
        len=strlen(sourceData);
        if(len!=11)return -1;
        memcpy(yearTemp,sourceData,4);
        //yearTemp[4]='\0';
        memcpy(monthTemp,sourceData + 5,3);
        //monthTemp[3]='\0';
        memcpy(dateTemp,sourceData + 9,2);
        //dateTemp[2]='\0';
        res = checkYMD(yearTemp,monthTemp,dateTemp);
    }
    if(kind_number==4)//like Jan-08-1999 MDY
    {
        len=strlen(sourceData);
        if(len!=11)return -1;
        memcpy(monthTemp,sourceData,3);
        //monthTemp[3]='\0';
        memcpy(dateTemp,sourceData + 4,2);
        //dateTemp[2]='\0';
        memcpy(yearTemp,sourceData + 7,4);
        //yearTemp[4]='\0';
        res = checkYMD(yearTemp,monthTemp,dateTemp);
    }
    date_instance->date = char2Int(yearTemp) * YEAR_BASE
    		+ char2Int(monthTemp) * MONTH_BASE
    		+ char2Int(dateTemp) * DAY_BASE;
// /   memcpy(date_instance->date + YEAR_OFF, yearTemp,YEAR_SIZE);
//    memcpy(date_instance->date + MONTH_OFF, monthTemp, MONTH_SIZE);
//    memcpy(date_instance->date + DAY_OFF, dateTemp, DAY_SIZE );
//    date_instance->date[YMD_SIZE] = '\0';
    return res;
}
pDate_type* iniTypeDate(char* Data)//初始化date类型：Data是需要写入的长整型数据，返回Date_type结构体指针,返回0则空间分配失败或时间格式不支持
{
    pDate_type *date_instance=NULL;
    unsigned int len=0;
    char *dst=NULL,*src=NULL;
    date_instance=(pDate_type*)vmalloc0(sizeof(pDate_type));
    if(processDateString(date_instance, Data)==0)return date_instance;//时间格式正确
    return 0;//时间格式不正确
}

void destroyTypeDate(pDate_type* date_instance)
{
//	vfree((void**)&date_instance);
}

int dateMem2Disk(pDate_type* date_instance, char* buffer)//date类型：将内存中的结构序列化，写入buffer，返回0表示成功
{
    char *dst=NULL,*src=NULL;
    memset(buffer,'\0',YMD_SIZE+1);
    memcpy(buffer,&(date_instance->date),sizeof(date_instance->date));
//    vfree((void**)&(date_instance->data));
//    vfree((void**)&date_instance);
    return sizeof(date_instance->date);
}
void dateDisk2Mem(pDate_type *date_instance,char* buffer,uint32 *number)//date类型：从buffer中获得数据，转换为Date_type结构体，返回Date_type结构体指针,返回0则空间分配失败，参数number用于返回写入的字节数
{
    char *dst=NULL,*src=NULL;
    *number=sizeof(date_instance->date);;
	memcpy(&(date_instance->date),buffer,*number);
//	date_instance->date[YMD_SIZE] = '\0';

}

int dateCmp(pDate_type* a, pDate_type* b)
{
//	int res = strcmp(a->date, b->date);
	int res = (a->date - b ->date);
	return res;

}

bool setTypeDateData(pDate_type* date_instance, char* data)
{
	 if(processDateString(date_instance, data)==0)
		 	return true;//时间格式正确
	 return false;
}
