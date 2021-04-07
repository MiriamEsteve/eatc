#include <stdio.h>
#include <stdlib.h>

#include "vlist.h"

/////////////////////////////////////////////////////////////
// Funciones internas de la libreria (no incluidas en el .h)
/////////////////////////////////////////////////////////////

// Crea un nuevo 'VoidListNode' incorporando el nuevo 'data'
//   copy=1 -> hace copia de 'data'
//   copy=0 -> toma la referencia de 'data'
VLN *newVLN(void *data, void* (*listNew)(void*), int copy)
{
	VLN *n;

	if(!(n = malloc(sizeof(VLN))))
		return NULL;
	n->front=n->back=NULL;
	if(copy)
	{
		if(!(n->data = (*listNew)(data)))
			return NULL;
	}
	else
		n->data = data;
	return n;		
}

// Crea un nuevo 'VoidListNode' al principio de la lista
int newFirstVLN(VLIST *l, void *data, int copy)
{
	if(l->size==0)
	{
		if(!(l->first = l->last = newVLN(data,l->thisNew,copy)))
			return 0;
	}
	else
	{
		if(!(l->first->back = newVLN(data,l->thisNew,copy)))
			return 0;
		l->first->back->front = l->first;
		l->first = l->first->back;
	}
	l->size++;
	return 1;
}

// Crea un nuevo 'VoidListNode' al final de la lista
int newLastVLN(VLIST *l, void *data, int copy)
{
	if(l->size==0)
	{
		if(!(l->first = l->last = newVLN(data,l->thisNew,copy)))
			return 0;
	}
	else
	{
		if(!(l->last->front = newVLN(data,l->thisNew,copy)))
			return 0;
		l->last->front->back = l->last;
		l->last = l->last->front;
	}
	l->size++;
	return 1;
}

// Crea un nuevo 'VoidListNode' a la derecha (front) de 'pos'
int newPositionVLN(VLIST *l, VLN *p, void *data, int copy)
{
	VLN *n;
	
	if(!(n = newVLN(data,l->thisNew,copy)))
		return 0;
	n->back = p;
	n->front = p->front;
	p->front->back = n;
	p->front = n;
	l->size++;
	return 1;
}

// Eliminar el 'VoidListNode' apuntado 'pos' 
void delPositionVLN(VLIST *l, VLN *p)
{
	if(p==l->first)
		l->first = p->front;
	if(p==l->last)
		l->last = p->back;
	if(p->back)
		p->back->front = p->front;
	if(p->front)
		p->front->back = p->back;
	if(p->data)
		l->thisDel(p->data);

	free(p);
	l->size--;
}


/////////////////////////////////////////////////////////////
// Funciones publicas
/////////////////////////////////////////////////////////////
void listErr(char *err)
{
	fprintf(stderr, "  ## LIST-ERROR >> %s\n", err);
	getchar();
	exit(0);
}

void listIni(VLIST *l)
{
	l->first = l->last = NULL;
	l->thisPrt = l->thisDel = NULL;
	l->thisNew = NULL;
	l->size = 0;
}

void listIniAll(VLIST *l, void  (*ptrFunc)(void*), void* (*newFunc)(void*), void (*delFunc)(void*), void (*cpyFunc)(void*, void*))
{
	l->first = l->last = NULL;
	l->thisPrt = ptrFunc;
	l->thisNew = newFunc;
	l->thisDel = delFunc;
	l->thisCpy = cpyFunc;
	l->size = 0;
}

void listIniPrt(VLIST *l, void (*ptrFunc)(void*))
{
	l->thisPrt = ptrFunc;
}

void listIniNew(VLIST *l, void* (*newFunc)(void*))
{
	l->thisNew = newFunc;
}

void listIniDel(VLIST *l, void (*delFunc)(void*))
{
	l->thisDel = delFunc;
}

VLIST* listNew(void (*ptrFunc)(void*), void* (*newFunc)(void*), void (*delFunc)(void*), void (*cpyFunc)(void*, void*))
{
	VLIST *l=(VLIST*)malloc(sizeof(VLIST));

	if(!l)
		listErr("listNew [memory fault-1]");

	l->first = l->last = NULL;
	l->thisPrt = ptrFunc;
	l->thisNew = newFunc;
	l->thisDel = delFunc;
	l->thisCpy = cpyFunc;
	l->size = 0;
	return l;
}

void listEmpty(VLIST *l)
{
	while(l->first)
	{
		l->last = l->first->front;
		l->thisDel(l->first->data);
		free(l->first);
		l->first = l->last;
	}
	l->size = 0;
	l->first = l->last = NULL;
}

void listFree(void *lPointer)
{
	VLIST *l=(VLIST*)lPointer;
	if(l)
	{
		listEmpty(l);
		free(l);
	}
}

void listPrint(void *lPointer)
{
	VLIST *l=(VLIST*)lPointer;
	VLN *n;
	int cont=0;

	if(!l)
	{
		printf("[ ## void list-1 ##]");
		return;
	}

	if(!(n=l->first))
	{
		printf("[ ## void list-2 ##]");
		return;
	}

	printf("LIST SIZE: %d", l->size);
	while(n)
	{
		printf("\n[%3d] -> ", cont++);
		l->thisPrt(n->data);
		n=n->front;
	}
	if(cont>0)
		printf("\n");
	printf("]\n");
}

void* listNovel(void *lPointer)
{
	VLIST *lNovel = (VLIST*)malloc(sizeof(VLIST));

	if(!lNovel)
		listErr("listNovel [NULL pointer]");
	lNovel->first = lNovel->last = NULL;
	lNovel->size = 0;
	listCopy(lNovel, lPointer);
	return lNovel;
}

void listCopy(void *lPointer, void *lPointer2)
{
	VLIST *l  = (VLIST*)lPointer;
	VLIST *l2 = (VLIST*)lPointer2;
	VLN *n;

	if(!lPointer || !lPointer2)
		listErr("listCopy [NULL pointer(s)]");

	listEmpty(l);
	l->thisPrt = l2->thisPrt;
	l->thisDel = l2->thisDel;
	l->thisCpy = l2->thisCpy;
	l->thisNew = l2->thisNew;

	if(!(n = l2->first))
		return;
	if(!newFirstVLN(l,n->data,1))
	{
		printf("data = %p\n", n->data);
		listErr("listCopy [memory fault-1]");
	}

	while( (n=n->front) )
	{
		if(!newLastVLN(l,n->data,1))
			listErr("copyList [memory fault-2]");
	}
}

void listCopyRef(void *lPointer, void *lPointer2)
{
	VLIST *l  = (VLIST*)lPointer;
	VLIST *l2 = (VLIST*)lPointer2;

	if(!lPointer || !lPointer2)
		listErr("listCopyRef [NULL pointer(s)]");

	listEmpty(l);
	l->thisPrt = l2->thisPrt;
	l->thisDel = l2->thisDel;
	l->thisCpy = l2->thisCpy;
	l->thisNew = l2->thisNew;
	
	l->first = l2->first;
	l->last = l2->last;
	l->size = l2->size;
}

void listConcat(VLIST *l, VLIST *l2)
{
	VLN *n = l2->first;

	while(n)
	{
		if(!newLastVLN(l,n->data,1))
			listErr("copyList [memory fault-2]");
		n = n->front;
	}
}

void listAppend(VLIST *l, void *data)
{
	if(l->size==0)
	{
		if(!newFirstVLN(l,data,0))
			listErr("append [memory fault-1]");
	}
	else
	{
		if(!newLastVLN(l,data,0))
			listErr("append [memory fault-2]");
	}
}

void listAppendCopy(VLIST *l, void *data)
{
	if(l->size==0)
	{
		if(!newFirstVLN(l,data,1))
			listErr("appendCopy [memory fault-1]");
	}
	else
	{
		if(!newLastVLN(l,data,1))
			listErr("appendCopy [memory fault-2]");
	}
}

void listAdd(VLIST *l, int pos, void *data)
{
	char textAux[100];
	VLN *p=l->first;

	if(pos>l->size || pos<0)
	{
		sprintf(textAux, "add [position %d err (size list:%d)]", pos, l->size);
		listErr(textAux);
	}
	
	if(pos==0)
	{
		if(!newFirstVLN(l,data,0))
			listErr("add [memory fault-1]");
	}
	else if(pos==l->size)
	{
		if(!newLastVLN(l,data,0))
			listErr("add [memory fault-2]");
	}
	else
	{
		while(--pos)
			p=p->front;
		if(!newPositionVLN(l,p,data,0))
			listErr("add [memory fault-3]");
	}
}

void listAddCopy(VLIST *l, int pos, void *data)
{
	char textAux[100];
	VLN *p=l->first;

	if(pos>l->size || pos<0)
	{
		sprintf(textAux, "addCopy [position %d err (size list:%d)]", pos, l->size);
		listErr(textAux);
	}

	if(pos==0)
	{
		if(!newFirstVLN(l,data,1))
			listErr("addCopy [memory fault-1]");
	}
	else if(pos==l->size)
	{
		if(!newLastVLN(l,data,1))
			listErr("addCopy [memory fault-2]");
	}
	else
	{
		while(--pos)
			p=p->front;
		if(!newPositionVLN(l,p,data,1))
			listErr("addCopy [memory fault-3]");
	}
}

void listDel(VLIST *l, int pos)
{
	char textAux[100];
	VLN *p=l->first;

	if(pos>=l->size || pos<0)
	{
		sprintf(textAux, "del [position %d err (size list:%d)]", pos, l->size);
		listErr(textAux);
	}
	if(pos==(l->size - 1))
	{
		delPositionVLN(l,l->last);
		return;
	}

	while(pos--)
		p=p->front;
	delPositionVLN(l,p);
}

void listDelLast(VLIST *l)
{
	if(!l->last)
		listErr("delLast [list is void]");
	delPositionVLN(l,l->last);
}

int listDelData(VLIST *l, void *data)
{
	VLN *p=l->first;

	while(p!=NULL)
	{
		if(p->data == data)
		{
			delPositionVLN(l,p);
			return 1;
		}
		p=p->front;
	}
	return 0;
}

void listNulData(VLIST *l, void *data)
{
	VLN *p=l->first;
	
	while(p!=NULL)
	{
		if(p->data == data)
		{
			p->data=NULL;
			return;
		}
		p=p->front;
	}
}

int listDelIter(VLIST *l, VListIter *iter, int set)
{
	VLN *p = (*iter);
	if(l && iter)
	{
		if(set<0)
			(*iter) = p->back;
		else if(set>0)
			(*iter) = p->front;
		else
			(*iter) = NULL;

		delPositionVLN(l,p);
		return 1;
	}
	return 0;
}

void listNulIter(VLIST *l, VListIter *iter)
{
	(*iter)->data=NULL;
}

void* listGetRef(VLIST *l, int pos)
{
	char textAux[100];
	VLN *p=l->first;

	if(pos>=l->size || pos<0)
	{
		sprintf(textAux, "getRef [position %d err (size list:%d)]", pos, l->size);
		listErr(textAux);
	}
	if(pos==(l->size - 1))
	{
		p=l->last;
		pos=0;
	}

	while(pos)
	{
		p=p->front;
		pos--;
	}
	return p->data;
}

void* listGetRefLast(VLIST *l)
{
	VLN *p=l->last;

	if(!p)
		listErr("getRefLast [list is void]");
	return p->data;
}

void  listRefMoveFirst(VLIST *l, VListIter *iter)
{
	(*iter) = l->first;
}

void  listRefMoveLast(VLIST *l, VListIter *iter)
{
	(*iter) = l->last;
}

void* listRefMoveNext(VLIST *l, VListIter *iter)
{
	void *p;
	
	if(iter== NULL || (*iter) == NULL)
		return NULL;
	p = (*iter)->data;
	(*iter) = (*iter)->front;

	return p;
}

void* listRefMovePrev(VLIST *l, VListIter *iter)
{
	void *p;
	
	if(iter== NULL || (*iter) == NULL)
		return NULL;
	p = (*iter)->data;
	(*iter) = (*iter)->back;

	return p;
}


void listGetCopy(VLIST *l, int pos, void *dReturn)
{
	char textAux[100];
	VLN *p=l->first;

	if(pos>=l->size || pos<0)
	{
		sprintf(textAux, "getRef [position %d err (size list:%d)]", pos, l->size);
		listErr(textAux);
	}
	if(pos==(l->size - 1))
	{
		p=l->last;
		pos=0;
	}

	while(pos--)
		p=p->front;
	l->thisCpy(dReturn, p->data);
}

void listGetCopyLast(VLIST *l, void *dReturn)
{
}

