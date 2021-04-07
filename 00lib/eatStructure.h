/*
Estructura de nodo para algoritmo EAT
*/


#ifndef _DATA_DEA_
#define _DATA_DEA_

//#define INF (1.0/0.0)
#define DEBUG printf

#include "00lib/vtree.h"


// int NCOLS; // Numero de columnas del fichero de datos
// Indicar su valor antes de usar 'newNEAT', ...

/************************************************************************************************************************
 #################################### NEAT ###########################################
**************************************************************************************************************************/
// ************************ Estructura Var Info *****************************
typedef struct
{
	double SL, SR, S;
} VARINFO;

// **************************** Estructura NEAT *****************************
typedef struct NodeEAT
{
	int id, idF; // ID de nodo / ID del padre
	int nX, nY;  // nº de Xs (inputs) / nº de Ys (outputs)
	int *index, nIndex; // array datos compatibles con el nodo (soporte)
	int nArray;    // array de valores diferentes (nArray <= nIndex)
	double *array;
	VARINFO *varInfo; // tamanyo: nX (num. variables independientes)
	double R, errMin;
	int xi;
	double s, sMax; // 'sMax' es para el backtracking
	double *y;
	double *a, *b; // tamanyo: nX (num. variables independientes)
} NEAT;

// **************************** Funciones NEAT *****************************
void  EATErr(char *err); // Mensajes de error EAT
void  NEATPrint(void *d);
void* NEATNew(void *d);
void  NEATDel(void *d);
void  NEATCpy(void *d1, void *d2);
void  NEATCpySon(void *d1, void *d2); // copia info para el hijo (izq. o der.)
void  NEATSet(void *d, int nX, int nY, int N);

void  NEATPrintVTN(void *d);
void* NEATNewVTN(void *d);
void  NEATDelVTN(void *d);
void  NEATCpyVTN(void *d1, void *d2);


/************************************************************************************************************************
 ################################## Pruning EAT ########################################
**************************************************************************************************************************/
// ************************* Estructura Pruning EAT **************************
typedef struct
{
    double alpha;
    double score;
	double errFolders;
    VTREE *tree;
}TREEALPHA;

// ************************* Funciones Pruning EAT ***************************
void  TreeAlphaPrint(void *ta); // 'ta' -> treeAlpha
void* TreeAlphaNew(void *ta);
void  TreeAlphaDel(void *ta);
void  TreeAlphaCpy(void *ta1, void *ta2);

#endif
