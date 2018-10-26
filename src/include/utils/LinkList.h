/**
* name		: LinkList.h
* date		: 17/10/19
* author	: bsl
* description	: 链表基本操作头文件
**/

#ifndef LINKLIST_H_
#define LINKLIST_H_

#include <stdlib.h>
#include <stdio.h>  
#include <malloc.h>
#include "utils/mlog.h"

#define foreach(node, list) for(node = ((List)list)->next; node != NULL; node = node->next)

typedef void* ListCell;						//定义数据项类型   
typedef struct node
{
    ListCell cell;        					//数据域  
    struct node* next;						//指向下一个节点的指针       
}ListNode, *List;
List mlist;
bool bListEmpty(List* L);
void vListInit(List* L);
void vListInsert(List* L, void* cell);
void vListPrint(List* L);
void vListDestroy(List* L);
void* pListRemove(List* L, void* cell, bool (*)(void*, void*));
void* pListFind(List* L, void* cell, bool (*)(void*, void*));
bool bListSetValue(List* L, void* value, uint index);

#endif
