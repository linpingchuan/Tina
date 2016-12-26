#pragma once
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

// 宏NEW返回一个指向尚未初始化的空间指针
#define NEW(p,a) ((p)=allocate(sizeof*(p),(a)))

// 宏NEW0在分配空间后，通过C语言的memset将其初始化为 0
// memset将其第一个参数作为函数结果返回
#define NEW0(p,a) memset(NEW(((p),(a)),0,sizeof *(p)))

#undef roundup
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

#define NELEMS(a) ((int)(sizeof(a)/sizeof((a)[0])))


// 空间分配
extern void *allocate(unsigned long n, unsigned a);
extern void *newarray(unsigned long m, unsigned long n, unsigned a);
// 空间释放
extern void deallocate(unsigned a);

// 字符串
extern char *string(const char *str);
extern char *stringn(const char *str, int len);
extern char *stringd(long n);

enum{PERM=0,FUNC,STMT};


typedef struct _list *List;
struct _list {
	void *x;
	List link;
};

extern List append(void *x, List plist);
extern int length(List plist);
extern void *ltov(List *plist, unsigned a);

extern void error(const char*, ...);