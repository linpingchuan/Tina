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

// ��NEW����һ��ָ����δ��ʼ���Ŀռ�ָ��
#define NEW(p,a) ((p)=allocate(sizeof*(p),(a)))

// ��NEW0�ڷ���ռ��ͨ��C���Ե�memset�����ʼ��Ϊ 0
// memset�����һ��������Ϊ�����������
#define NEW0(p,a) memset(NEW(((p),(a)),0,sizeof *(p)))

#undef roundup
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

#define NELEMS(a) ((int)(sizeof(a)/sizeof((a)[0])))

#define isqual(t) ((t)->op >= CONST)
/**
 * unqual�������ͷ�������(Type)��δ�޶�(unqualified)��ʽ��
 * ��ȥ�������е�const��volatile.
 * ���������������δ�޶����ͣ�δ�޶����Ϳ�ͨ������unqual��á�
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
// ָ��ѭ���б�
typedef struct _list *List;
typedef struct _type *Type;
// ����
typedef struct symbol *Symbol;
typedef struct field *Field;

typedef struct _table *Table;

// ��������š�ȫ�ֱ�����������ֲ�����
enum { CONSTANTS = 1, LABELS, GLOBAL, PARAM, LOCAL };
// ÿ�������������һ���ַ����������Ͷ����б�����ͺ�׺
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
	unsigned char size, align, outofline;
}Metrics;

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
 * �Ա��ʽ��Ϊ���������ֵı���ʱ���������ã�
 * �������һ��ָ��ó����ķ��ŵ�ָ�롣
 * ��Щ���ű�����constants���С�
 * ��labelsһ���������Ҳ�����������޹صģ�
 * ���г������ŵ�scope��ȡֵCONSTANTS��
 * ���ճ������ͣ���ֵ�������Ӧ�����С�
 * ���磬���������i���޷����ַ������uc��
 */
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
// ���ű�ı�ʾ
extern Table table(Table tp, int level);
extern void foreach(Table tp, int level, void(*apply)(Symbol, void *), void *cl);
// ������ĸı�
extern void enterscope();
extern void exitscope();

/*
install����Ϊ������name����һ�����ţ�
���Ѹ÷��ż��������������������Ӧ�ı��С�
�����Ҫ����������һ���±�
�ú�������һ��ָ����ŵ�ָ�롣
name������ַ������������ַ���Լ������Ĺ�ϣֵ��
tpp��һ��ָ����ָ�롣
���*tppָ��ĳ������ı�,��identifiers��
����Ŀǰû�������level���������������Ӧ�ı�.
��install����Ϊ����level���������������һ����
������*tpp;
Ȼ�� install����һ����ڣ����������㣬
����ʼ�����ŵ�ĳЩ�򣬲��Ѹ���ڼ����ϣ�����С�
level����Ϊ0����С�ڸñ�������������
���levelΪ0�����ʾnameӦ�ý�����*tpp��ʾ�ı���С�
install����һ��ָ����Ӧ�������Ĳ���������к���ԭ�ͣ�
��ʹ���еķ��ſ������ñ��棬��ʹ���������ڲ��������������ġ�
*/
extern Symbol install(char *name, Table *tpp, int level, int arena);

extern Symbol lookup(const char* name, Table tp);
// ���
extern int genlabel(int);
extern Symbol findlabel(int);
// ����
extern Symbol constant(Type ty, Value v);
extern char *vtoa(Type, Value);
extern Symbol intconst(int n);
extern Symbol genident(int, Type, int);
extern Symbol temporary(int, Type, int);
extern Symbol temporary(int, Type);
extern Symbol newtemp(int, int, int);
extern Symbol findtype(Type);
// Ϊ���������ɱ�ʶ���ͷ��ű���Ϣ�Ľ��������б�
extern void use(Symbol, Coordinate);
extern void locus(Table, Coordinate*);


extern void fprint(FILE *f, const char *fmt, ...);


// ���͹���
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
 * type�ṹ�屣���˱������������������ṹ�����Ϻ�ö�ٵ�������Ϣ
 * �����type�ڵ����������չʾType���ڲ���Ϣ��
 * ��˿��Զ�ȡsize���align�򣬲��Ҷ�ȡx��
 * lccԼ�������ֻ��������Щ�򣬶���ǰ�˿��Է���Type��������
 * op,type,size��align������˴�����������Ĵ󲿷���Ϣ��
 * �Դ������ֻ��ǵ�δ�޶����ͣ���������(built-in)���͡��ṹ�����������Լ�ö�����ͣ�
 * u.sym��ָ����ű���ڣ����ű���ڽ����������������й���Ϣ��
 * ���ű���ڸ����������������⣬��������͵ĳ���������Ϊָ���һ���֡�
 * u.sym->addressed��ֵΪ0��
 * u.sym->type����ָ���������������ڽ����ӳ������͡�
 * ÿ���ṹ�����Ϻ�ö�����Ͷ���һ�����ű���ڣ�
 * ÿ�ֻ������Ͷ�Ӧһ�����ű���ڣ�����ָ�����͹���һ�����ű���ڡ�
 * ��Щ��ڶ�����types���С�
 * ʹ�����ֱ�ʾ��sym.c�ж���ĺ��������ڹ���types
 */
struct _type {
	// op�������͵����Ͳ���������
	int op;
	// type�������Ͳ�����
	Type type;
	// align���size�����ֽ�Ϊ��λ�������͵Ķ����ֽ����͸����Ͷ���Ĵ�С��
	// �������ɽӿ��й涨��size������align����������
	// ��������˱���Ϊ��������ռ䣬ʹ�ñ�����ַ�Ǳ������͵Ķ����ֽ�������������
	int align;
	int size;
	union {
		// �ṹ������������ͨ�����ʶ��ġ�
		Symbol sym;
		// �������͵�type������˺�������ֵ������
		struct {
			unsigned oldstyle : 1;
			Type *proto;
		}f;
	}u;
	// x����ݵĽ�ɫ�����ڷ��Žṹsymbols�е�һ����
	// ���������ͨ������Xtype�������ͽṹ�м�����Ŀ�������ص���
	// ��һ����ͨ�����ڶԵ�������֧�֡�
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
 * ��ĳЩĿ������ϣ���Щ��������Ҫ�Ǹ����������ܴ洢��ָ���С�
 * ���Ա���������һ����̬�����������ʼ���ɸø��㳣����ֵ��
 * ������Щ������u.c.locָ���˲����ı�������Ӧ�ķ��š�
 * �����ϣ�type��u.c�򱣴��˳�����������֪��Ϣ��
 */
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
		 // �ṹ��ö������
		struct {
			int label;
			Symbol equatedto;
		}l;
		// ��������
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
		}c; // ����
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
// ������
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