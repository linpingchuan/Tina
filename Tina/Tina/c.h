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

// 常量、标号、全局变量、参数或局部变量
enum{CONSTANT=1,LABELS,GLOBAL,PARAM,LOCAL};
// 精确指明了符号在何处定义
typedef struct coord {
	// file域指明了包含该定义的文件的名字
	char *file;
	// y指明了定义出现的行号
	// x指明了定义行中的字符的位置
	unsigned x, y;
}Coordinate;
typedef struct type *Type;
/**
 * type结构体保存了变量，函数，常量，结构，联合和枚举等类型信息
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
// 符号
typedef struct symbol *Symbol;
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