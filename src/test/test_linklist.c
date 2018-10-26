/**
* name		: test_linklist.c
* date		: 17/10/19
* author	: bsl
* description	: 测试链表的初始化、添加、删除、查找和销毁等操作.
**/

#include "utils/LinkList.h"

/**
* fun()			: 比较 dest 和 orign 内容是否一致
* @param (void* dest)	: List 内指向未知结构的指针
* @param (void* orign)	: 待比较内容的指针
**/
bool fun(void* dest, void* orign)
{   
    if(dest == orign)
    {
        return 1;
    }
    
    return 0;
}


int main()
{
 
    vmlog(LINKLIST_LOG, "------ <test_linklist.c> begining ------");
    int a = 10;
    int b = 20;
    int c = 30;

    List L = NULL;
    vListInit(&L);							// 初始化
 
    vListInsert(&L, &a);						// 插入
    vListInsert(&L, &b);
    vListInsert(&L, &c);
 
    vListPrint(&L);							// 打印
 
    void* r_a = pListFind(&L, &a, fun);					// 查找
    vmlog(LINKLIST_LOG, "before removing: the find result is %p", r_a);
    
    pListRemove(&L, &a, fun);						// 删除指定节点
    r_a = pListFind(&L, &a, fun);
    vmlog(LINKLIST_LOG, "after removing: the find result is %p", r_a);

    vListDestroy(&L);							// 销毁
    vmlog(LINKLIST_LOG, "------- <test_linklist.c> ending ------");

    return 0;
}

