#include "c.h"
/**
 * List������0����Ԫ�أ�ÿ��Ԫ�ش����list�ṹ��x���С�
 * Listָ���б������һ��list�ṹ���յ�List����Ϊ���б�
 */

static List freenodes;

// �Ѱ���x�Ľڵ����list�б��ĩβ������list
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

// �����б���Ԫ�ص���Ŀ
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

// ��list�е�n��Ԫ�ظ��Ƶ�a��ָ�ķ��������Կ�Ԫ�ؽ�β��ָ������
// �ͷ��б�ṹ�����ظ�����
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