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