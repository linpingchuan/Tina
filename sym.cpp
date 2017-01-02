#include "c.h"
#include <stdio.h>

static char rcsid[] = "$Id$";

#define equalp(x) v.x== p->sym.u.c.v.x
struct entry {
	struct symbol sym;
	struct entry *link;
};
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
	struct entry *buckets[256];
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
/**
 * loci��symbols�ֱ𱣴���ָ��Coordinate��Symbol��ָ�롣
 * symbols����ÿ�������һ�������β��
 * ����������loci��Ӧ��Դ����λ���ϵĿɼ��ķ�����ɡ�
 * ����ڵķ��ų�����ͨ��up����Է��������ڸõ�ɼ��ķ��š�
 */
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
		sav = src;
		/**
		* forѭ���������all����
		* ֱ�������������С�����������еķ���.
		* ���ϸ���������˵��all�����Ǳ���ģ���Ϊforeach���Ա�����ϣ��.
		* �������ϣ��ַ�޹ص�˳��Ϊÿ�����ŵ���apply����ʹ�����Ĵ����˳��������޹�
		*/
		for (p = tp->all; p&&p->scope == lev; p = p->up) {
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
void exitscope() {
	// rmtypes���������ͻ�����ɾ��
	rmtypes(level);
	if (types->level == level)
		types = types->previous;
	if (identifiers->level == level) {

		identifiers = identifiers->previous;
	}
	--level;
}

/**
 * install����Ϊ������name����һ�����ţ�
 * ���Ѹ÷��ż��������������������Ӧ�ı��С�
 * �����Ҫ����������һ���±�
 * �ú�������һ��ָ����ŵ�ָ�롣
 * name������ַ������������ַ���Լ������Ĺ�ϣֵ��
 * tpp��һ��ָ����ָ�롣
 * ���*tppָ��ĳ������ı�,��identifiers��
 * ����Ŀǰû�������level���������������Ӧ�ı�.
 * ��install����Ϊ����level���������������һ����
 * ������*tpp;
 * Ȼ�� install����һ����ڣ����������㣬
 * ����ʼ�����ŵ�ĳЩ�򣬲��Ѹ���ڼ����ϣ�����С�
 * level����Ϊ0����С�ڸñ�������������
 * ���levelΪ0�����ʾnameӦ�ý�����*tpp��ʾ�ı���С�
 * install����һ��ָ����Ӧ�������Ĳ���������к���ԭ�ͣ�
 * ��ʹ���еķ��ſ������ñ��棬��ʹ���������ڲ��������������ġ�
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
 * lookup����ʵ���ڱ��в���һ�����֣����ҵĹؼ����Ƿ��ŵ�name��
 * ����ҵ��ˣ��ú���������һ��ָ����ŵ�ָ�룬���򷵻ؿ�ָ�롣
 * �ڴ����У��ڲ�ѭ��ɨ���ϣ�������ѭ��ɨ�����������
 * �ַ���ģ�鱣֤���ҽ��������ַ�����ȫ��ͬ�ǣ�
 * ���ǲ���ͬһ�������������ַ����ıȽϷǳ��򵥡�
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
 * ���ű�ģ�黹�ṩ�˹����źͳ����ĺ�����
 * ��Щ��������lookup��install ���ƣ����ǲ��漰���������
 * ���ұ�źͳ���ʱ������б�Ҫ���ͻὨ����Щ��źͳ�������˲����ܻ�ɹ���
 * ���ҵĹؼ���������u�б�źͳ������е���
 * ������������ź�Դ�����еı�ŵ��ڲ���ʾ����ȡ������
 * ����genlabelͨ���ۼӼ���������һ������.
 * genlabelҲ�������ڲ���Ψһ�ģ����������֣������һ����ʱ���������֡�
 */
int genlabel(int n) {
	static int label = 1;
	label += n;
	return label - n;
}
/**
 * Դ�����е�ÿ����Ŷ�����Ӧ��һ���ڲ���ţ�
 * ��Щ�ڲ���źͱ�����������������Ŷ�������label���С�
 * ����ÿ���������Ὠ��һ�������ı�����findlabel�������й���
 * findlabel���������������һ��������������ظñ�Ŷ�Ӧ�ķ��š�
 * �����Ҫ����Ὠ���÷��ţ����г�ʼ����֪ͨ�����ˡ�
 * generated��һλ������λ��<symbol flags>����ʾһ�������ķ���.
 * ���ڲ�������Щ�������֣�ĳЩ�����˿�����������ĸ�ʽ�Ա�����������ϵĻ��ҡ�
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
 * ����ÿ��������˵��constants��ֻ������һ��ʵ����
 * ���磬"hello world" �ڳ����г��������Σ�
 * ���������ε����ö�ָ���˱��е�ͬһ�����š�
 * constants����ʵ�����ڳ������в��Ҹ������ͺ�ֵ�ĳ�����
 * �����Ҫ�����ڱ������Ӹó�������������һ��ָ����ŵ�ָ�롣
 * ��������ӱ���ɾ����
 */
Symbol constant(Type ty, Value v) {
	struct entry *p;
	unsigned h = v.u&(HASHSIZE - 1);
	static union {
		int x;
		// �ֽڴ洢����
		char endian;
	}little = { 1 };
	
	ty = unqual(ty);
	/**
	 * ���v�����ڱ��У��򷵻�ָ������ŵ�ָ�룻
	 * �����ڱ�������һ�����Ų���ʼ����
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
	// name������vtoa�������صĳ������ַ����ı�ʾ��
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
	 * constant����defsymbol��֪ͨ��������Щ�������Գ�����dag���޻�����ͼ�У�
	 * ���ڻ������ͣ������ͺ͸������ͣ�ֻ�ж�addressed��־�����˲��ԣ�lcc�����Щ���ú�ų���dag�С�
	 */
	if (ty->u.sym && !ty->u.sym->addressed)
		(*IR->defsymbol)(&p->sym);
	p->sym.defined = 1;
	return &p->sym;
}
/**
 * vtoa���ص�ֵֻ�����ͺ�ָ������ָ���С�
 * �����������ͣ����ص��ַ��������ܿɿ���������Ӧ����ֵ��
 * �����Ĳ����ǻ�������ʵ�ʵ�ֵ��
 * ���������ǵ��ַ�����ʾ����Ϊ��Щ����������ֱ����Ȼ�ر�ʾ���ַ�����
 * ���磬�������ʽ(double)(float)0.3���Խ�0.3��β��һ���������ص�ֵ
 * ����Ч���Ͳ�������Ч���ַ�����ʾ������
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
 * �ڱ���ǰ�˺ͺ���ж��д��������γ�����
 * intconst��װ�˽�����֪ͨ���γ����Ĺ���
 */
Symbol intconst(int n) {
	Value v;
	v.i = n;
	return constant(inttype, v);
}
/**
 * �����ǰ�˻�Ϊ����Ŀ�Ĳ����ֲ�������
 * ���磬���Բ�����̬�ı��������ڱ��泬��һ�еı��������ַ�������switch������ת��ȣ�
 * Ҳ���Բ���һЩ�ֲ��������ں������ݺͷ��ؽṹ���Լ������������ʽ�Ľ����switch���Ĳ���ֵ��
 * genident���ݸ��������͡��洢���������򣬲���һ����ʶ������ʼ����
 * ����name����������ɵ��ַ�����generated��־������Ϊ1.
 * �����;ֲ������������ط�֪ͨ�����ˣ������ݽṹ����ǰ�˺�ĳ������ĺ��
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
 * ��ʱ����������һ������ı�����
 * ���Ƕ�����temporary��־��
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
 *��������ʱҲ��Ҫ��ʱ������
 * ����Ϊ���ڿռĴ�����������ڲ�֪������ϵͳ��
 * ���Բ���ֱ�ӵ���temporary������
 * newtemp����һ�����ͺ�ˣ�
 * ͨ������btot���ú�׺ӳ��Ϊ��Ӧ�����ͣ�
 * �����ø����͵���temporary.
 * ����newtemp�����ڴ������ɵ�ʱ��
 * �����ǰ�˵���ʱ������������֪ͨ��
 * ��Ϊʱ����
 * ��ˣ�newtemp����local��ͨ�����ǡ�
 * ��־defined��֪ͨ����֮���ó�1.
 */
Symbol newtemp(int sclass, int tc, int size) {
	Symbol p = temporary(sclass, btot(tc, size));
	(*IR->local)(p);
	p->defined = 1;
	return p;
}
/**
 * -xѡ��ʹlcc��ÿ������use���ó�һ��ָ��Coordinate��ָ���б�,
 * ��ʾ�÷��ŵ����������
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
 * locus������tp->all��cp����symbols��loci�С�
 * tp->allָ���������*tp��ķ��ţ�����ǰ�ɼ������б�
 */
void locus(Table tp, Coordinate *cp) {
	loci = append(cp, loci);
	symbols = append(allsymbols(tp), symbols);
}