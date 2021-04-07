/*
Librer�a VOID LIST (Lista Gen�rica)
===================================
Cada nodo contiene un puntero *void para referenciar el dato
que dicho nodo debe contener. Requerimientos
*/

#ifndef _VOID_LIST___
#define _VOID_LIST___

// Estructura NODO para la lista VOIDLIST
typedef struct VoidListNode
{
	void *data;
	struct VoidListNode *front, *back;
} VLN;

typedef struct 
{
	int size;
	VLN *first, *last;
	void (*thisPrt)(void*), (*thisDel)(void*), (*thisCpy)(void*, void*);
	void*(*thisNew)(void*);
} VLIST;

typedef VLN* VListIter;

// Funciones
void   listErr(char *err);

void listIni(VLIST *l);
void listIniAll(VLIST *l, void  (*ptrFunc)(void*), void* (*newFunc)(void*), void (*delFunc)(void*), void (*cpyFunc)(void*, void*));
void listIniPrt(VLIST *l, void  (*ptrFunc)(void*));
void listIniNew(VLIST *l, void* (*newFunc)(void*));
void listIniDel(VLIST *l, void  (*delFunc)(void*));

VLIST* listNew(void (*ptrFunc)(void*), void* (*newFunc)(void*), void (*delFunc)(void*), void (*cpyFunc)(void*, void*));
void   listEmpty(VLIST *l);
void   listFree(void *lPointer);
void   listPrint(void *lPointer);
void*  listNovel(void *lPointer);

void listCopy(void *lPointer, void *lPointer2);
void listCopyRef(void *lPointer, void *lPointer2);

void listConcat(VLIST *l, VLIST *l2);

void listAppend(VLIST *l, void *data);
void listAppendCopy(VLIST *l, void *data);

void listAdd(VLIST *l, int pos, void *data);
void listAddCopy(VLIST *l, int pos, void *data);

void listDel(VLIST *l, int pos);
void listDelLast(VLIST *l);
int listDelData(VLIST *l, void *data);
void listNulData(VLIST *l, void *data);

int listDelIter(VLIST *l, VListIter *iter, int set);
void  listNulIter(VLIST *l, VListIter *iter);
void* listGetRef(VLIST *l, int pos);
void* listGetRefLast(VLIST *l);

void  listRefMoveFirst(VLIST *l, VListIter *iter);
void  listRefMoveLast(VLIST *l, VListIter *iter);
void* listRefMoveNext(VLIST *l, VListIter *iter);
void* listRefMovePrev(VLIST *l, VListIter *iter);

void listGetCopy(VLIST *l, int pos, void *dReturn);
void listGetCopyLast(VLIST *l, void *dReturn);


#endif
