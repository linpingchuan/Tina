#include "c.h"
#include <stdio.h>

static char rcsid[] = "$Id$";

#define equalp(x) v.x== p->sym.u.c.v.x

struct _table {
	// level���ֵָ��������
	int level;

	// previous��ָ������������Ӧ��table
	Table previous;

	// buckets����һ��ָ�����飬ÿ��ָ��ָ���ϣ��
	// ��ϣ����ÿ����ڱ�����һ��symbol�ṹ��һ��ָ��������һ��ڵ�ָ��
	// ���Ҫ����һ�����ţ�
	// ����ݹؼ��ּ����ϣ����ֵ���ҵ���Ӧ�Ĺ�ϣ����
	// Ȼ��ͨ�����������ҵ���Ӧ�ķ��ţ�
	// ���δ���ָ÷��ţ���ͨ��previous������������������н��в���
	struct entry {
		struct symbol sym;
		struct entry *link;
	}*buckets[256];
	// ��ÿ����ṹ�У�all��ָ���ɵ�ǰ������������������з�����ɵ��б��ͷ
	// ���б���ͨ��symbol��up������������
	Symbol all;
};

#define HASHSIZE NELEMS(((Table)0)->buckets)

static struct _table
cns = { CONSTANTS },
ext = { GLOBAL },
ids = { GLOBAL },
tys = { GLOBAL };
Table constants = &cns;
Table externals = &ext;
// ָ��ǰ������ı�
Table identifiers = &ids;
// ָ��������ΪGLOBAL�ı�ʶ����
Table globals = &ids;
Table types = &tys;
Table labels;

/**
 * �ڲ�Ƕ��������ı��Ƕ�̬�����ģ���������Ӧ���ı�������ӡ�
 * ������������ɺ����ж�̬����ı����ͷţ���ˣ���̬������FUNC�������н��з����
 * 
 */
Table table(Table tp, int level) {
	Table _new;
	_new = (Table)memset(allocate(sizeof *(_new), FUNC), 0, sizeof *(_new));
	_new->previous = tp;
	_new->level = level;
	if (tp)
		_new->all = tp->all;
	return _new;
}

void foreach(Table tp, int lev, void(*apply)(Symbol, void *), void *cl) {

}