#include "c.h"
/**
 * List保存了0或多个元素，每个元素存放在list结构的x域中。
 * List指向列表中最后一个list结构，空的List定义为空列表。
 */

static List freenodes;

// 把包含x的节点放入list列表的末尾并返回list
List append(void *x, List plist) {
	List _new;
	if ((_new = freenodes) != NULL)
		freenodes = freenodes->link;
	else
		_new =(List) allocate(sizeof*(_new), PERM);
	if (plist) {
		_new->link = plist->link;
		plist->link = _new;
	}
	else {
		_new->link = _new;
	}
	return _new;
}

// 返回列表中元素的数目
int length(List plist) {
	int n = 0;
	if (plist) {
		List lp = plist;
		do 
			n++;
		while ((lp = lp->link) != plist);
	}
	return n;
}

// 把list中的n个元素复制到a所指的分配区中以空元素结尾的指针数组
// 释放列表结构并返回该数组
void *ltov(List *plist, unsigned arena) {
	int i = 0;
	void **_array =(void **) newarray(length(*plist) + 1, sizeof _array[0], arena);

	if (*plist) {
		List lp = *plist;
		do {
			lp = lp->link;
			_array[i++] = lp->x;
		} while (lp != *plist);
#ifndef PURIFY
		lp = (*plist)->link;
		(*plist)->link = freenodes;
		freenodes = lp;
#endif
	}
	*plist = NULL;
	_array[i] = NULL;
	return _array;
}