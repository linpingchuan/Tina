#ifndef C_H
#define C_H
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "config.h"

// ��NEW����һ��ָ����δ��ʼ���Ŀռ�ָ��
#define NEW(p,a) ((p)=allocate(sizeof*(p),(a)))

// ��NEW0�ڷ���ռ��ͨ��C���Ե�memset�����ʼ��Ϊ 0
// memset�����һ��������Ϊ�����������
#define NEW0(p,a) memset(NEW(((p),(a)),0,sizeof *(p)))

#undef roundup
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

#define NELEMS(a) ((int)(sizeof(a)/sizeof((a)[0])))

typedef struct node *Node;
// ָ��ѭ���б�
typedef struct _list *List;
typedef struct type *Type;
// ����
typedef struct symbol *Symbol;
typedef struct field *Field;

typedef struct _table *Table;

// ��������š�ȫ�ֱ�����������ֲ�����
enum { CONSTANTS = 1, LABELS, GLOBAL, PARAM, LOCAL };
// ��ȷָ���˷����ںδ�����
typedef struct coord {
	// file��ָ���˰����ö�����ļ�������
	char *file;
	// yָ���˶�����ֵ��к�
	// xָ���˶������е��ַ���λ��
	unsigned x, y;
}Coordinate;

typedef struct {
	unsigned listed : 1;
	unsigned registered : 1;
	unsigned emitted : 1;
	unsigned copy : 1;
	unsigned equatable : 1;
	unsigned spills : 1;
	unsigned mayrecalc : 1;
	void *state;
	short inst;
	Node kids[3];
	Node prev, next;
	short argno;
}Xnode;

typedef struct {
	unsigned printed : 1;
	unsigned marked;
	unsigned short typeno;
	void *xt;
}Xtype;
typedef struct metrics {
	unsigned char size, align, outoflines;
}Metrics;

typedef struct interface {
	
	void(*defsymbol)(Symbol);
}Interface;

typedef struct {
	Symbol vbl;
	short set;
	short number;
	unsigned mask;
}*Regnode;

typedef struct {
	char *name;
	unsigned int eaddr;
	int offset;
	Node lastuse;
	int usecount;
	Regnode regnode;
	Symbol *wildcard;
}Xsymbol;

typedef union value {
	long i;
	unsigned long u;
	long double d;
	void *p;
	void(*g)(void);
}Value;

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

struct node {
	short op;
	short count;
	Symbol syms[3];
	Node kids[2];
	Node link;
	Xnode x;
};
struct _list {
	void *x;
	List link;
};

extern List append(void *x, List plist);
extern int length(List plist);
extern void *ltov(List *plist, unsigned a);
// rmtypes�������ͻ�����ɾ��
extern void rmtypes(int);
extern void error(const char*, ...);
// ���ű�ı�ʾ
extern Table table(Table tp, int level);
extern void foreach(Table tp, int level, void(*apply)(Symbol, void *), void *cl);
// ������ĸı�
extern void enterscope();
extern void exitscope();
// ���Ҹ�������ʶ��
extern Symbol install(char *name, Table *tpp, int level, int arena);
extern Symbol lookup(const char*name, Table tp);
// ���
extern int genlabel(int);
extern Symbol findlabel(int);

/**
* type�ṹ�屣���˱������������������ṹ�����Ϻ�ö�ٵ�������Ϣ
*/
struct _type {
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

struct field {
	char *name;
	Type type;
	int offset;
	short bitsize;
	short lsb;
	Field link;
};

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
		/**
		 * ��������������ڲ����ָ����ͬλ�ã�
		 * ����Щ��ŵ�equatedto��ָ������һ����š�
		 */
		struct {
			int label;
			Symbol equatedto;
		}l;
		struct {
			unsigned cfields : 1;
			unsigned vfields : 1;
			Table ftab;
			Field flist;
		}s;
		int value;
		Symbol *idlist;
		struct {
			Value min, max;
		}limits;
		struct {
			Value v;
			Symbol loc;
		}c;
		struct {
			Coordinate pt;
			int label;
			int ncalls;
			Symbol *callee;
		}f;
		int seg;
		Symbol alias;
		struct {
			Node cse;
			int replace;
			Symbol next;
		}t;
	}u;
	Xsymbol x;
};

// ���ű��Ӽ�
extern Table constants;
// �������Ϊextern�ı�ʶ��,���ھ����ⲿ��ʶ��������ͻ
extern Table externals;
// identifiers���һ���֣���������ļ�������ı�ʶ��
extern Table globals;
// ����һ���ʶ��
extern Table identifiers;
// ���������������ڲ����
extern Table labels;
// ������ͱ��
extern Table types;
// src�б������Ϣʹ��apply��������ϳ����ܹ�ָ����ȷ��Դ��������
extern Coordinate src;
// ȫ�ֱ���level��ֵ�Ͷ�Ӧ�ı�һ���ʾ��һ��������
extern int level;
// ���������
extern Interface *IR;
#endif