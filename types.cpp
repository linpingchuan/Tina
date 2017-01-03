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

Type chartype;
Type doubletype;
Type floattype;
Type inttype;
Type longdoble;
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
Type voidttype;
Type unsignedptr;
Type signedptr;
Type widechar;

void rmtypes(int lev) {

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
	tn =(struct entry*) memset(allocate(sizeof*(tn), PERM), 0, sizeof*(tn));
	tn->type.op = op;
	tn->type.type = ty;
	tn->type.size = size;
	tn->type.u.sym =(Symbol) sym;
	tn->link = typetable[h];
	typetable[h] = tn;
	return &tn->type;
}

static Type xxinit(int op, char *name, Metrics m) {
	Symbol p = install(string(name), &types, GLOBAL, PERM);
	Type ty = type(op, 0, m.size, m.align, p);

	assert(ty->align == 0 || ty->size%ty->align == 0);
	p->type = ty;
	p->addressed = m.outoflines;
	switch (ty->op) {
	case INT:
		break;
	default:assert(0);
	}
	return ty;
}

/** typetable �ڳ�ʼ��ʱ��ֻ���й������ͺ�void*����
 * ǰ��ʹ����Щ���������ض����ͣ�����Ϊ��֪���ڵ���������typetable.
 * type_init������ʼ����Щȫ�ֱ�����typetable
 */
void type_init(int argc,char *argv[]) {

}
/**
 * type�������Թ����������ͣ�����������װ�˶�type�ĵ��á�
 */

// ptr��������ָ������
Type ptr(Type ty) {
	return type(POINTER, ty, IR->ptrmetric.size, IR->ptrmetric.align, pointersym);
}