#include "c.h"
#include <stdio.h>

static char rcsid[] = "$Id$";

#define equalp(x) v.x== p->sym.u.c.v.x

struct _table {
	// level域的值指明作用域
	int level;

	// previous域指向外层作用域对应的table
	Table previous;

	// buckets域是一个指针数组，每个指针指向哈希链
	// 哈希链的每个入口保存了一个symbol结构和一个指向链中下一入口的指针
	// 如果要查找一个符号，
	// 则根据关键字计算哈希函数值，找到相应的哈希链，
	// 然后通过遍历该链找到相应的符号；
	// 如果未发现该符号，则通过previous域在外层作用域的入口中进行查找
	struct entry {
		struct symbol sym;
		struct entry *link;
	}*buckets[256];
	// 在每个表结构中，all域指向由当前及其外层作用域中所有符号组成的列表的头
	// 该列表是通过symbol的up域连接起来的
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
// 指向当前作用域的表
Table identifiers = &ids;
// 指向作用域为GLOBAL的标识符表
Table globals = &ids;
Table types = &tys;
Table labels;

/**
 * 内层嵌套作用域的表都是动态创建的，并且与相应外层的表进行链接。
 * 函数被编译完成后，所有动态分配的表都被释放，因此，动态表是在FUNC分配区中进行分配的
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