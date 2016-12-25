#pragma once
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

// ��NEW����һ��ָ����δ��ʼ���Ŀռ�ָ��
#define NEW(p,a) ((p)=allocate(sizeof*(p),(a)))

// ��NEW0�ڷ���ռ��ͨ��C���Ե�memset�����ʼ��Ϊ 0
// memset�����һ��������Ϊ�����������
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