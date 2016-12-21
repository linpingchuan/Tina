#include "c.h"

struct block
{
	// 小于 avail 都是已经被分配的区域
	// [avail,limit] 这段区间，都是继续可分配空间；
	// next 指向链表的下一块
	struct block *next;
	char *limit; 
	char *avail;
};

union align {
	long l;
	char *p;
	double d;
	int(*f)(void);
};

union header {
	struct block b;
	union align a;
};
#ifdef PURIFY
// arena : 分配区
union header *arena[3]; 

void *allocate(unsigned long n, unsigned a) {
	union header *_new = malloc(sizeof *_new + n);

	assert(a < NELEMS(arena));

	if (_new == NULL) {
		error("insufficent memory\n");
		exit(1);
	}

	_new->b.next = (void *)arena[a];
	arena[a] = _new;
	return _new + 1;
}

void deallocate(unsigned a) {
	union header *p, *q;
	assert(a < NELEMS(arena));
	for (p = arena[a];p;p = q) {
		q = (void*)p->b.next;
		free(p);
	}
	free(p);
	arena[a] = NULL;
}

viod *newarray(unsigned long m, unsigned long n, unsigned a) {
	return allocate(m*n, a);
}
#else
static struct block
first[] = { {NULL},{NULL},{NULL} };
*arena[] = { &first[0],&first[1],&first[2] };
#endif
