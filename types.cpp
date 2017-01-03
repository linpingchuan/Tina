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