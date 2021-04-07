/*
Librería VOID TREE (Árbol Genérico)
===================================
Cada nodo contiene un puntero *void para referenciar el dato
que dicho nodo debe contener. Requerimientos
*/

#ifndef _VOID_TREE___
#define _VOID_TREE___

// Estructura NODO para la lista VOIDLIST
typedef struct VoidTreeNode
{
	void *data;
	struct VoidTreeNode *father, *left, *right, *next;
} VTN;

typedef struct 
{
	int size;
	VTN *first;
	void (*thisPrt)(void*), (*thisDel)(void*), (*thisCpy)(void*, void*);
	void*(*thisNew)(void*);
} VTREE;

typedef struct
{
	VTN **n;   // Array de punteros a Nodos VTN
	int size, i;
} VTreeIter;


// Funciones
void treeErr(char *err);

void treeIni(VTREE *t);
void treeIniAll(VTREE *t, void  (*ptrFunc)(void*), void* (*newFunc)(void*), void (*delFunc)(void*), void (*cpyFunc)(void*, void*));
void treeIniPrt(VTREE *t, void  (*ptrFunc)(void*));
void treeIniNew(VTREE *t, void* (*newFunc)(void*));
void treeIniDel(VTREE *t, void  (*delFunc)(void*));

VTREE* treeNew(void (*ptrFunc)(void*), void* (*newFunc)(void*), void (*delFunc)(void*), void (*cpyFunc)(void*, void*));
void   treeEmpty(VTREE *t);
void   treeFree(void *tPointer);
void   treePrint(void *tPointer);
void*  treeNovel(void *tPointer);
void   treeDelNode(VTREE *t, VTN *n);
void   treeDelBranches(VTREE *t, VTN *n);

void treeCopy(void *tPointer, void *tPointer2);
void treeCopyRef(void *tPointer, void *tPointer2);

void treeAppendRoot(VTREE *t, void *data);
void treeAppendRootCopy(VTREE *t, void *data);
void treeAppendSons(VTN *n, void *dataLeft, void *dataRight, VTREE *t);
void treeAppendSonsCopy(VTN *n, void *dataLeft, void *dataRight, VTREE *t);


// Operar con iteradores del arbol
#define PREORDER  1
#define INORDER   2
#define POSTORDER 3
#define LEVELS    4
#define LEAVES    5

#define START     1
#define END       1

void  treeIterFirst(VTreeIter *it);
void  treeIterLast(VTreeIter *it);
void* treeIterNext(VTreeIter *it);
void* treeIterPrev(VTreeIter *it);
VTN*  treeIterNextNode(VTreeIter *it);
VTN*  treeIterPrevNode(VTreeIter *it);
void  treeIterFree(VTreeIter *it);
void  treeIterSet(VTREE *t, VTreeIter *it, int mode, int set);
#endif
