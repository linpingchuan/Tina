#ifndef C_H
#define C_H
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "config.h"

// 宏NEW返回一个指向尚未初始化的空间指针
#define NEW(p,a) ((p)=allocate(sizeof*(p),(a)))

// 宏NEW0在分配空间后，通过C语言的memset将其初始化为 0
// memset将其第一个参数作为函数结果返回
#define NEW0(p,a) memset(NEW(((p),(a)),0,sizeof *(p)))

#undef roundup
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

#define NELEMS(a) ((int)(sizeof(a)/sizeof((a)[0])))

typedef struct node *Node;
// 指针循环列表
typedef struct _list *List;
typedef struct type *Type;
// 符号
typedef struct symbol *Symbol;
typedef struct field *Field;

typedef struct _table *Table;

// 常量、标号、全局变量、参数或局部变量
enum { CONSTANTS = 1, LABELS, GLOBAL, PARAM, LOCAL };
// 精确指明了符号在何处定义
typedef struct coord {
	// file域指明了包含该定义的文件的名字
	char *file;
	// y指明了定义出现的行号
	// x指明了定义行中的字符的位置
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
// rmtypes将其类型缓冲中删除
extern void rmtypes(int);
extern void error(const char*, ...);
// 符号表的表示
extern Table table(Table tp, int level);
extern void foreach(Table tp, int level, void(*apply)(Symbol, void *), void *cl);
// 作用域的改变
extern void enterscope();
extern void exitscope();
// 查找个建立标识符
extern Symbol install(char *name, Table *tpp, int level, int arena);
extern Symbol lookup(const char*name, Table tp);
// 标号
extern int genlabel(int);
extern Symbol findlabel(int);

/**
* type结构体保存了变量，函数，常量，结构，联合和枚举等类型信息
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
	* name域通常是符号表的关键域.
	* 对于标识符和表示类型名的关键字，
	* 该域保存了源代码中使用的名字;
	* 对于编译器生成的标识符，如无标记的结构,
	* name保存的是由数字组成的字符串
	*/
	char *name;
	// scope说明了在第k层中声明的局部变量，其scope域等于LOCAL+K
	int scope;
	// src域指明了该符号在源代码中定义点的位置
	Coordinate src;
	/**
	* up域把符号表中所有符号连成了一个链表，
	* 最后载入符号表的那个符号为链首。
	* 从后向前遍历该链表可以访问到当前作用域的所有符号,
	* 包括因为嵌套内层作用域中的标识符声明而被隐藏的符号。
	* 这种功能有助于编译后端产生调试器所用的符号表信息。
	*/
	Symbol up;
	/**
	* 此选项可以保存每个符号的所有使用信息。
	* 如果该选项被设置，
	* uses域保存一个Coordinate列表，说明了符号的使用情况.
	* 如果该选项未被设置，uses为null
	*/
	List uses;
	/**
	* 符号的扩展存储类型。
	* 可以是AUTO、REGISTER、STATIC和EXTERN
	* sclass还可以取值TYPEDEF表示typedef，取值ENUM表示枚举常量
	* 对于常量和标号该域未被使用，其值为0
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
		 * 如果两个或更多个内部标号指向相同位置，
		 * 则这些标号的equatedto域指向其中一个标号。
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

// 符号表子集
extern Table constants;
// 存放声明为extern的标识符,用于警告外部标识符声明冲突
extern Table externals;
// identifiers表的一部分，保存具有文件作用域的标识符
extern Table globals;
// 保存一般标识符
extern Table identifiers;
// 保存编译器定义的内部标号
extern Table labels;
// 存放类型标记
extern Table types;
// src中保存的信息使得apply引发的诊断程序能够指向正确的源程序坐标
extern Coordinate src;
// 全局变量level的值和对应的表一起表示了一个作用域
extern int level;
// 编译器后端
extern Interface *IR;
#endif