#include "c.h"
#include <stdio.h>

static char rcsid[] = "$Id$";

#define equalp(x) v.x== p->sym.u.c.v.x
struct entry {
	struct symbol sym;
	struct entry *link;
};
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
	struct entry *buckets[256];
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
/**
 * loci和symbols分别保存了指向Coordinate和Symbol的指针。
 * symbols表中每个入口是一个链表的尾，
 * 该链表由在loci对应的源代码位置上的可见的符号组成。
 * 从入口的符号出发，通过up域可以访问所有在该点可见的符号。
 */
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
		sav = src;
		/**
		* for循环遍历表的all链表，
		* 直到链表或遇到较小层数作用域中的符号.
		* 从严格意义上来说，all并不是必须的，因为foreach可以遍历哈希表.
		* 按照与哈希地址无关的顺序为每个符号调用apply，会使产生的代码的顺序与机器无关
		*/
		for (p = tp->all; p&&p->scope == lev; p = p->up) {
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
void exitscope() {
	// rmtypes将从其类型缓冲中删除
	rmtypes(level);
	if (types->level == level)
		types = types->previous;
	if (identifiers->level == level) {

		identifiers = identifiers->previous;
	}
	--level;
}

/**
 * install函数为给定的name分配一个符号，
 * 并把该符号加入与给定作用域层数相对应的表中。
 * 如果需要，还将建立一个新表。
 * 该函数返回一个指向符号的指针。
 * name存放了字符串，根据其地址可以计算它的哈希值。
 * tpp是一个指向表的指针。
 * 如果*tpp指向某作用域的表,如identifiers，
 * 并且目前没有与参数level给定的作用域相对应的表.
 * 则install将先为参数level给定的作用域分配一个表。
 * 并更新*tpp;
 * 然后 install分配一个入口，将该项清零，
 * 最后初始化符号的某些域，并把该入口加入哈希链表中。
 * level必须为0，或不小于该表的作用域层数。
 * 如果level为0，则表示name应该建立在*tpp表示的表格中。
 * install接受一个指明相应分配区的参数，如果有函数原型，
 * 则使其中的符号可以永久保存，即使它们是在内层作用域中声明的。
 */
Symbol install(char *name, Table *tpp, int level, int arena) {
	Table tp = *tpp;
	struct entry *p;
	unsigned h = (unsigned)name*(HASHSIZE - 1);
	assert(level == 0 || level >= tp->level);

	if (level > 0 && tp->level < level)
		tp = *tpp = table(tp, level);
	p = (struct entry*)memset(allocate(sizeof *(p), arena), 0, sizeof *(p));
	p->sym.name = name;
	p->sym.scope = level;
	p->sym.up = tp->all;
	tp->all = &p->sym;
	p->link = tp->buckets[h];
	tp->buckets[h] = p;
	return &p->sym;
}
/**
 * lookup函数实现在表中查找一个名字，查找的关键字是符号的name域。
 * 如果找到了，该函数将返回一个指向符号的指针，否则返回空指针。
 * 在代码中，内层循环扫描哈希链，外层循环扫描外层作用域。
 * 字符串模块保证当且仅当两个字符串完全相同是，
 * 它们才是同一个副本，所以字符串的比较非常简单。
 */
Symbol lookup(char *name, Table tp) {
	struct entry *p;
	unsigned h = (unsigned)name&(HASHSIZE - 1);
	do
		for (p = tp->buckets[h]; p; p = p->link)
			if (name == p->sym.name)
				return &p->sym;
	while ((tp = tp->previous) != NULL);
	return NULL;
}

/**
 * 符号表模块还提供了管理标号和常量的函数。
 * 这些管理函数与lookup和install 相似，但是不涉及作用域管理。
 * 查找标号和常量时，如果有必要，就会建立这些标号和常量，因此查找总会成功。
 * 查找的关键字是联合u中标号和常量特有的域。
 * 编译器产生标号和源程序中的标号的内部表示都采取整数。
 * 函数genlabel通过累加计数器产生一个整数.
 * genlabel也可以用于产生唯一的，匿名的名字，如产生一个临时变量的名字。
 */
int genlabel(int n) {
	static int label = 1;
	label += n;
	return label - n;
}
/**
 * 源程序中的每个标号都有相应的一个内部标号，
 * 这些内部标号和编译器产生的其他标号都保存在label表中。
 * 对于每个函数都会建立一个这样的表，并有findlabel函数进行管理。
 * findlabel函数的输入参数是一个标号数，并返回该标号对应的符号。
 * 如果需要，则会建立该符号，进行初始化并通知编译后端。
 * generated是一位二进制位域<symbol flags>，表示一个产生的符号.
 * 对于产生的这些符号名字，某些编译后端可以利用特殊的格式以避免表在连接上的混乱。
 */
Symbol findlabel(int lab) {
	struct entry *p;
	unsigned h = lab&(HASHSIZE - 1);
	for (p = labels->buckets[h]; p; p = p->link)
		if (lab == p->sym.u.l.label)
			return &p->sym;
	p = (struct entry *)memset(allocate(sizeof*(p), FUNC), 0, sizeof*(p));
	p->sym.name = stringd(lab);
	p->sym.scope = LABELS;
	p->sym.up = labels->all;
	labels->all = &p->sym;
	p->link = labels->buckets[h];
	labels->buckets[h] = p;
	p->sym.generated = 1;
	p->sym.u.l.label = lab;
	(*IR->defsymbol)(&p->sym);
	return &p->sym;
}
/**
 * 对于每个常量来说，constants中只保存了一个实例。
 * 例如，"hello world" 在程序中出现了三次，
 * 所有这三次的引用都指向了表中的同一个符号。
 * constants函数实现了在常量表中查找给定类型和值的常量，
 * 如果需要，将在表中增加该常量，函数返回一个指向符号的指针。
 * 常量不会从表中删除。
 */
Symbol constant(Type ty, Value v) {
	struct entry *p;
	unsigned h = v.u&(HASHSIZE - 1);
	static union {
		int x;
		// 字节存储次序
		char endian;
	}little = { 1 };
	
	ty = unqual(ty);
	/**
	 * 如果v出现在表中，则返回指向其符号的指针；
	 * 否则，在表中新增一个符号并初始化。
	 */
	for (p = constants->buckets[h]; p; p = p->link)
		if (eqtype(ty, p->sym.type, 1))
			switch (ty->op) {
			case INT:
				if (equalp(i))
					return &p->sym;
				break;
			case UNSIGNED:
				if (equalp(u))
					return &p->sym;
				break;
			case FLOAT:
				if (v.d == 0.0) {
					float z1 = v.d, z2 = p->sym.u.c.v.d;
					char *b1 = (char *)&z1, *b2 = (char *)&z2;
					if (z1 == z2
						&& (!little.endian&&b1[0] == b2[0]
							|| little.endian&&b1[sizeof(z1) - 1] == b2[sizeof(z2) - 1]))
						return &p->sym;
				}
				else if (equalp(d))
					return &p->sym;
				break;
			case FUNCTION:
				if (equalp(g))
					return &p->sym;
				break;
			case ARRAY:
			case POINTER:
				if (equalp(p))
					return &p->sym;
				break;
			default:
				assert(0);
			}

	p = (struct entry *)memset(allocate(sizeof*(p), FUNC), 0, sizeof*(p));
	// name域存放了vtoa函数返回的常量的字符串的表示。
	p->sym.name = vtoa(ty, v);
	p->sym.scope = CONSTANTS;
	p->sym.type = ty;
	p->sym.sclass = STATIC;
	p->sym.u.c.v = v;
	p->link = constants->buckets[h];
	p->sym.up = constants->all;
	constants->all = &p->sym;
	constants->buckets[h] = p;
	/**
	 * constant调用defsymbol，通知编译后端这些常量可以出现在dag（无环有向图中）
	 * 对于基本类型，如整型和浮点类型，只有对addressed标志进行了测试，lcc完成这些配置后才出现dag中。
	 */
	if (ty->u.sym && !ty->u.sym->addressed)
		(*IR->defsymbol)(&p->sym);
	p->sym.defined = 1;
	return &p->sym;
}
/**
 * vtoa返回的值只对整型和指向常量的指针有。
 * 对于其他类型，返回的字符串并不能可靠地描述对应的数值。
 * 常量的查找是基于它们实际的值，
 * 而不是它们的字符串表示，因为有些浮点数不能直接自然地表示成字符串。
 * 例如，常量表达式(double)(float)0.3可以将0.3截尾成一个与机器相关的值
 * 这种效果就不能用有效的字符串表示出来。
 */
char *vtoa(Type ty, Value v) {
	ty = unqual(ty);
	switch (ty->op) {
	case INT: return stringd(v.i);
	case UNSIGNED:return stringf((v.u&~0x7FFF) ? "0x%X" : "%U", v.u);
	case FLOAT:return stringf("%g", (double)v.d);
	case ARRAY:
		if (ty->type == chartype || ty->type == signedchar || ty->type == unsignedchar)
			return (char *)v.p;
		return stringf("%p", v.p);
	case POINTER:return stringf("%p", v.p);
	case FUNCTION:return stringf("%p", v.g);
	}
	assert(0);
	return NULL;
}
/**
 * 在编译前端和后端中都有大量的整形常量。
 * intconst封装了建立和通知整形常量的功能
 */
Symbol intconst(int n) {
	Value v;
	v.i = n;
	return constant(inttype, v);
}
/**
 * 编译的前端会为各种目的产生局部变量。
 * 例如，可以产生静态的变量，用于保存超出一行的变量（如字符串）和switch语句的跳转表等，
 * 也可以产生一些局部变量用于函数传递和返回结构，以及保存条件表达式的结果和switch语句的测试值。
 * genident根据给定的类型、存储类别和作用域，产生一个标识符并初始化。
 * 其中name是由数字组成的字符串，generated标志被设置为1.
 * 参数和局部变量在其他地方通知编译后端，该数据结构连接前端和某个具体的后端
 */
Symbol genident(int scls, Type ty, int lev) {
	Symbol p;
	p = (Symbol)memset(allocate(sizeof*(p), lev >= LOCAL ? FUNC : PERM), 0, sizeof*(p));
	p->name = stringd(genlabel(1));
	p->scope = lev;
	p->sclass = scls;
	p->type = ty;
	p->generated = 1;
	if (lev == GLOBAL)
		(*IR->defsymbol)(p);
	return p;
}
/**
 * 临时变量是另外一类产生的变量，
 * 它们都具有temporary标志；
 * 
 */
Symbol temporary(int scls, Type ty, int lev) {
	Symbol p;
	p = (Symbol)memset(allocate(sizeof*(p), FUNC), 0, sizeof*(p));
	p->name = stringd(++tempid);
	p->scope = level < LOCAL ? LOCAL : level;
	p->sclass = scls;
	p->type = ty;
	p->temporary = 1;
	p->generated = 1;
	return p;
}
Symbol temporary(int scls, Type ty) {
	Symbol p;
	p = (Symbol)memset(allocate(sizeof*(p), FUNC), 0, sizeof*(p));
	p->name = stringd(++tempid);
	p->scope = level < LOCAL ? LOCAL : level;
	p->sclass = scls;
	p->type = ty;
	p->temporary = 1;
	p->generated = 1;
	return p;
}
/**
 *编译后端有时也需要临时变量，
 * 比如为了腾空寄存器，后端由于不知道类型系统，
 * 所以不能直接调用temporary函数。
 * newtemp接受一个类型后端，
 * 通过调用btot将该后缀映射为相应的类型，
 * 再利用该类型调用temporary.
 * 调用newtemp发生在代码生成的时候，
 * 如果像前端的临时变量那样进行通知，
 * 则为时已晚。
 * 因此，newtemp调用local来通告他们。
 * 标志defined在通知完后端之后被置成1.
 */
Symbol newtemp(int sclass, int tc, int size) {
	Symbol p = temporary(sclass, btot(tc, size));
	(*IR->local)(p);
	p->defined = 1;
	return p;
}
/**
 * -x选项使lcc把每个符号use域置成一个指向Coordinate的指针列表,
 * 表示该符号的引用情况。
 */
void use(Symbol p, Coordinate src) {
	Coordinate *cp;
	cp = (Coordinate *)allocate(sizeof*(cp), PERM);
	*cp = src;
	p->uses = append(cp, p->uses);
}

Symbol allsymbols(Table tp) {
	return tp->all;
}
/**
 * locus函数把tp->all和cp加入symbols和loci中。
 * tp->all指向最近加入*tp表的符号，即当前可见符号列表。
 */
void locus(Table tp, Coordinate *cp) {
	loci = append(cp, loci);
	symbols = append(allsymbols(tp), symbols);
}