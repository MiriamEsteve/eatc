#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>

#include "00lib/eatStructure.h"


/****************************************************************************************
 ###################### Nodo para el arbol EAT #####################
******************************************************************************************/
// *********** Mensajes de error EAT ********************
void EATErr(char* err)
{
	fprintf(stderr, "  ## EAT-ERROR >> %s\n", err);
	getchar();
	exit(0);
}

void NEATPrint(void* d)
{
	int i;
	NEAT* n = (NEAT*)d;

	if (!n)
	{
		printf("[NEAT NULL]\n");
		return;
	}

	printf("[ME:%d (F:%d), xi: %d, s: %.4f, R: %.4f(%.4f), y->[%.4f ", n->id, n->idF, n->xi, n->s, n->R, n->errMin, n->y[0]);
	for (i = 1; i < n->nY; i++)
		printf(",%.4f", n->y[i]);

	printf("], [idx:%d -> (%d", n->nIndex, n->index[0]);
	if (n->nIndex < 11)
	{
		for (i = 1; i < n->nIndex; i++)
			printf(",%d", n->index[i]);
	}
	else
	{
		for (i = 1; i < 8; i++)
			printf(",%d", n->index[i]);
		printf(", ...,%d", n->index[(n->nIndex - 1)]);
	}
	printf(")]");

	/*******************************************************************
	// a
	printf("		   a:[%.5f", n->a[0]);
	for(i=1 ; i<n->nX ; i++)
	   printf(", %.5f", n->a[i]);
	printf("]\n");

	// b
	printf("		   b:[%.5f", n->b[0]);
	for(i=1 ; i<n->nX ; i++)
	   printf(", %.5f", n->b[i]);
	printf("]\n");

	// varInfo
	printf("		   vInfo:[[SL:%.4f, SR:%.4f, S:%.4f]", n->varInfo[0].SL, n->varInfo[0].SR, n->varInfo[0].S);
	for(i=1 ; i<n->nX ; i++)
		printf(" [SL:%.4f, SR:%.4f, S:%.4f]", n->varInfo[i].SL, n->varInfo[i].SR, n->varInfo[i].S);
	/*******************************************************************/

	printf("]\n");

}

void* NEATNew(void* d)
{
	char aux[200];
	int i;
	NEAT* newD;

	if (!(newD = (NEAT*)malloc(sizeof(NEAT))))
		return NULL;

	((NEAT*)newD)->nIndex = ((NEAT*)d)->nIndex;
	if(((NEAT*)d)->nIndex == 0)
		((NEAT*)newD)->index = NULL;
	else
	{
		if (!(((NEAT*)newD)->index = (int*)malloc(sizeof(int) * ((NEAT*)newD)->nIndex)))
			EATErr("NEATNew [memory fault-1]");
		for (i = 0; i < ((NEAT*)newD)->nIndex; i++)
			((NEAT*)newD)->index[i] = ((NEAT*)d)->index[i];
	}
	((NEAT*)newD)->nArray = ((NEAT*)d)->nArray;
	if(((NEAT*)d)->nArray == 0)
		((NEAT*)newD)->array = NULL;
	else
	{
		if (!(((NEAT*)newD)->array = (double*)malloc(sizeof(double) * ((NEAT*)newD)->nArray)))
			EATErr("NEATNew [memory fault-2]");
		for (i = 0; i < ((NEAT*)newD)->nArray; i++)
			((NEAT*)newD)->array[i] = ((NEAT*)d)->array[i];
	}
	newD->nX = ((NEAT*)d)->nX;
	newD->nY = ((NEAT*)d)->nY;

	((NEAT*)newD)->varInfo = (VARINFO*)malloc(sizeof(VARINFO) * newD->nX);
	((NEAT*)newD)->y = (double*)malloc(sizeof(double) * newD->nY);
	if (!((NEAT*)newD)->varInfo || !((NEAT*)newD)->y)
	{
		sprintf(aux, "NEATNew [memory fault-3] varInfo=%p, y=%p [nX=%d]", ((NEAT*)newD)->varInfo, ((NEAT*)newD)->y, newD->nX);
		EATErr(aux);
	}

	for (i = 0; i < newD->nX; i++)
	{
		((NEAT*)newD)->varInfo[i].SL = ((NEAT*)d)->varInfo[i].SL;
		((NEAT*)newD)->varInfo[i].SR = ((NEAT*)d)->varInfo[i].SR;
		((NEAT*)newD)->varInfo[i].S = ((NEAT*)d)->varInfo[i].S;
	}

	((NEAT*)newD)->id = ((NEAT*)d)->id;
	((NEAT*)newD)->idF = ((NEAT*)d)->idF;
	((NEAT*)newD)->R = ((NEAT*)d)->R;
	((NEAT*)newD)->errMin = ((NEAT*)d)->errMin;
	((NEAT*)newD)->xi = ((NEAT*)d)->xi;
	((NEAT*)newD)->s = ((NEAT*)d)->s;
	((NEAT*)newD)->sMax = ((NEAT*)d)->sMax;
	for (i = 0; i < newD->nY; i++)
		((NEAT*)newD)->y[i] = ((NEAT*)d)->y[i];

	((NEAT*)newD)->a = (double*)malloc(sizeof(double) * newD->nX);
	((NEAT*)newD)->b = (double*)malloc(sizeof(double) * newD->nX);
	if (!((NEAT*)newD)->a || !((NEAT*)newD)->b)
		EATErr("NEATNew [memory fault-4]");

	for (i = 0; i < newD->nX; i++)
	{
		((NEAT*)newD)->a[i] = ((NEAT*)d)->a[i];
		((NEAT*)newD)->b[i] = ((NEAT*)d)->b[i];
	}

	return (void*)newD;
}

void  NEATDel(void* d)
{
	if (d)
	{
		free(((NEAT*)d)->index);
		free(((NEAT*)d)->array);
		free(((NEAT*)d)->varInfo);
		free(((NEAT*)d)->y);
		free(((NEAT*)d)->a);
		free(((NEAT*)d)->b);
		free(((NEAT*)d));
	}
}

void  NEATCpy(void* d1, void* d2)
{
	int i;

	((NEAT*)d1)->nIndex = ((NEAT*)d2)->nIndex;
	if (((NEAT*)d1)->nIndex == 0)
		((NEAT*)d1)->index = NULL;
	else
	{
		if (!(((NEAT*)d1)->index = (int*)malloc(sizeof(int) * ((NEAT*)d1)->nIndex)))
			EATErr("NEATCpy [memory fault-1]");
		for (i = 0; i < ((NEAT*)d1)->nIndex; i++)
			((NEAT*)d1)->index[i] = ((NEAT*)d2)->index[i];
	}
	((NEAT*)d1)->nArray = ((NEAT*)d2)->nArray;
	if (((NEAT*)d1)->nArray == 0)
		((NEAT*)d1)->array = NULL;
	else
	{
		if (!(((NEAT*)d1)->array = (double*)malloc(sizeof(double) * ((NEAT*)d1)->nArray)))
			EATErr("NEATCpy [memory fault-1]");
		for (i = 0; i < ((NEAT*)d1)->nArray; i++)
			((NEAT*)d1)->array[i] = ((NEAT*)d2)->array[i];
	}

	((NEAT*)d1)->nX = ((NEAT*)d2)->nX;
	((NEAT*)d1)->nY = ((NEAT*)d2)->nY;
	((NEAT*)d1)->varInfo = (VARINFO*)malloc(sizeof(VARINFO) * ((NEAT*)d1)->nX);
	((NEAT*)d1)->y = (double*)malloc(sizeof(double) * ((NEAT*)d1)->nY);
	if (!((NEAT*)d1)->varInfo || !((NEAT*)d1)->y)
		EATErr("NEATCpy [memory fault-2]");

	for (i = 0; i < ((NEAT*)d1)->nX; i++)
	{
		((NEAT*)d1)->varInfo[i].SL = ((NEAT*)d2)->varInfo[i].SL;
		((NEAT*)d1)->varInfo[i].SR = ((NEAT*)d2)->varInfo[i].SR;
		((NEAT*)d1)->varInfo[i].S = ((NEAT*)d2)->varInfo[i].S;
	}

	((NEAT*)d1)->id = ((NEAT*)d2)->id;
	((NEAT*)d1)->idF = ((NEAT*)d2)->idF;
	((NEAT*)d1)->R = ((NEAT*)d2)->R;
	((NEAT*)d1)->errMin = ((NEAT*)d2)->errMin;
	((NEAT*)d1)->xi = ((NEAT*)d2)->xi;
	((NEAT*)d1)->s = ((NEAT*)d2)->s;
	((NEAT*)d1)->sMax = ((NEAT*)d2)->sMax;
	for (i = 0; i < ((NEAT*)d1)->nY; i++)
		((NEAT*)d1)->y[i] = ((NEAT*)d2)->y[i];

	((NEAT*)d1)->a = (double*)malloc(sizeof(double) * ((NEAT*)d1)->nX);
	((NEAT*)d1)->b = (double*)malloc(sizeof(double) * ((NEAT*)d1)->nX);
	if (!((NEAT*)d1)->a || !((NEAT*)d1)->b)
		EATErr("NEATCpy [memory fault-3]");
	for (i = 0; i < ((NEAT*)d1)->nX; i++)
	{
		((NEAT*)d1)->a[i] = ((NEAT*)d2)->a[i];
		((NEAT*)d1)->b[i] = ((NEAT*)d2)->b[i];
	}
}

// copia info para el hijo (izq. o der.)
void  NEATCpySon(void* d1, void* d2)
{
	int i;

	((NEAT*)d1)->s = ((NEAT*)d1)->xi = -1;

	for (i = 0; i < ((NEAT*)d2)->nX; i++)
	{
		((NEAT*)d1)->a[i] = ((NEAT*)d2)->a[i];
		((NEAT*)d1)->b[i] = ((NEAT*)d2)->b[i];
	}
}

void  NEATSet(void* d, int nX, int nY, int N)
{
	int i;

	((NEAT*)d)->nIndex = N;
	if (N == 0)
		((NEAT*)d)->index = NULL;
	else
	{
		if (!(((NEAT*)d)->index = (int*)malloc(sizeof(int) * N)) || !(((NEAT*)d)->array = (double*)malloc(sizeof(double) * N)))
			EATErr("NEATSet [memory fault-1]");
		for (i = 0; i < N; i++)
			((NEAT*)d)->index[i] = i;
	}
	((NEAT*)d)->nArray = 0;

	((NEAT*)d)->nX = nX;
	((NEAT*)d)->nY = nY;

	((NEAT*)d)->varInfo = (VARINFO*)malloc(sizeof(VARINFO) * ((NEAT*)d)->nX);
	((NEAT*)d)->y = (double*)malloc(sizeof(double) * ((NEAT*)d)->nY);
	if (!((NEAT*)d)->varInfo || !((NEAT*)d)->y)
		EATErr("NEATSet [memory fault-2]");
	for (i = 0; i < ((NEAT*)d)->nX; i++)
		((NEAT*)d)->varInfo[i].SL = ((NEAT*)d)->varInfo[i].SR = ((NEAT*)d)->varInfo[i].S = 0;

	((NEAT*)d)->R = -1;
	((NEAT*)d)->errMin = -1;
	((NEAT*)d)->xi = -1;
	((NEAT*)d)->s = -1;
	((NEAT*)d)->sMax = -1;
	for (i = 0; i < ((NEAT*)d)->nY; i++)
		((NEAT*)d)->y[i] = -1;

	((NEAT*)d)->a = (double*)malloc(sizeof(double) * ((NEAT*)d)->nX);
	((NEAT*)d)->b = (double*)malloc(sizeof(double) * ((NEAT*)d)->nX);
	if (!((NEAT*)d)->a || !((NEAT*)d)->b)
		EATErr("NEATSet [memory fault-2]");

	for (i = 0; i < ((NEAT*)d)->nX; i++)
	{
		((NEAT*)d)->a[i] = 0;
		((NEAT*)d)->b[i] = INFINITY;
	}
}

//-----------------
void  NEATPrintVTN(void* d)
{
	if(!d)
	{
		printf("[ ## NULL NEATPrintVTN-1 ##]");
		return;
	}
	if(!((VTN*)d)->data)
	{
		printf("[ ## NULL NEATPrintVTN-2 ##]");
		return;
	}
	NEATPrint(((VTN*)d)->data);
}
void* NEATNewVTN(void* d) { return d; }
void  NEATDelVTN(void* d) {  }
void  NEATCpyVTN(void* d1, void* d2) { d1 = d2; }


/****************************************************************************************
 ########################## Pruning EAT ##########################
******************************************************************************************/
void  TreeAlphaPrint(void* ta) // 'ta' -> treeAlpha
{
	if(!ta)
	{
		printf("[## NULL TREE-ALPHA-1 ##]");
		return;
	}
	if(!(((TREEALPHA*)ta)->tree) || !(((TREEALPHA*)ta)->tree->size))
	{
		printf("[## NULL TREE-ALPHA-2 ##]");
		return;
	}

	printf(" [alpha: %.15f, score: %f, errFold: %f, size: %d]\n",
		((TREEALPHA*)ta)->alpha, ((TREEALPHA*)ta)->score, ((TREEALPHA*)ta)->errFolders, ((TREEALPHA*)ta)->tree->size);
	//listPrint(((TREEALPHA*)ta)->tree);
}

void* TreeAlphaNew(void* ta)
{
	TREEALPHA* newTA;

	if (!ta)
		EATErr("TreeAlphaNew [memory fault - 1]");

	if (!(newTA = (TREEALPHA*)malloc(sizeof(TREEALPHA))))
		EATErr("TreeAlphaNew [memory fault - 2]");

	((TREEALPHA*)newTA)->tree = treeNew(NEATPrint, NEATNew, NEATDel, NEATCpy);
	treeCopy(((TREEALPHA*)newTA)->tree, ((TREEALPHA*)ta)->tree);
	((TREEALPHA*)newTA)->alpha = ((TREEALPHA*)ta)->alpha;
	((TREEALPHA*)newTA)->score = ((TREEALPHA*)ta)->score;
	((TREEALPHA*)newTA)->errFolders = ((TREEALPHA*)ta)->errFolders;

	if (newTA->tree == NULL)
		EATErr("TreeAlphaNew [memory fault - 3]");

	return (void*)newTA;
}

void  TreeAlphaDel(void* ta)
{
	if ((TREEALPHA*)ta)
	{
		treeFree(((TREEALPHA*)ta)->tree);
		free((TREEALPHA*)ta);
	}
}

void  TreeAlphaCpy(void* ta1, void* ta2)
{
	((TREEALPHA*)ta1)->alpha = ((TREEALPHA*)ta2)->alpha;
	((TREEALPHA*)ta1)->score = ((TREEALPHA*)ta2)->score;
	((TREEALPHA*)ta1)->errFolders = ((TREEALPHA*)ta2)->errFolders;
	treeCopy(((TREEALPHA*)ta1)->tree, ((TREEALPHA*)ta2)->tree);
}


