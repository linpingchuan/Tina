#include "c.h"
#include <float.h>

/**
 * 类型检查的基本操作之一是判断两个类型是否等价。
 * 如果任意类型只有一份副本，等价测试就可以简化。
 * 任意字符串只保留一份副本，串的比较就很简单。
 */


static char rcsid[] = "$Id$";

// type管理类型表typetable
static struct entry {
	struct _type type;
	struct entry *link;
} *typetable[128];

static Symbol pointersym;

static int maxlevel;

Type chartype;
Type doubletype;
Type floattype;
Type inttype;
Type longdouble;
Type longtype;
Type longlong;
Type shorttype;
Type signedchar;
Type unsignedchar;
Type unsignedlong;
Type unsignedlonglong;
Type unsignedshort;
Type unsignedtype;
Type funcptype;
Type charptype;
Type voidptype;
Type voidtype;
Type unsignedptr;
Type signedptr;
Type widechar;
/**
 * exitscope调用rmtypes(lev)从typetable中删除那些u.sym->scope大于或等于lev的类型
 */
void rmtypes(int lev) {
	if (maxlevel >= lev) {
		int i;
		maxlevel = 0;
		for (i = 0; i < NELEMS(typetable); i++) {
			struct entry *tn, **tq = &typetable[i];
			// TODO
		}
	}
}
/**
 * eqtype函数用于测试类型是否相同。
 */
int eqtype(Type ty1, Type ty2, int ret) {
	if (ty1 == ty2)
		return 1;
	if (ty1->op != ty2->op)
		return 0;

	//assert(0);
	return 0;
}

Type btot(int op, int size) {
#define xx(ty) if(size==(ty)->size) return ty;
#undef xx
	return 0;
}
/**
 * type函数在typetable中搜索指定类型或者创造一个新的类型。
 * type总是为函数类型和不完全的数组类型创建新类型。
 * 创建新类型时，type初始化参数指定的域，清空x域，
 * 将类型加入相应的哈希链中，并返回新的类型。
 * ------------------------------------------------
 * type在搜索typetable时，利用类型操作符和操作数地址的异或值作为哈希值，
 * 搜索相应的哈希链，寻找具有相同操作符、操作数、大小、对齐字节数和符号表入口的类型。
 */
static Type type(int op, Type ty, int size, int align, void *sym) {
	unsigned h = (op ^ ((unsigned long)ty >> 3))&(NELEMS(typetable) - 1);
	struct entry *tn;
	if (op != FUNCTION && (op != ARRAY || size > 0))
		for (tn = typetable[h]; tn; tn = tn->link)
			if (tn->type.op == op&&tn->type.type == ty
				&&tn->type.size == size&&tn->type.align == align
				&&tn->type.u.sym == sym)
				return &tn->type;
	tn = (struct entry*) memset(allocate(sizeof*(tn), PERM), 0, sizeof*(tn));
	tn->type.op = op;
	tn->type.type = ty;
	tn->type.size = size;
	tn->type.u.sym = (Symbol)sym;
	tn->link = typetable[h];
	typetable[h] = tn;
	return &tn->type;
}
/**
 * 无符号整数类型与有符号整数整形具有相同的类型操作符、大小和对齐字节数，
 * 但具有不同的符号表入口,因此要为他们构建不同类型。
 * 同样，lcc假设long,int,long double和double具有相同的结构。
 * 但每种都是单独的一种类型。
 * 测试某个类型是否表示长整数类型，只要将该类型与longtype进行比较。
 * IR指向后端提供的接口记录。
 * 类型void没有度量。
 *
 */
static Type xxinit(int op, char *name, Metrics m) {
	Symbol p = install(string(name), &types, GLOBAL, PERM);
	Type ty = type(op, 0, m.size, m.align, p);

	assert(ty->align == 0 || ty->size%ty->align == 0);
	p->type = ty;
	p->addressed = m.outofline;

	switch (ty->op) {
	case INT:
		p->u.limits.max.i = ones(8 * ty->size) >> 1;
		p->u.limits.min.i = -p->u.limits.max.i - 1;
		break;
	case UNSIGNED:
		p->u.limits.max.u = ones(8 * ty->size);
		p->u.limits.max.u = 0;
		break;
	case FLOAT:
		if (ty->size == sizeof(float))
			p->u.limits.max.d = FLT_MAX;
		else if (ty->size == sizeof(double))
			p->u.limits.max.d = DBL_MAX;
		else
			p->u.limits.max.d = LDBL_MAX;
		p->u.limits.min.d = -p->u.limits.max.d;
		break;
	default:assert(0);
	}
	return ty;
}

/** typetable 在初始化时，只具有固有类型和void*类型
 * 前端使用这些变量引用特定类型，避免为已知存在的类型搜索typetable.
 * type_init函数初始化这些全局变量和typetable.
 */
void type_init(int argc, char *argv[]) {
	static int inited;
	int i;

	if (inited)
		return;
	inited = 1;
	if (!IR)
		return;
	for (i = 1; i < argc; i++) {
		int size, align, outofline;
		if (strncmp(argv[i], "-unsigned_char=", 15) == 0)
			IR->unsigned_char = argv[i][15] - '0';
#define xx(name) \
	else if(sscanf_s(argv[i],"-"#name"=%d,%d,%d",&size,&align,&outofline)==3){\
		IR->name.size=size;IR->name.align=align;\
			IR->name.outofline = outofline;}
		xx(charmetric)
			xx(shortmetric)
			xx(intmetric)
			xx(longmetric)
			xx(longlongmetric)
			xx(floatmetric)
			xx(doublemetric)
			xx(longdoublemetric)
			xx(ptrmetric)
			xx(structmetric)
#undef xx
	}
#define xx(v,name,op,metrics) v=xxinit(op,name,IR->metrics)
	xx(chartype, "char", IR->unsigned_char ? UNSIGNED : INT, charmetric);
	xx(doubletype, "double", FLOAT, doublemetric);
	xx(floattype, "float", FLOAT, floatmetric);
	xx(inttype, "int", INT, intmetric);
	xx(longdouble, "long double", FLOAT, longdoublemetric);
	xx(longtype, "long int", INT, longmetric);
	xx(longlong, "long long int", INT, longlongmetric);
	xx(shorttype, "short", INT, shortmetric);
	xx(signedchar, "signed char", INT, charmetric);
	xx(unsignedchar, "unsigned char", UNSIGNED, charmetric);
	xx(unsignedlong, "unsigned long", UNSIGNED, longmetric);
	xx(unsignedshort, "unsigned short", UNSIGNED, shortmetric);
	xx(unsignedtype, "unsigned int", UNSIGNED, intmetric);
	xx(unsignedlonglong, "unsigned long long", UNSIGNED, longlongmetric);
#undef xx
	{
		/**
		 * types表包含了标识符或标记命名的所有类型。
		 * 基本类型由xxinit装载，且不会被删除。
		 * exitscope函数将结构、联合、枚举类型的符号表入口从types中删除时，
		 * 与结构、联合、枚举标记相关联的类型也必须从types表中删除。
		 * exitscope调用rmtypes(lev)从typetable中删除那些u.sym->scope大于或等于lev的类型。
		 */
		Symbol p;
		p = install(string("void"), &types, GLOBAL, PERM);
		voidtype = type(VOID, NULL, 0, 0, p);
		p->type = voidtype;
	}
	pointersym = install(string("T*"), &types, GLOBAL, PERM);
	pointersym->addressed = IR->ptrmetric.outofline;
	pointersym->u.limits.max.p = (void*)ones(8 * IR->ptrmetric.size);
	pointersym->u.limits.min.p = 0;
	voidtype = ptr(voidtype);
	funcptype = ptr(func(voidtype, NULL, 1));
	chartype = ptr(chartype);
}
/**
 * type函数可以构造任意类型，其他函数封装了对type的调用。
 */

 // ptr函数创建指针类型
Type ptr(Type ty) {
	return type(POINTER, ty, IR->ptrmetric.size, IR->ptrmetric.align, pointersym);
}
/**
 * deref间接访问指针，
 * 即deref返回引用类型，给定类型(POINTER ty),deref返回ty.
 * def在操作数非法时产生错误。
 * 从技术上来说，这些测试是类型检查的一部分，不属于类型构造，将这部分测试放到类型构造中，可以简化类型检查代码，避免疏漏。
 * deref的最后一行处理指向枚举的指针：间接访问枚举指针必须返回与它关联的未限定整数类型。
 */
Type deref(Type ty) {
	if (isptr(ty))
		ty = ty->type;
	else
		error("type error: %s\n", "pointer expected");
	return isenum(ty) ? unqual(ty)->type : ty;
}

Type func(Type ty, Type *proto, int style) {
	return ty;
}

/**
 * array(ty,n,a)函数创建类型(ARRAY n ty)，
 * 结果类型的对其字节数就是ty的对齐字节数。
 * array还检查非法操作数.
 */
Type array(Type ty, int n, int a) {
	assert(ty);
	/*  
	 * C语言中，不允许出现函数数组，void类型数组以及除GLOBAL外其他任意作用域的不完全(长度为0)数组。
	 * array不能表示超过INT_MAX字节的数组的大小，因此array也禁止大小超过INT_MAX字节的数组。
	 */
	if (isfunc(ty)) {
		error("illegal type 'array of %t'\n", ty);
		return array(inttype, n, 0);
	}
	if (level >= GLOBAL&&isarray(ty) && ty->size == 0)
		error("missing array size\n");
	if (ty->size == 0) {
		if (unqual(ty) == voidtype)
			error("illegal type 'array of %t'\n", ty);
		else if (Aflag >= 2)
			warning("declaring type 'array of %t' is undefined\n", ty);
	}
	else if (n > INT_MAX / ty->size) {
		error("size of 'array of %t' exceeds %d bytes", ty, INT_MAX);
		n = 1;
	}

	return type(ARRAY, ty, n*ty->size, a ? a : ty->align, NULL);
}