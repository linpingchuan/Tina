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

int level = GLOBAL;
static int tempid;
List loci, symbols;
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
/**
 * while循环查找与作用域对应的表，
 * 如果找到了，foreach函数就把每个符号的定义位置保存在全局变量src中，
 * 并为符号调用apply函数。
 * cl是一个指针，它指向与调用相关的数据closure，该数据由foreach的调用者提供，
 * 如果有需要，这些数据将传递apply函数以便其访问。
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
		* for循环遍历表的all链表，
		* 直到链表或遇到较小层数作用域中的符号.
		* 从严格意义上来说，all并不是必须的，因为foreach可以遍历哈希表.
		* 按照与哈希地址无关的顺序为每个符号调用apply，会使产生的代码的顺序与机器无关
		*/
		for (p = tp->all;p&&p->scope == lev;p = p->up) {
			src = p->src;
			(*apply)(p, cl);
		}
		src = sav;
	}
}

/**
 * 进入一个新的作用域，level将递增
 */
void enterscope() {
	if (++level == LOCAL)
		tempid = 0;
}

/**
 * 退出作用域，level将递减，相应的identifiers和types表也随着撤销
 */
void exitscoope() {
	if (types->level == level)
		types = types->previous;
	if (identifiers->level == level) {

		identifiers = identifiers->previous;
	}
	--level;
}