/**
* name		: test_swizzmap.c
* date		: 17/10/19
* author	: bsl
* description	: 指针混写测试
**/

#include "SwizzMap.h"

int main()
{
    vmlog(SWIZZMAP_LOG, "------ <SwizzMap.c> begining ------");
    char *pa = "a";
    char *pb = "b";
    char *pc = "c";

    
    int a = 10;
    int b = 20;
    int c = 30;

    SwizzMapData *smd_a = pSwizzMapDataInit(pa, &a);				// 单个记录初始化
    SwizzMapData *smd_b = pSwizzMapDataInit(pb, &b);
    SwizzMapData *smd_c = pSwizzMapDataInit(pc, &c);

    vSwizzMapInit();								// 对应表初始化
    vmlog(SWIZZMAP_LOG, "<SwizzMap.c>,tips: <SwizzMap=%p>", SwizzMap.list);

    vSwizzMapInsert(smd_a);							// 记录插入
    vSwizzMapInsert(smd_b);
    vSwizzMapInsert(smd_c);

    vSwizzMapPrint();								// 对应表打印

    void* r_pa = pSwizzMapFind(pa);						// 根据硬盘地址查询内存地址
    vmlog(SWIZZMAP_LOG, "<SwizzMap.c>,tips: <r_pa=%p> <actual=%p>", r_pa, &a);
    void* r_pb = pSwizzMapFind(pb);
    vmlog(SWIZZMAP_LOG, "<SwizzMap.c>,tips: <r_pb=%p> <actual=%p>", r_pb, &b);

    vSwizzMapDelete(pa);							// 根据硬盘地址删除链表一个节点
    r_pa = pSwizzMapFind(pa);
    vmlog(SWIZZMAP_LOG, "<SwizzMap.c>,tips <after delete>: <r_pa=%p> <actual=%p>", r_pa, &a);

    vSwizzMapDestroy();								// 对应表销毁

    vmlog(SWIZZMAP_LOG, "------ <SwizzMap.c> ending ------");
    return 0;
}


