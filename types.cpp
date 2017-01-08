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
/*
  eqtype函数用于测试类型是否相同。
  判断两个类型是否兼容是类型检查的关键。
  如果两个类型兼容，eqtype函数返回1，反之返回0.
  如果ty1或ty2是不完全类型，则eqtype返回第三个参数ret的值。
  每个类型总是与自身兼容。
  

 */
int eqtype(Type ty1, Type ty2, int ret) {
	/*
		type函数保证大多数类型只有一个实例，这样许多兼容类型都可以通过eqtype的第一步测试。
		同样，许多类型不兼容都是因为测试类型具有不同的操作符，绝对是不兼容的，导致eqtype返回0.
	*/
	if (ty1 == ty2)
		return 1;
	/*
		如果不同类型具有相同的操作符CHAR，SHORT，UNSIGNED或INT，
		但表示不同的类型，如unsigned short和signed short，这两个类型也不兼容。
		同样，两个枚举类型，结构或联合类型，只有当他们是相同类型才是兼容的。
	*/
	if (ty1->op != ty2->op)
		return 0;
	/*
		1.指针类型：如果两个指针类型引用的类型兼容，则指针类型兼容;
		
		2.非限定类型：如果两个相似的限定类型的非限定类型，则它们也兼容;
		
		3.数组类型：如果其中一个数组为不完全类型，eqtype返回ret，否则它们的类型仍兼容。
				   当eqtype递归调用自身时，ret总为1，在别处调用时ret通常也为1.
				   一些操作符，如指针比较，要求操作数要么都是不完全类型，要么都是完全类型，
				   这时eqtype的调用参数ret等于0.
				   上面代码中的第一个测试时处理两个数组的大小都是位置的情况.
		
		4.函数类型：如果两个函数类型的返回类型和原型都兼容，那么这两个函数类型都兼容。
				   
	*/
	switch (ty1->op) {
	case ENUM:case UNION:case STRUCT:
	case UNSIGNED:case INT:case FLOAT:
		return 0;
	case POINTER:
		return eqtype(ty1->type, ty2->type, 1);
	case CONST:case VOLATILE:case CONST+VOLATILE:
		return eqtype(ty1->type, ty2->type, 1);
	case ARRAY:
		if (eqtype(ty1->type, ty2->type, 1)) {
			if (ty1->size == ty2->size)
				return 1;
			if (ty1->size == 0 || ty2->size == 0)
				return ret;
		}
		return 0;
	case FUNCTION:
		if (eqtype(ty1->type, ty2->type, 1)) {
			Type *p1 = ty1->u.f.proto, *p2 = ty2->u.f.proto;
			if (p1 == p2)
				return 1;
			if (p1&&p2) {
				for (; *p1&&*p2; p1++, p2++) {
					if (eqtype(unqual(*p1), unqual(*p2), 1) == 0)
						return 0;
				}
			}
			else {

			}
			return 0;

		}
	}

	assert(0);
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
/*
	struct{
		unsigned oldstyle:1;
		Type *proto;
	}f;
	f.oldstyle标记区分两种函数类型：
	1表示旧风格(old style)类型，即省略参数的类型；
	0表示新风格(new style)类型，即总是包含参数的类型。
	f.proto指向以空指针(null)结尾的Type数组。
	f.proto[i]是第i+1个参数的类型。
	因为旧风格的函数类型可能带原型，需要f.oldstyle标记。
	
    -------------------------------------------------------------------

	func函数创建类型（FUNCTION ty{proto}),ty是返回值类型，花括号括住的是圆形。
	func初始化原型和old-style标记。
 */
Type func(Type ty, Type *proto, int style) {
	if (ty && (isarray(ty) || isfunc(ty)))
		error("illegal return type '%t'\n", ty);
	ty = type(FUNCTION, ty, 0, 0, NULL);
	ty->u.f.proto = proto;
	ty->u.f.oldstyle = style;
	return ty;
}

/*
	freturn对于函数类型的作用与deref对指针类型的作用一样
	他以类型(FUNCTION ty)作为输入，间接访问(FUNCTION ty),
	生成函数返回值类型ty
*/
Type freturn(Type ty) {
	if (isfunc(ty))
		return ty->type;
	error("type error: %s\n", "function expected");
	return inttype;
}
/*
	variadic断言通过查找函数原型的末尾是否有void类型来测试函数类型是否带有长度可变的参数列表
	参数数目可变的函数要求至少声明了一个参数，然后才是0个或多个可选参数。
	因此，判断时不会将在原型结尾处的void与不带参数的函数搞混淆，后者的原型只有1个元素，即{(VOID)}
*/
int variadic(Type ty) {
	if (isfunc(ty) && ty->u.f.proto) {
		int i;
		for (i = 0; ty->u.f.proto[i]; i++)
			;
		return i > 1 && ty->u.f.proto[i - 1] == voidtype;
	}
	return 0;
}
/*
	newstruct创建新的类型(STRUCT["tag"])或(UNION["tag"]),
	tag是标记。
	声明或定义一个新的结构或联合时，无论有没有域列表，
	structdcl函数调用newstruct。
	创建新的结构体或联合类型时，类型的标记装入types表。
	对匿名的结构和联合，也就是没有标记的结构和联合，
	newstruct为它们生成标记。
	---------------------------------------------
	将新的标记加入types表时，可能会创建作用域层数大于maxlevel的入口，因此，如果需要，应调整maxlevel.
	结构类型指向它们的符号表入口，反过来符号表入口也指向结构类型，这样，标记可以映射到类型，类型也可以映射到标记.
	例如，当标记用在声明符中时，标记映射成类型，参见structdcl。
	当rmtypes将类型从typetable中删除时，需要将类型映射成标记。
*/
Type newstruct(int op, char *tag) {
	Symbol p;

	assert(tag);
	if (*tag == 0)
		tag = stringd(genlabel(1));
	else {
		/*
			在同一作用域中多次定义相同的标记是非法的，但多次声明同一标记是合法的。
			如果给出了包含域的结构声明，那么就声明并定义了结构标记；
			如果使用不包含域的结构标记，那么只是声明标记而已。例如：
			struct employee{
				char *name;
				struct date *hired;
				char ssn[9];
			}
			声明并定义了employee，但只是声明了date.
			定义一个标记时，其defined标记被设置，通过检查defined标记可以判断标记是否重复定义。
			+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			参数和参数类型的作用域为PARAM,局部变量的作用域从PARAM+1开始。
			ANSI C规定了参数与顶层局部变量的作用域相同，因此必须测试该作用域下局部变量是否重定义了参数中定义的标记。
			这种作用域的划分并不是ANSI C标准的要求，lcc内部使用它来区分参数和局部变量，foreach函数能够分别分别访问他们。

			+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		*/
		if ((p = lookup(tag, types)) != NULL 
			&& (p->scope == level
			|| p->scope == PARAM
			&&level == PARAM + 1)) {
			if (p->type->op == op && !p->defined)
				return p->type;
			error("redefinition of '%s' previously defined at %w\n", p->name, &p->src);
		}
	}
	p = install(tag, &types, level, PERM);
	p->type = type(op, NULL, 0, 0, p);
	if (p->scope > maxlevel)
		maxlevel = p->scope;
	p->src = src;
	return p->type;

}
/*
	newfield函数分配一个field结构，将该结构附加到结构类型ty的符号表入口的域列表中，
	从而在ty中加入一个类型为fty的域。
*/
Field newfield(char *name, Type ty, Type fty) {
	Field p, *q = &ty->u.sym->u.s.flist;
	if (name == NULL)
		name = stringd(genlabel(1));
	for (p = *q; p; q = &p->link, p = *q)
		if (p->name == name)
			error("duplicate field name '%s' in '%t'\n", name, ty);
	p = (Field)memset(allocate(sizeof*(p), PERM), 0, sizeof*(p));
	*q = p;
	p->name = name;
	p->type = ty;

	if (xref) {
		if (ty->u.sym->u.s.ftab == NULL)
			ty->u.sym->u.s.ftab = table(NULL, level);
		install(name, &ty->u.sym->u.s.ftab, 0, PERM)->src = src;
	}
	return p;
}
/**
 * array(ty,n,a)函数创建类型(ARRAY n ty)，
 * 结果类型的对其字节数就是ty的对齐字节数。
 * array还检查非法操作数.
 * C语言中，不允许出现函数数组，void类型数组以及除GLOBAL外其他任意作用域的不完全(长度为0)数组。
 * array不能表示超过INT_MAX字节的数组的大小，因此array也禁止大小超过INT_MAX字节的数组。
 * 如果lcc编译选项-A出现了两次，则将Aflag置为2，表示lcc要对ANSI用法报警。
 * 因此，当Aflag等于2时，如果声明的不完全数组的类型是不完全类型，array应该输出警告信息。
 */
Type array(Type ty, int n, int a) {
	assert(ty);
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
/**
 * 在许多情况下，数组类型"退化"(decay)为指向数组元素类型的指针
 * 例如当数组作为形参的类型时。
 * atop函数实现这种"退化".
 */
Type atop(Type ty) {
	if (isarray(ty))
		return ptr(ty->type);
	error("type error: %s\n", "array expected");
	return ptr(ty);
}
/**
 * qual分别构造限定类型。
 * 给定限定类型ty,qual检查非法的操作数，
 * 创建(CONST ty),(VOLATILE ty)或(CONST+VOLATILE ty).
 * 如果ty是类型(ARRAY ety),而限定是作用于数组元素类型，
 * 那么qual(op,ty)就会创建(ARRAY(op,ety))。
 * 如果ty已经是限定的，即是(CONST ty->type)或(VOLATILE ty->type),
 * 并且op是另一个限定符，那么qual创建(CONST+VOLATILE ty->type).
 * 我们只使用一个类型节点而不是一或两个类型节点来描述限定类型
 */
Type qual(int op, Type ty) {
	if (isarray(ty)) {
		ty = type(ARRAY, qual(op, ty->type), ty->size, ty->align, NULL);
	}
	else if (isfunc(ty)) {
		warning("qualified function type ignored\n");
	}
	else if (isconst(ty) && op == CONST || isvolatile(ty) && op == VOLATILE) {
		error("illegal type %k %t\n", op, ty);
	}
	else {
		if (isqual(ty)) {
			op += ty->op;
			ty = ty->type;
		}
		ty = type(op, ty, ty->size, ty->align, NULL);
	}
	return ty;
}