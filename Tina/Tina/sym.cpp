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

int level = GLOBAL;
static int tempid;
List loci, symbols;
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
/**
 * whileѭ���������������Ӧ�ı�
 * ����ҵ��ˣ�foreach�����Ͱ�ÿ�����ŵĶ���λ�ñ�����ȫ�ֱ���src�У�
 * ��Ϊ���ŵ���apply������
 * cl��һ��ָ�룬��ָ���������ص�����closure����������foreach�ĵ������ṩ��
 * �������Ҫ����Щ���ݽ�����apply�����Ա�����ʡ�
 * 
 */
void foreach(Table tp, int lev, void(*apply)(Symbol, void *), void *cl) {
	assert(tp);
	while (tp&&tp->level > lev)
		tp = tp->previous;
	if (tp&&tp->level == lev) {
		Symbol p;
		Coordinate sav;
		sav=src;
		/**
		* forѭ���������all����
		* ֱ�������������С�����������еķ���.
		* ���ϸ���������˵��all�����Ǳ���ģ���Ϊforeach���Ա�����ϣ��.
		* �������ϣ��ַ�޹ص�˳��Ϊÿ�����ŵ���apply����ʹ�����Ĵ����˳��������޹�
		*/
		for (p = tp->all;p&&p->scope == lev;p = p->up) {
			src = p->src;
			(*apply)(p, cl);
		}
		src = sav;
	}
}

/**
 * ����һ���µ�������level������
 */
void enterscope() {
	if (++level == LOCAL)
		tempid = 0;
}

/**
 * �˳�������level���ݼ�����Ӧ��identifiers��types��Ҳ���ų���
 */
void exitscoope() {
	if (types->level == level)
		types = types->previous;
	if (identifiers->level == level) {

		identifiers = identifiers->previous;
	}
	--level;
}