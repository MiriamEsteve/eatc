//#define INF (1.0/0.0)
#define DEBUG printf

#include "00lib/vlist.h"
#include "00lib/data.h"
#include "00lib/vtree.h"
#include "00lib/eatStructure.h"


/************************************************************************************************************************
 #################################### EAT ###########################################
**************************************************************************************************************************/
void   ArrayPrint(double *array, int nArray);
void   ArrayPrintInt(int *array, int nArray);

double MaxDF(DFrameNum *df, int *index, int nIndex, int y);
double QuadErr(DFrameNum *df, int *index, int nIndex, int *Y, double *yEst, int nY);

int    ComparePareto(const void *a, const void *b);
int    CompareDouble(const void *a, const void *b);
int    IsValueInArray(double val, double *array, int nArray); // find duplicates
int    GetSortArrayFormIndex(double *array, DFrameNum *df, int column, int *index, int nIndex);

int*   NumValsLeft(DFrameNum *df, int *tIndex, int nIndex, int xi, double s, int *nIndexL); //Datos que quedan a la izq.
int*   NumValsRight(DFrameNum *df, int *tIndex, int nIndex, int xi, double s, int *nIndexR); //Datos que quedan a la der.

int    IsFinalNode(NEAT *t, DFrameNum *df, int *X, int numStop);

void   EstimEAT(NEAT *tData, DFrameNum *df, VLIST *leaves, int *Y, int *indexL, int nIndexL, double *estL);
void   ErrEAT(NEAT *tData, DFrameNum *df, VLIST *leaves, int *Y, int xCol, double s,
              double *errL, double *errR, double *estL);

void   Split(NEAT *tData, VLIST *leaves, DFrameNum *df, int *X, int *Y, NEAT **TL, NEAT **TR);

// ******************************* ALPHA y EAT **********************************
int  IsContent(NEAT *t1, NEAT *t2);
int  IsPareto(VTREE *tree, NEAT *i);
void RBranch(VTN *t, VTREE *tree, double *errBranch, int *numLeaves);
double Alpha(VTREE *tree);

VTN* SelectFather(VLIST *leaves, int num);
void EATStart(DFrameNum *df, int *X, int nX, int *Y, int nY, VTREE *tree, VLIST *leaves, VLIST *treeAlphaList);

void EAT(DFrameNum *df, int *X, int nX, int *Y, int nY, int numStop, VTREE *tree, VLIST *treeAlphaList);

int CheckEAT(VTREE *tree);


/************************************************************************************************************************
 ################################## EAT Prunning #######################################
**************************************************************************************************************************/
double* Predictor(VTREE *tree, double *reg);
void TreesForRCV(DFrameNum *df, int *X, int nX, int *Y, int nY, int folder, int numStop, VLIST **TAiv);
double RCV(DFrameNum *Lv, int *Y, int nY, double alphaIprim, int folder, VLIST **TAiv, TREEALPHA **BestTivs);

// *****************************  Pruning EAT *********************************
TREEALPHA* EATPruning(VLIST *treeAlphaList, DFrameNum *df, int *X, int nX, int *Y, int nY, int folder, int numStop);







