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


// �ռ����
extern void *allocate(unsigned long n, unsigned a);
extern void *newarray(unsigned long m, unsigned long n, unsigned a);
// �ռ��ͷ�
extern void deallocate(unsigned a);

// �ַ���
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

// ��������š�ȫ�ֱ�����������ֲ�����
enum{CONSTANT=1,LABELS,GLOBAL,PARAM,LOCAL};
// ��ȷָ���˷����ںδ�����
typedef struct coord {
	// file��ָ���˰����ö�����ļ�������
	char *file;
	// yָ���˶�����ֵ��к�
	// xָ���˶������е��ַ���λ��
	unsigned x, y;
}Coordinate;
typedef struct type *Type;
/**
 * type�ṹ�屣���˱������������������ṹ�����Ϻ�ö�ٵ�������Ϣ
 */
struct type {
	int op;
	Type type;
	int align;
	int size;
	union {
		Symbol sym;
		struct {
			unsigned oldstyle : 1;
			Type *proto;
		}f;
	}u;
	Xtype x;
};
typedef struct {
	unsigned printed : 1;
	unsigned marked;
	unsigned short typeno;
	void *xt;
}Xtype;
typedef field *Field;
struct field {
	char *name;
	Type type;
	int offset;
	short bitsize;
	short lsb;
	Field link;
};
typedef struct table* Table;
// ����
typedef struct symbol *Symbol;
struct symbol {
	/** 
	 * name��ͨ���Ƿ��ű�Ĺؼ���.
	 * ���ڱ�ʶ���ͱ�ʾ�������Ĺؼ��֣�
	 * ���򱣴���Դ������ʹ�õ�����;
	 * ���ڱ��������ɵı�ʶ�������ޱ�ǵĽṹ,
	 * name���������������ɵ��ַ���
	 */
	char *name;
	// scope˵�����ڵ�k���������ľֲ���������scope�����LOCAL+K
	int scope;
	// src��ָ���˸÷�����Դ�����ж�����λ��
	Coordinate src;
	/** 
	 * up��ѷ��ű������з���������һ������
	 * ���������ű���Ǹ�����Ϊ���ס�
	 * �Ӻ���ǰ������������Է��ʵ���ǰ����������з���,
	 * ������ΪǶ���ڲ��������еı�ʶ�������������صķ��š�
	 * ���ֹ��������ڱ����˲������������õķ��ű���Ϣ��
	 */
	Symbol up;
	/**
	 * ��ѡ����Ա���ÿ�����ŵ�����ʹ����Ϣ��
	 * �����ѡ����ã�
	 * uses�򱣴�һ��Coordinate�б�˵���˷��ŵ�ʹ�����.
	 * �����ѡ��δ�����ã�usesΪnull
	 */
	List uses;
	/**
	 * ���ŵ���չ�洢���͡�
	 * ������AUTO��REGISTER��STATIC��EXTERN
	 * sclass������ȡֵTYPEDEF��ʾtypedef��ȡֵENUM��ʾö�ٳ���
	 * ���ڳ����ͱ�Ÿ���δ��ʹ�ã���ֵΪ0
	 */
	int sclass;

	unsigned structarg : 1;
	unsigned addressed : 1;
	unsigned computed : 1;
	unsigned temporary : 1;
	unsigned generated : 1;
	unsigned defined : 1;

	Type type;
	float ref;

	union {
		struct {
			int labels;
			Symbol equatedto;
		}l;
		struct {
			unsigned cfields : 1;
			unsigned vfields : 1;
			Table ftab;
			Field flist;
		}s;
	};
	
};
extern void error(const char*, ...);