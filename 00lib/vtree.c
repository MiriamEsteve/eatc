#include <stdio.h>
#include <stdlib.h>

#include "vtree.h"

/////////////////////////////////////////////////////////////
// Funciones internas de la librería (no incluidas en el .h)
/////////////////////////////////////////////////////////////

void delTreeBranch(VTN *branch, VTREE *t)
{
	if(branch->left)
		delTreeBranch(branch->left, t);
	if(branch->right)
		delTreeBranch(branch->right, t);
	t->thisDel(branch->data);
	free(branch);
}

VTN* treeCopyRecursive(VTN *n, VTREE *t)
{
	VTN *nAux;
	
	if(n==NULL)
		return NULL;
	
	if( !(nAux=(VTN*)malloc(sizeof(VTN))) )
		treeErr("treeCopyRecursive [memory fault-1]");
	if( !(nAux->data=t->thisNew(n->data)) )
		treeErr("treeCopyRecursive [memory fault-2]");

	if(n->left)
	{
		nAux->left = treeCopyRecursive(n->left , t);
		nAux->left->father = nAux;
	}
	else
		nAux->left = NULL;
	if(n->right)
	{
		nAux->right = treeCopyRecursive(n->right, t);
		nAux->right->father = nAux;
	}
	else
		nAux->right = NULL;

	return nAux;
}



/////////////////////////////////////////////////////////////
// Funciones publicas
/////////////////////////////////////////////////////////////
void treeErr(char *err)
{
	fprintf(stderr, "  ## TREE-ERROR >> %s\n", err);
	getchar();
	exit(0);
}

void treeIni(VTREE *t)
{
	t->first = NULL;
	t->thisPrt = t->thisDel = NULL;
	t->thisNew = NULL;
	t->size = 0;
}

void treeIniAll(VTREE *t, void  (*ptrFunc)(void*), void* (*newFunc)(void*), void (*delFunc)(void*), void (*cpyFunc)(void*, void*))
{
	t->first = NULL;
	t->thisPrt = ptrFunc;
	t->thisNew = newFunc;
	t->thisDel = delFunc;
	t->thisCpy = cpyFunc;
	t->size = 0;
}

void treeIniPrt(VTREE *t, void  (*ptrFunc)(void*))
{
	t->thisPrt = ptrFunc;
}

void treeIniNew(VTREE *t, void* (*newFunc)(void*))
{
	t->thisNew = newFunc;
}

void treeIniDel(VTREE *t, void  (*delFunc)(void*))
{
	t->thisDel = delFunc;
}

VTREE* treeNew(void (*ptrFunc)(void*), void* (*newFunc)(void*), void (*delFunc)(void*), void (*cpyFunc)(void*, void*))
{
	VTREE *t = (VTREE*)malloc(sizeof(VTREE));

	if(!t)
		treeErr("treeNew [memory fault]");

	t->first = NULL;
	t->thisPrt = ptrFunc;
	t->thisNew = newFunc;
	t->thisDel = delFunc;
	t->thisCpy = cpyFunc;
	t->size = 0;
	return t;
}

void treeEmpty(VTREE *t)
{
	if(!t)
		return;
	if(t->first)
	{
		if(t->first->left)
			delTreeBranch(t->first->left, t);
		if(t->first->right)
			delTreeBranch(t->first->right, t);
		t->thisDel(t->first->data);
		free(t->first);
		t->first = NULL;
		t->size = 0;
	}
}

void treeFree(void *tPointer)
{
	VTREE *t=(VTREE*)tPointer;
	if(t)
	{
		treeEmpty(t);
		free(t);
	}
}

void treePrint(void *tPointer) // Imprime el árbol por niveles
{
	VTREE *t=(VTREE*)tPointer;
	int cont=0;
	int contPush=0;
	int contPop=0;
	VTN **cola;
	
	if(!t)
	{
		printf("[ ## void tree-1 ##]");
		return;
	}
	if(!t->first || !t->size)
	{
		printf("[ ## void tree-2 ##]");
		return;
	}

	if( !(cola = (VTN**)malloc(sizeof(VTN)*t->size)) )
		treeErr("treePrint [memory fault]");

	cola[contPush]=t->first;
	contPush++;

	printf("[");
	while(contPop < contPush)
	{
		printf("\n [%3d] -> ", cont++);
		t->thisPrt(cola[contPop]->data);
		if(cola[contPop]->left)
		{
			cola[contPush]=cola[contPop]->left;
			contPush++;
		}
		if(cola[contPop]->right)
		{
			cola[contPush]=cola[contPop]->right;
			contPush++;
		}
		contPop++;
	}
	printf("]\n");
	free(cola);
}

void* treeNovel(void *tPointer)
{
	VTREE *tNovel = (VTREE*)malloc(sizeof(VTREE));

	if(!tNovel)
		treeErr("treeNew [NULL pointer]");
	tNovel->first = NULL;
	tNovel->size = 0;
	treeCopy(tNovel, tPointer);
	return tNovel;
}

void treeDelNode(VTREE *t, VTN *n)
{
	VTN *father = n->father;

	if(father->left == n)
		father->left = NULL;
	else if(father->right == n)
		father->right = NULL;
	else
		treeErr("treeDelNode [left/right fault]");

	t->thisDel(n->data);
	free(n);
	t->size--;
}

void treeDelBranches(VTREE *t, VTN *n)
{
	if(n->left)
	{
		if(n->left->left || n->left->right)
			treeDelBranches(t, n->left);
		t->thisDel(n->left->data);
		free(n->left);
		t->size--;
		n->left = NULL;
	}
	if(n->right)
	{
		if(n->right->left || n->right->right)
			treeDelBranches(t, n->right);
		t->thisDel(n->right->data);
		free(n->right);
		t->size--;
		n->right = NULL;
	}
}

void treeCopy(void *tPointer, void *tPointer2)
{
	VTREE *t  = (VTREE*)tPointer;
	VTREE *t2 = (VTREE*)tPointer2;

	if(!tPointer || !tPointer2)
		treeErr("treeCopy [NULL pointer(s)]");

	treeEmpty(t);
	t->thisPrt = t2->thisPrt;
	t->thisDel = t2->thisDel;
	t->thisCpy = t2->thisCpy;
	t->thisNew = t2->thisNew;

	if(!t2->first)
		return;

	t->first = treeCopyRecursive(t2->first, t);
	t->first->father=NULL;
	t->size=t2->size;
}

void treeCopyRef(void *tPointer, void *tPointer2)
{
	VTREE *t  = (VTREE*)tPointer;
	VTREE *t2 = (VTREE*)tPointer2;

	if(!tPointer || !tPointer2)
		treeErr("treeCopyRef [NULL pointer(s)]");

	treeEmpty(t);
	t->thisPrt = t2->thisPrt;
	t->thisDel = t2->thisDel;
	t->thisCpy = t2->thisCpy;
	t->thisNew = t2->thisNew;

	if(!t2->first)
		return;

	t->first = t2->first;
	t->first->father=NULL;
	t->size=t2->size;
}

void treeAppendRoot(VTREE *t, void *data)
{
	if(t==NULL)
		treeErr("treeAppendRoot [NULL tree]");

	if(t->first || t->size!=0)
		treeErr("treeAppendRoot [no void tree!!!!]");

	if( !(t->first = (VTN*)malloc(sizeof(VTN))) )
		treeErr("treeAppendRoot [memory fault]");
	
	t->first->data = data;
	t->first->left = t->first->right = t->first->father = t->first->next = NULL;
	t->size=1;
}

void treeAppendRootCopy(VTREE *t, void *data)
{
	if(t==NULL)
		treeErr("treeAppendRootCopy [NULL tree]");

	if(t->first || t->size!=0)
		treeErr("treeAppendRootCopy [no void tree!!!!]");

	if( !(t->first = (VTN*)malloc(sizeof(VTN))) )
		treeErr("treeAppendRootCopy [memory fault-1]");
	
	if( !(t->first->data = t->thisNew(data)) )
		treeErr("treeAppendRootCopy [memory fault-2]");
	t->first->left = t->first->right = t->first->father = t->first->next = NULL;
	t->size=1;
}

void treeAppendSons(VTN *nFather, void *dataLeft, void *dataRight, VTREE *t)
{
	if(!nFather)
		treeErr("treeAppendSons [NULL father]");
	if(nFather->left || nFather->right)
		treeErr("treeAppendSons [NULL son(s)]");
	if( !(nFather->left=(VTN*)malloc(sizeof(VTN))) )
		treeErr("treeAppendSons [memory fault-1]");
	if( !(nFather->right=(VTN*)malloc(sizeof(VTN))) )
		treeErr("treeAppendSons [memory fault-2]");

	nFather->left->left = nFather->left->right = nFather->right->left = nFather->right->right = NULL;
	nFather->left->father = nFather->right->father = nFather;
	nFather->left->data = dataLeft;
	nFather->right->data= dataRight;
	t->size+=2;
}

void treeAppendSonsCopy(VTN *nFather, void *dataLeft, void *dataRight, VTREE *t)
{
	if(!nFather)
		treeErr("treeAppendSonsCopy [NULL father]");
	if( !(nFather->left=(VTN*)malloc(sizeof(VTN))) )
		treeErr("treeAppendSonsCopy [memory fault-1]");
	if( !(nFather->right=(VTN*)malloc(sizeof(VTN))) )
		treeErr("treeAppendSonsCopy [memory fault-2]");

	nFather->left->left = nFather->left->right = nFather->right->left = nFather->right->right = NULL;
	nFather->left->father = nFather->right->father = nFather;
	nFather->left->data = t->thisNew(dataLeft);
	nFather->right->data= t->thisNew(dataRight);
	t->size+=2;
}


/////////////////////////////////////////////////////////////
// Funciones publicas ITERADORES
/////////////////////////////////////////////////////////////
void  treeIterFirst(VTreeIter *it)
{
	it->i = 0;
}

void  treeIterLast(VTreeIter *it)
{
	it->i = it->size-1;
}

void* treeIterNext(VTreeIter *it)
{
	VTN *node;

	if(it==NULL || it->n==NULL || it->i >= it->size)
		return NULL;

	node = it->n[ it->i ];
	it->i++;
	if(it->i >= it->size)
		treeIterFree(it);

	return node->data;
}

void* treeIterPrev(VTreeIter *it)
{
	VTN *node;

	if(it==NULL || it->n==NULL || it->i < 0)
		return NULL;

	node = it->n[ it->i ];
	it->i--;
	if(it->i < 0)
		treeIterFree(it);

	return node->data;
}

VTN*  treeIterNextNode(VTreeIter *it)
{
	VTN *node;

	if(it==NULL || it->n==NULL || it->i >= it->size)
		return NULL;

	node = it->n[ it->i ];
	it->i++;
	if(it->i >= it->size)
		treeIterFree(it);

	return node;
}

VTN*  treeIterPrevNode(VTreeIter *it)
{
	VTN *node;

	if(it==NULL || it->n==NULL || it->i < 0)
		return NULL;

	node = it->n[ it->i ];
	it->i--;
	if(it->i < 0)
		treeIterFree(it);

	return node;
}

void treeIterFree(VTreeIter *it)
{
	free(it->n);
	it->n=NULL;
	it->size = it->i = 0;
}

void treeIterSet(VTREE *t, VTreeIter *it, int mode, int set)
{
	VTN *n;
	int a, b, leave;

	if(t->size==0)
	{
		it->n = NULL;
		it->size = it->i = 0;
		return;
	}

	if( !(it->n=(VTN**)malloc(sizeof(VTN*) * t->size)) )
		treeErr("treeIterSet [memory fault]");
	it->size = t->size;
	for(a=0 ; a<it->size ; a++)
		it->n[a] = NULL;
	
	switch(mode)
	{
		case PREORDER:
		case INORDER:
		case POSTORDER:
		case LEVELS:
			a = b = 0;
			it->n[b] = t->first;
			b++;
			while(a < it->size)
			{
				if(!(it->n[a]))
					treeErr("treeIterSet [NULL pointer]");

				if(it->n[a]->left)
				{
					it->n[b] = it->n[a]->left;
					b++;
				}
				if(it->n[a]->right)
				{
					it->n[b] = it->n[a]->right;
					b++;
				}
				a++;
			}
			break;

		case LEAVES:
			leave = a = b = 0;
			it->n[b] = t->first;
			if(it->n[b]->left==NULL && it->n[b]->right==NULL) // es un nodo hoja
			{
				it->n[leave] = it->n[b];
				leave++;
			}
			b++;
			while(a < b)
			{
				if(it->n[a]->left)
				{
					it->n[b] = it->n[a]->left;
					if(it->n[b]->left==NULL && it->n[b]->right==NULL) // es un nodo hoja
					{
						it->n[leave] = it->n[b];
						leave++;
					}
					b++;
				}
				if(it->n[a]->right)
				{
					it->n[b] = it->n[a]->right;
					if(it->n[b]->left==NULL && it->n[b]->right==NULL) // es un nodo hoja
					{
						it->n[leave] = it->n[b];
						leave++;
					}
					b++;
				}
				a++;
			}
			it->size = leave;
			break;

		default:
			treeErr("treeIterSet [mode err]");
			break;
	}
	if(set==START)
		it->i = 0;
	else if(set==END)
		it->i = it->size-1;
	else
		treeErr("treeIterSet [set err]");
}


