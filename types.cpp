#include "c.h"
#include <float.h>

/**
 * ���ͼ��Ļ�������֮һ���ж����������Ƿ�ȼۡ�
 * �����������ֻ��һ�ݸ������ȼ۲��ԾͿ��Լ򻯡�
 * �����ַ���ֻ����һ�ݸ��������ıȽϾͺܼ򵥡�
 */


static char rcsid[] = "$Id$";

// type�������ͱ�typetable
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
 * exitscope����rmtypes(lev)��typetable��ɾ����Щu.sym->scope���ڻ����lev������
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
 * eqtype�������ڲ��������Ƿ���ͬ��
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
 * type������typetable������ָ�����ͻ��ߴ���һ���µ����͡�
 * type����Ϊ�������ͺͲ���ȫ���������ʹ��������͡�
 * ����������ʱ��type��ʼ������ָ���������x��
 * �����ͼ�����Ӧ�Ĺ�ϣ���У��������µ����͡�
 * ------------------------------------------------
 * type������typetableʱ���������Ͳ������Ͳ�������ַ�����ֵ��Ϊ��ϣֵ��
 * ������Ӧ�Ĺ�ϣ����Ѱ�Ҿ�����ͬ������������������С�������ֽ����ͷ��ű���ڵ����͡�
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
 * �޷��������������з����������ξ�����ͬ�����Ͳ���������С�Ͷ����ֽ�����
 * �����в�ͬ�ķ��ű����,���ҪΪ���ǹ�����ͬ���͡�
 * ͬ����lcc����long,int,long double��double������ͬ�Ľṹ��
 * ��ÿ�ֶ��ǵ�����һ�����͡�
 * ����ĳ�������Ƿ��ʾ���������ͣ�ֻҪ����������longtype���бȽϡ�
 * IRָ�����ṩ�Ľӿڼ�¼��
 * ����voidû�ж�����
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

/** typetable �ڳ�ʼ��ʱ��ֻ���й������ͺ�void*����
 * ǰ��ʹ����Щ���������ض����ͣ�����Ϊ��֪���ڵ���������typetable.
 * type_init������ʼ����Щȫ�ֱ�����typetable.
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
		 * types������˱�ʶ�������������������͡�
		 * ����������xxinitװ�أ��Ҳ��ᱻɾ����
		 * exitscope�������ṹ�����ϡ�ö�����͵ķ��ű���ڴ�types��ɾ��ʱ��
		 * ��ṹ�����ϡ�ö�ٱ�������������Ҳ�����types����ɾ����
		 * exitscope����rmtypes(lev)��typetable��ɾ����Щu.sym->scope���ڻ����lev�����͡�
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
 * type�������Թ����������ͣ�����������װ�˶�type�ĵ��á�
 */

 // ptr��������ָ������
Type ptr(Type ty) {
	return type(POINTER, ty, IR->ptrmetric.size, IR->ptrmetric.align, pointersym);
}
/**
 * deref��ӷ���ָ�룬
 * ��deref�����������ͣ���������(POINTER ty),deref����ty.
 * def�ڲ������Ƿ�ʱ��������
 * �Ӽ�������˵����Щ���������ͼ���һ���֣����������͹��죬���ⲿ�ֲ��Էŵ����͹����У����Լ����ͼ����룬������©��
 * deref�����һ�д���ָ��ö�ٵ�ָ�룺��ӷ���ö��ָ����뷵������������δ�޶��������͡�
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
 * array(ty,n,a)������������(ARRAY n ty)��
 * ������͵Ķ����ֽ�������ty�Ķ����ֽ�����
 * array�����Ƿ�������.
 */
Type array(Type ty, int n, int a) {
	assert(ty);
	/*  
	 * C�����У���������ֺ������飬void���������Լ���GLOBAL����������������Ĳ���ȫ(����Ϊ0)���顣
	 * array���ܱ�ʾ����INT_MAX�ֽڵ�����Ĵ�С�����arrayҲ��ֹ��С����INT_MAX�ֽڵ����顣
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