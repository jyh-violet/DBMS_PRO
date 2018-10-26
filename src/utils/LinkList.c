/**
* name		: LinkList.c
* date		: 17/10/19
* author	: bsl
* description	: 实现LinkList.h中函数的定义
**/

#include "utils/LinkList.h"

/**
* vListInit()		: 初始化链表操作
* @param (List* L) 	: 链表的头指针地址
**/
void vListInit(List* L)
{
    *L = (List)vmalloc(sizeof(ListNode));
    if(*L == NULL)					//判断链表初始化是否成功(成功:初始化元素; 失败:中断程序执行.)
    {
    	vmlog(ERROR, "<LinkList.c>,error: vListInit() Failed!");
    }

    (*L)->cell = NULL;
    (*L)->next = NULL;
    
    return;
}

/**
* vListInsert()		: 向链表内添加元素
* @param (List* L)	: 链表头指针
* @param (void* cell)	: 待存储数据地址
**/
void vListInsert(List* L, void* cell)
{
    if(*L == NULL)
    {
        vmlog(WARNING, "<LinkList.c>,error: vListInsert() Failed!(List Is Null)");
        vListInit(L);
    }
    
    List p = *L;
    List r = NULL;

    r = (List)vmalloc(sizeof(Node));
    r->next = p->next;
    r->cell = cell;

    p->next = r;
    return;
}

/**
* vListPrint()		: 打印链表内容
* @param (List* L)	: 链表头指针
**/
void vListPrint(List* L)
{
    if(*L == NULL)
    {
        vmlog(LINKLIST_LOG, "<LinkList.c>,tips: List Print Failed!(List Is Null)");
	return;
    }

    List p = *L;
    List q = p->next; 
    uint index = 0;

    while(q != NULL)
    {
    	vmlog(LINKLIST_LOG, "<LinkList.c>,tips: <index=%u> <cell=%p>", index, (q->cell));
    	p = q;
	q = p->next;
	index ++;
    }
}

/**
* void vListDestroy()	: 销毁链表
* @param (List* L)	: 链表头指针
**/
void vListDestroy(List* L)
{
    if(*L == NULL)
    {
    	return;
    }

    List p = *L;
    List q = p->next;
    
    while( q != NULL)
    {
    	vmlog(MALLOC_TRACE, "vListDestroy-- p->cell:%p", p->cell);
    	vfree((void*)&(p->cell));
    	vfree((void**)&p);
        p = q;
        q = p->next;
    }
    vfree((void*)&(p->cell));
    vfree((void**)&p);
    *L = NULL;
    return;
}

/**
* pListFind()			: 链表内查找元素
* @param (List* L)		: 链表头指针
* @param (void* cell)		: 数据指针
* @Param (*)(void*,void*)	: 函数指针
**/
void* pListFind(List* L, void* cell, bool (*function)(void*,void*))
{
    void* result = NULL;
    if(*L == NULL)
    {
    	return result;
    }

    List p = *L;
    List q = p->next;
   
    while(q != NULL)
    {
    	if((*function)(q->cell, cell) == true)
	{
	    result = q->cell;
	    break;
	}
    	p = q;
	q = p->next;
    }

    return result;
}

/**
* pListRemove()			: 删除一个节点
* @param (List* L)		: 链表头指针
* @Param (void* cell)		: 数据指针
* @param (*)(void*,void*)	: 函数指针
**/
void* pListRemove(List* L, void* cell, bool (*function)(void*, void*))
{
	vmlog(LINKLIST_LOG, "--LinkList.c/pListRemove--");
    void* result = NULL;

    if(*L == NULL)
    {
    	return result;
    }

    List p = *L;
    List q = p->next;
    while(q != NULL)
    {
    	if((*function)(q->cell, cell) == true)
    	{
    		p->next = q->next;
    		result = q->cell;
    		vfree((void**) &q);
    		break;
    	}
    	p = q;
    	q = p->next;
    }

    return result;
}

/**
* bListEmpty()		: 判断链表是否为空
* @param (List* L)	: 链表头指针
**/
bool bListEmpty(List* L)
{
    if(*L == NULL)
    {
        return true;
    }

    if((*L)->next == NULL)
    {
        return true;
    }

    return false;
}

bool bListSetValue(List* L, void* value, uint index)
{
	uint i = 0;
	if(*L == NULL)
	{
		return false;
	}

	List p = *L;
	List q = p->next;
	while((q != NULL) && (i < index))
	{
		p = q;
		q = p->next;
		i ++;
	}
	if(q != NULL)
	{
		vfree((void*)&(p->cell));
		p->cell = value;
		return true;
	}else
	{
		return false;
	}


}

