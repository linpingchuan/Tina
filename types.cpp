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
/*
  eqtype�������ڲ��������Ƿ���ͬ��
  �ж����������Ƿ���������ͼ��Ĺؼ���
  ����������ͼ��ݣ�eqtype��������1����֮����0.
  ���ty1��ty2�ǲ���ȫ���ͣ���eqtype���ص���������ret��ֵ��
  ÿ������������������ݡ�
  

 */
int eqtype(Type ty1, Type ty2, int ret) {
	/*
		type������֤���������ֻ��һ��ʵ�����������������Ͷ�����ͨ��eqtype�ĵ�һ�����ԡ�
		ͬ����������Ͳ����ݶ�����Ϊ�������;��в�ͬ�Ĳ������������ǲ����ݵģ�����eqtype����0.
	*/
	if (ty1 == ty2)
		return 1;
	/*
		�����ͬ���;�����ͬ�Ĳ�����CHAR��SHORT��UNSIGNED��INT��
		����ʾ��ͬ�����ͣ���unsigned short��signed short������������Ҳ�����ݡ�
		ͬ��������ö�����ͣ��ṹ���������ͣ�ֻ�е���������ͬ���Ͳ��Ǽ��ݵġ�
	*/
	if (ty1->op != ty2->op)
		return 0;
	/*
		1.ָ�����ͣ��������ָ���������õ����ͼ��ݣ���ָ�����ͼ���;
		
		2.���޶����ͣ�����������Ƶ��޶����͵ķ��޶����ͣ�������Ҳ����;
		
		3.�������ͣ��������һ������Ϊ����ȫ���ͣ�eqtype����ret���������ǵ������Լ��ݡ�
				   ��eqtype�ݹ��������ʱ��ret��Ϊ1���ڱ𴦵���ʱretͨ��ҲΪ1.
				   һЩ����������ָ��Ƚϣ�Ҫ�������Ҫô���ǲ���ȫ���ͣ�Ҫô������ȫ���ͣ�
				   ��ʱeqtype�ĵ��ò���ret����0.
				   ��������еĵ�һ������ʱ������������Ĵ�С����λ�õ����.
		
		4.�������ͣ���������������͵ķ������ͺ�ԭ�Ͷ����ݣ���ô�������������Ͷ����ݡ�
				   
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
/*
	struct{
		unsigned oldstyle:1;
		Type *proto;
	}f;
	f.oldstyle����������ֺ������ͣ�
	1��ʾ�ɷ��(old style)���ͣ���ʡ�Բ��������ͣ�
	0��ʾ�·��(new style)���ͣ������ǰ������������͡�
	f.protoָ���Կ�ָ��(null)��β��Type���顣
	f.proto[i]�ǵ�i+1�����������͡�
	��Ϊ�ɷ��ĺ������Ϳ��ܴ�ԭ�ͣ���Ҫf.oldstyle��ǡ�
	
    -------------------------------------------------------------------

	func�����������ͣ�FUNCTION ty{proto}),ty�Ƿ���ֵ���ͣ���������ס����Բ�Ρ�
	func��ʼ��ԭ�ͺ�old-style��ǡ�
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
	freturn���ں������͵�������deref��ָ�����͵�����һ��
	��������(FUNCTION ty)��Ϊ���룬��ӷ���(FUNCTION ty),
	���ɺ�������ֵ����ty
*/
Type freturn(Type ty) {
	if (isfunc(ty))
		return ty->type;
	error("type error: %s\n", "function expected");
	return inttype;
}
/*
	variadic����ͨ�����Һ���ԭ�͵�ĩβ�Ƿ���void���������Ժ��������Ƿ���г��ȿɱ�Ĳ����б�
	������Ŀ�ɱ�ĺ���Ҫ������������һ��������Ȼ�����0��������ѡ������
	��ˣ��ж�ʱ���Ὣ��ԭ�ͽ�β����void�벻�������ĺ�������������ߵ�ԭ��ֻ��1��Ԫ�أ���{(VOID)}
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
	newstruct�����µ�����(STRUCT["tag"])��(UNION["tag"]),
	tag�Ǳ�ǡ�
	��������һ���µĽṹ������ʱ��������û�����б�
	structdcl��������newstruct��
	�����µĽṹ�����������ʱ�����͵ı��װ��types��
	�������Ľṹ�����ϣ�Ҳ����û�б�ǵĽṹ�����ϣ�
	newstructΪ�������ɱ�ǡ�
	---------------------------------------------
	���µı�Ǽ���types��ʱ�����ܻᴴ���������������maxlevel����ڣ���ˣ������Ҫ��Ӧ����maxlevel.
	�ṹ����ָ�����ǵķ��ű���ڣ����������ű����Ҳָ��ṹ���ͣ���������ǿ���ӳ�䵽���ͣ�����Ҳ����ӳ�䵽���.
	���磬�����������������ʱ�����ӳ������ͣ��μ�structdcl��
	��rmtypes�����ʹ�typetable��ɾ��ʱ����Ҫ������ӳ��ɱ�ǡ�
*/
Type newstruct(int op, char *tag) {
	Symbol p;

	assert(tag);
	if (*tag == 0)
		tag = stringd(genlabel(1));
	else {
		/*
			��ͬһ�������ж�ζ�����ͬ�ı���ǷǷ��ģ����������ͬһ����ǺϷ��ġ�
			��������˰�����Ľṹ��������ô�������������˽ṹ��ǣ�
			���ʹ�ò�������Ľṹ��ǣ���ôֻ��������Ƕ��ѡ����磺
			struct employee{
				char *name;
				struct date *hired;
				char ssn[9];
			}
			������������employee����ֻ��������date.
			����һ�����ʱ����defined��Ǳ����ã�ͨ�����defined��ǿ����жϱ���Ƿ��ظ����塣
			+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			�����Ͳ������͵�������ΪPARAM,�ֲ��������������PARAM+1��ʼ��
			ANSI C�涨�˲����붥��ֲ���������������ͬ����˱�����Ը��������¾ֲ������Ƿ��ض����˲����ж���ı�ǡ�
			����������Ļ��ֲ�����ANSI C��׼��Ҫ��lcc�ڲ�ʹ���������ֲ����;ֲ�������foreach�����ܹ��ֱ�ֱ�������ǡ�

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
	newfield��������һ��field�ṹ�����ýṹ���ӵ��ṹ����ty�ķ��ű���ڵ����б��У�
	�Ӷ���ty�м���һ������Ϊfty����
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
 * array(ty,n,a)������������(ARRAY n ty)��
 * ������͵Ķ����ֽ�������ty�Ķ����ֽ�����
 * array�����Ƿ�������.
 * C�����У���������ֺ������飬void���������Լ���GLOBAL����������������Ĳ���ȫ(����Ϊ0)���顣
 * array���ܱ�ʾ����INT_MAX�ֽڵ�����Ĵ�С�����arrayҲ��ֹ��С����INT_MAX�ֽڵ����顣
 * ���lcc����ѡ��-A���������Σ���Aflag��Ϊ2����ʾlccҪ��ANSI�÷�������
 * ��ˣ���Aflag����2ʱ����������Ĳ���ȫ����������ǲ���ȫ���ͣ�arrayӦ�����������Ϣ��
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
 * ���������£���������"�˻�"(decay)Ϊָ������Ԫ�����͵�ָ��
 * ���統������Ϊ�βε�����ʱ��
 * atop����ʵ������"�˻�".
 */
Type atop(Type ty) {
	if (isarray(ty))
		return ptr(ty->type);
	error("type error: %s\n", "array expected");
	return ptr(ty);
}
/**
 * qual�ֱ����޶����͡�
 * �����޶�����ty,qual���Ƿ��Ĳ�������
 * ����(CONST ty),(VOLATILE ty)��(CONST+VOLATILE ty).
 * ���ty������(ARRAY ety),���޶�������������Ԫ�����ͣ�
 * ��ôqual(op,ty)�ͻᴴ��(ARRAY(op,ety))��
 * ���ty�Ѿ����޶��ģ�����(CONST ty->type)��(VOLATILE ty->type),
 * ����op����һ���޶�������ôqual����(CONST+VOLATILE ty->type).
 * ����ֻʹ��һ�����ͽڵ������һ���������ͽڵ��������޶�����
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