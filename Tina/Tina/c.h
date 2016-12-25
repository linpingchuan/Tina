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

extern void *allocate(unsigned long n, unsigned a);
extern void deallocate(unsigned a);
extern void *newarray(unsigned long m, unsigned long n, unsigned a);


extern char *string(const char *str);
extern char *stringn(const char *str, int len);
extern char *stringd(long n);

enum{PERM=0,FUNC,STMT};

extern void error(const char*, ...);