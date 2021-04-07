
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define __TIEMPOS__

#include "00lib/distrib.h"
#include "00lib/data.h"
#include "00lib/vtree.h"
#include "eat.h"

void Datos(DFrameNum *df, int semilla)
{
	int row, col;
	double arr[10000];

	int numX = df->nCols-1;
	double alpha = 0.5/numX;
	double lambda = 3.0;

	srand(semilla);

	for(col=0 ; col<numX ; col++)
	{
		RandUniform(10, 50, arr, df->nRows);
		dfNumSetColumn(df, col, arr);
	}
	
	// ESCENARIO COBB DOUGLAS para calcular la 'Y'
	for(row=0 ; row<df->nRows ; row++)
	{
		// Función de 'dios'
		df->data[row][numX]=1;
		for(col=0 ; col<numX ; col++)
			df->data[row][numX] *= pow( df->data[row][col] , alpha);

		// Ineficiencia
		df->data[row][numX] *=  exp( log(1-(rand()/(1.0+RAND_MAX)))/lambda );
	}
}



int main(int argc, char *argv[])
{
	time_t t;
	int nCores = 2;
	int numNoFinalLeaves;

	int nRows = 50;
	int nCols = 4;
	char *header[]={"X1", "X2", "X3", "Y1"};
	int X[] = { 0, 1, 2};
	int nX=nCols-1;
	int nY = 1;
	int Y[]={nX};

	int semilla = 1; //(int)clock();
	//int semilla = (unsigned)time(&t);

	int numStop = 5;
	int folder = 5;
	int J = 5; //EATBoost depth tree
	clock_t t_ini, t_fin;


	VLIST *treeAlphaList = listNew(TreeAlphaPrint, TreeAlphaNew, TreeAlphaDel, TreeAlphaCpy);
	VLIST *leaves = listNew(NEATPrintVTN, NEATNewVTN, NEATDelVTN, NEATCpyVTN);
	VTREE *tree = treeNew(NEATPrint, NEATNew, NEATDel, NEATCpy);
	TREEALPHA *Tk;

	DFrameNum *df = dfNumNew(nRows, nCols, header);
	Datos(df, semilla); // COBB DUGLAS
	//double border = 0;
	//double noise = 0;
	//df = MakeDF_X2Y2(nRows, border, noise);
	//dfNumPrint(df);// getchar();

	// 1 - Arbol profundo Heuristico
	printf("1- EATStart\n");
	EATStart(df, X, nX, Y, nY, tree, leaves, treeAlphaList);
	numNoFinalLeaves = leaves->size; // 'leaves' contiene al noda raiz que SÍ es no-final

	// 2 - Llamar a EAT
	printf("2- deep_EAT\n");
	printf("nRows: %d, nCols: %d, numStop: %d\n", nRows, nCols, numStop);
	
	t_ini = clock();
	EAT(df, X, nX, Y, nY, numStop, 0, tree, leaves, numNoFinalLeaves, treeAlphaList);
	t_fin = clock();
	printf("Fin deep_EAT....\n=======\n");
	printf("nRows: %d, nCols: %d, numStop: %d, size: %d, time : %f\n", nRows, nCols, numStop, tree->size, (double)(t_fin - t_ini) / CLOCKS_PER_SEC);
	//treePrint(tree);
    if(CheckEAT(tree))
        printf("tree OK\n");
    else
        printf("tree KK\n");

	t_ini = clock();
	Tk = EATPruning(treeAlphaList, df, X, nX, Y, nY, folder, numStop);
	t_fin = clock();
	printf("Fin EAT....\n=======\n");
	printf("nRows: %d, nCols: %d, numStop: %d, size: %d, time : %f\n", nRows, nCols, numStop, Tk->tree->size, (double)(t_fin - t_ini) / CLOCKS_PER_SEC);
	//treePrint(tree);
    if(CheckEAT(tree))
        printf("tree OK\n");
    else
        printf("tree KK\n");
 	
	dfNumFree(df);
	listFree(treeAlphaList);
	listFree(leaves);
	treeFree(tree);

	return 0;
}

