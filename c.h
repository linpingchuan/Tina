#ifndef C_H
#define C_H
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "config.h"
#include "token.h"

// 宏NEW返回一个指向尚未初始化的空间指针
#define NEW(p,a) ((p)=allocate(sizeof*(p),(a)))

// 宏NEW0在分配空间后，通过C语言的memset将其初始化为 0
// memset将其第一个参数作为函数结果返回
#define NEW0(p,a) memset(NEW(((p),(a)),0,sizeof *(p)))

#undef roundup
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

#define NELEMS(a) ((int)(sizeof(a)/sizeof((a)[0])))

#define isqual(t) ((t)->op >= CONST)
/**
 * unqual函数类型返回类型(Type)的未限定(unqualified)形式。
 * 即去掉类型中的const和volatile.
 * 大多数操作作用于未限定类型，未限定类型可通过调用unqual获得。
 */
#define unqual(t) (isqual(t)?(t)->type:(t))

#define isvolatile(t) ((t)->op == VOLATILE \
					|| (t)->op == CONST+VOLATILE)

#define isconst(t) ((t)->op == CONST \
					|| (t)->op == CONST+VOLATILE)

#define isarray(t)	(unqual(t)->op == ARRAY)

#define isstruct(t) (unqual(t)->op == STRUCT \
					|| (t)->op == UNION)

#define isunion(t)	(unqual(t)->op == UNION)

#define isfunc(t)	(unqual(t)->op == FUNCTION)

#define isptr(t)	(unqual(t)->op == POINTER)

#define ischar(t)	((t)->size ==1 && isint(t))

#define isint(t)	(unqual(t)->op == INT \
					|| unqual(t)->op == UNSIGNED)

#define isfloat(t)	(unqual(t)->op == FLOAT)

#define isarith(t)	(unqual(t)->op <= UNSIGNED)

#define isunsigned(t)	(unqual(t)->op == UNSIGNED)

#define isscalar(t)	(unqual(t)->op <= POINTER \
					|| unqual(t)->op == ENUM)

#define isenum(t)	(unqual(t)->op == ENUM)

#define fieldsize(p) (p)->bitsize

#define filedright(p) ((p)->lsb-1)

#define fieldleft(p) (8*(p)->type->size - filedsize(p) -fieldright(p))

#define fieldmask(p) (~(~unsigned)0<<fieldsize(p))

#define ones(n)		((n)>=8*sizeof(unsigned long)?~0UL:~((~0UL)<<(n)))

#define optype(op) ((op)&0xF)

typedef struct node *Node;
// 指针循环列表
typedef struct _list *List;
typedef struct _type *Type;
// 符号
typedef struct symbol *Symbol;
typedef struct field *Field;

typedef struct _table *Table;

// 常量、标号、全局变量、参数或局部变量
enum { CONSTANTS = 1, LABELS, GLOBAL, PARAM, LOCAL };
// 每个操作符的最后一个字符是来自类型定义列表的类型后缀
enum {
	F = FLOAT,
	D = DOUBLE,
	C = CHAR,
	S = SHORT,
	I = INT,
	U = UNSIGNED,
	P = POINTER,
	V = VOID,
	B = STRUCT
};
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
/*
	类型度量(type metric)制定基本类型的大小和对齐字节数
	outofline标记控制相关类型的常量的位置。
	如果outofline为1，则该类型的常量不能出现在dag中，
	而是存在一个匿名的静态变量。
	对常量值的访问可通过对静态变量的存取获得。

*/
typedef struct metrics {
	unsigned char size, align, outofline;
}Metrics;

typedef struct {
	unsigned char max_unaligned_load;
	Symbol(*rmp)(int);
	void(*blkfetch)(int size, int off, int reg, int tmp);
	void(*blkstore)(int size, int off, int reg, int tmp);
	void(*blkloop)(int dreg, int doff, int sreg, int soff, int size, int tmps[]);
	void(*_label)(Node);
	int(*_rule)(void*, int);
	short **_string;
	char **_templates;
	char *_isinstruction;
	char **_ntname;
	void(*emit2)(Node);
	void(*doarg)(Node);
	void(*target)(Node);
	void(*colbber)(Node);
}Xinterface;

/*
	ptrmetric描述所有类型的指针。
	structmetric.align指定结构的最小对齐字节数，
	结构的最大对齐字节数就是它的各个域的对齐字节数和structmetric.align的最大值。
	只有某种类型的常量的值可以作为立即操作数出现在指令中时，
	编译器后端才将其类型度量的outofline标记置为0.

	--------------------------------------------------

	字符的大小和对齐的字节数都必须为1.
	前端可以正确地将符号整数，无符号整数和长整数作为不同类型处理，
	但是他们都共享intmetric。
	同样，double和long double类型共享double metric。
	每个指针必须能够存放在一个无符号整数中。

	--------------------------------------------------

	x域是对interface的扩展，后端使用它存放与目标及其相关的接口数据和函数。对后端是私有的.

	--------------------------------------------------

	如果接口标志位wants_dag为0,
	则前端生成显式的临时变量以保存那些被多次使用的公共子表达式。
	前端设置这些临时变量符号的u.t.cse域，
	u.t.cse可作为dag节点，计算所保存的值。

	--------------------------------------------------

	临时变量
*/
typedef struct interface {
	Metrics charmetric;
	Metrics shortmetric;
	Metrics intmetric;
	Metrics longmetric;
	Metrics longlongmetric;
	Metrics floatmetric;
	Metrics doublemetric;
	Metrics longdoublemetric;
	Metrics ptrmetric;
	Metrics structmetric;

	unsigned little_endian : 1;
	unsigned mulops_calls : 1;
	unsigned wants_callb : 1;
	unsigned wants_argb : 1;
	unsigned left_to_right : 1;
	unsigned wants_dag : 1;
	unsigned unsigned_char : 1;

	void(*defsymbol)(Symbol);
	void(*local)(Symbol);

	Node(*gen)(Node);
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
/**
 * 对表达式作为操作数出现的编译时常量的引用，
 * 常处理成一个指向该常量的符号的指针。
 * 这些符号保存在constants表中。
 * 与labels一样，这类表也是与作用域无关的；
 * 所有常量符号的scope域都取值CONSTANTS。
 * 按照常量类型，其值存放在相应的域中。
 * 例如，整数存放在i域，无符号字符存放在uc域。
 */
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
extern char *stringf(const char *, ...);

enum { PERM = 0, FUNC, STMT };

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

extern void error(const char*, ...);
extern void warning(const char*, ...);
// 符号表的表示
extern Table table(Table tp, int level);
extern void foreach(Table tp, int level, void(*apply)(Symbol, void *), void *cl);
// 作用域的改变
extern void enterscope();
extern void exitscope();

/*
install函数为给定的name分配一个符号，
并把该符号加入与给定作用域层数相对应的表中。
如果需要，还将建立一个新表。
该函数返回一个指向符号的指针。
name存放了字符串，根据其地址可以计算它的哈希值。
tpp是一个指向表的指针。
如果*tpp指向某作用域的表,如identifiers，
并且目前没有与参数level给定的作用域相对应的表.
则install将先为参数level给定的作用域分配一个表。
并更新*tpp;
然后 install分配一个入口，将该项清零，
最后初始化符号的某些域，并把该入口加入哈希链表中。
level必须为0，或不小于该表的作用域层数。
如果level为0，则表示name应该建立在*tpp表示的表格中。
install接受一个指明相应分配区的参数，如果有函数原型，
则使其中的符号可以永久保存，即使它们是在内层作用域中声明的。
*/
extern Symbol install(char *name, Table *tpp, int level, int arena);

extern Symbol lookup(const char* name, Table tp);
// 标号
extern int genlabel(int);
extern Symbol findlabel(int);
// 常量
extern Symbol constant(Type ty, Value v);
extern char *vtoa(Type, Value);
extern Symbol intconst(int n);
extern Symbol genident(int, Type, int);
extern Symbol temporary(int, Type, int);
extern Symbol temporary(int, Type);
extern Symbol newtemp(int, int, int);
extern Symbol findtype(Type);
// 为调试器生成标识符和符号表信息的交叉引用列表
extern void use(Symbol, Coordinate);
extern void locus(Table, Coordinate*);


extern void fprint(FILE *f, const char *fmt, ...);


// 类型管理
extern int eqtype(Type, Type, int);
extern void rmtypes(int);
extern void type_init(int, char*[]);
extern Type ptr(Type ty);
extern Type deref(Type ty);
extern Type array(Type, int, int);
extern Type func(Type ty, Type *proto, int style);
extern Type atop(Type);
extern Type qual(int op, Type ty);
extern Type freturn(Type);

extern int variadic(Type);
extern Type newstruct(int op, char *tag);
extern Field newfield(char *, Type, Type);
extern Type promote(Type ty);
extern Type compose(Type ty1, Type ty2);
extern int ttob(Type);
extern Type btot(int op);
extern Type btot(int, int);
extern int hasproto(Type);
extern void outtype(Type, FILE *);
extern void printdecl(Symbol, Type);
extern void printproto(Symbol, Symbol[]);
extern char *typestring(Type, char *);

extern Field fieldref(char *name, Type ty);
/**
 * type结构体保存了变量，函数，常量，结构，联合和枚举等类型信息
 * 输出对type节点的声明可以展示Type的内部信息，
 * 后端可以读取size域和align域，并且读取x域。
 * lcc约定，后端只允许检查这些域，而且前端可以访问Type的所有域。
 * op,type,size和align域给出了处理类型所需的大部分信息。
 * 对带有名字或标记的未限定类型，包括固有(built-in)类型、结构和联合类型以及枚举类型，
 * u.sym域指向符号表入口，符号表入口将给出更多与类型有关信息。
 * 符号表入口给出了类型名。此外，如果该类型的常量可以作为指令的一部分。
 * u.sym->addressed的值为0。
 * u.sym->type反向指向类型自身，可用于将标记映射成类型。
 * 每个结构、联合和枚举类型都有一个符号表入口，
 * 每种基本类型对应一个符号表入口，所有指针类型共用一个符号表入口。
 * 这些入口都出现types表中。
 * 使用这种表示，sym.c中定义的函数可用于管理types
 */
struct _type {
	// op域存放整型的类型操作符编码
	int op;
	// type域存放类型操作数
	Type type;
	// align域和size域以字节为单位给出类型的对齐字节数和该类型对象的大小。
	// 代码生成接口中规定，size必须是align的整数倍。
	// 编译器后端必须为变量分配空间，使得变量地址是变量类型的对齐字节数的整数倍。
	int align;
	int size;
	union {
		// 结构和联合类型是通过标记识别的。
		Symbol sym;
		// 函数类型的type域给出了函数返回值的类型
		struct {
			unsigned oldstyle : 1;
			Type *proto;
		}f;
	}u;
	// x域扮演的角色与它在符号结构symbols中的一样。
	// 编译器后端通过定义Xtype，向类型结构中加入与目标机器相关的域。
	// 这一机制通常用于对调试器的支持。
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
/**
 * 在某些目标机器上，有些常量，主要是浮点数，不能存储在指令中。
 * 所以编译器产生一个静态变量并将其初始化成该浮点常量的值。
 * 对于这些常量，u.c.loc指向了产生的变量所对应的符号。
 * 总体上，type和u.c域保存了敞亮的所有已知信息。
 */
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
	/* 
		临时变量的temporary域和generated域都置为1，
		而标号和其他生成的变量，
		如存放字符串文字的变量，只有generated置为1时,structarg标识结构参数。
	*/
	unsigned temporary : 1;
	unsigned generated : 1;
	unsigned defined : 1;
	/* 对于变量和常量，type域用于后端查询数据项的数据类型后缀 */
	Type type;
	/* 
		对于变量和标号，
		ref域的浮点值用于估计变量和标号被引用的次数，
		非零的值表示变量或标号至少被引用1次。
	*/
	float ref;
	/*
		对于标号，常量和某些变量，
		联合u的域补充说明了一些额外的数据
	*/
	union {
		/**
		 * 如果两个或更多个内部标号指向相同位置，
		 * 则这些标号的equatedto域指向其中一个标号。
		 * ----------------------------------------
		 * 标号的scope域等于LABELS，u.l.label是一个唯一标识该标号的数值，
		 * name是值的字符串的表示。标号没有type或sclass。
		 */
		 // 结构和枚举类型
		struct {
			int label;
			Symbol equatedto;
		}l;
		// 函数类型
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
		/*
			常量的scope域等于CONSTANTS，sclass等于STATIC
			对于整形或指针常量，name是C常量的字符串表示。
			对其他类型，name没有定义。
			常量的实际值存放在u.c.v域中。
			如果需要生成一个变量来存放常量，那么u.c.loc指向该常量的符号表入口。
		*/
		struct {
			Value v;
			Symbol loc;
		}c; // 常量
		struct {
			Coordinate pt;
			int label;
			int ncalls;
			Symbol *callee;
		}f;
		int seg;
		Symbol alias;
		/*
			如果接口标志wants_dag为0,
			则前端生成显式的临时变量以保存那些使用多次的公共子表达式。
			前端设置这些临时变量符号的u.t.cse域，
			u.t.cse可作为dag节点，计算所保存的值。
		*/
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
// 常量表
extern Table constants;

extern int Aflag;
extern int xref;

extern Type chartype;
extern Type doubletype;
extern Type floattype;
extern Type inttype;
extern Type longdouble;
extern Type longtype;
extern Type longlong;
extern Type shorttype;
extern Type signedchar;
extern Type unsignedchar;
extern Type unsignedlonglong;
extern Type unsignedlong;
extern Type unsignedshort;
extern Type unsignedtype;
extern Type charptype;
extern Type funcptype;
extern Type voidptype;
extern Type voidtype;
extern Type unsignedptr;
extern Type signedptr;
extern Type widechar;
#endif