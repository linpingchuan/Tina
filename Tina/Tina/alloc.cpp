#include "c.h"
// 小于 avail 都是已经被分配的区域
// [avail,limit] 这段区间，都是继续可分配空间；
// next 指向链表的下一块
struct block
{
	struct block *next;
	char *limit;
	char *avail;
};
// 联合align给出了宿主机上最小的对齐字节数。
// align的成员规定了最严格的对齐要求
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
// 如果PURIFY已定义，则采用malloc和free方式进行内存分配，这种方式方便查错
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
first[] = { {NULL},{NULL},{NULL} },
*arena[] = { &first[0],&first[1],&first[2] };

static struct block *freeblocks;

// 空间分配:按照内存边界对其原则确定分配空间大小，
// 据此增加avail指针值，并返回该指针原来的值。
// 与malloc类似，allocate必须返回一个对齐后的指针，
// 指针指向的空间可以存放任何类型的值，

void *allocate(unsigned long n, unsigned a) {
	struct block *ap;

	assert(a < NELEMS(arena));
	assert(n > 0);

	ap = arena[a];
	n = roundup(n, sizeof(union align));
	// while循环不断执行，
	// 直到ap所指的块至少有n个字节的可分配空间。
	// 大多数调用allocate的情况，
	// 这个块就是由allocate的第二个参数所指的分配区的指向的块
	while (ap->avail + n > ap->limit) {
		if ((ap->next = freeblocks) != NULL) {
			freeblocks = freeblocks->next;
			ap = ap->next;
		}
		else {
			// 新增的内存块，除了包括块的头、n个字节的需要分配的空间
			// 还有10KB的空闲待分配的空间
			unsigned m = sizeof(union header) + n + roundup(10 * 1024, sizeof(union align));
			ap->next = (struct block*) malloc(m);
			ap = ap->next;
			if (ap == NULL) {
				error("insufficient memory\n");
				exit(1);
			}
			ap->limit = (char*)ap + m;
		}
		// 联合header保证ap->avail总是指向一个对齐后的地址
		ap->avail = (char*)((union header*)ap + 1);
		ap->next = NULL;
		arena[a] = ap;
	}
	ap->avail += n;
	return ap->avail - n;
}

void *newarray(unsigned long m, unsigned long n, unsigned a) {
	return allocate(m*n, a);
}

// 一个空闲区的释放时，把它所有的内存块都加入空闲块列表中
// 自己重新初始化成指向单元素列表，而该元素的内存块长度为0
// 由于被释放的内存块已经通过其next指针连成了一个链
// 所以只要通过简单的指针操作就可以把整个链中的的所有块都加入freeblock中
void deallocate(unsigned a) {
	assert(a < NELEMS(arena));
	arena[a]->next = freeblocks;
	freeblocks = first[a].next;
	first[a].next = NULL;
	arena[a] = &first[a];
}
#endif
