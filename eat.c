#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>

#include "eat.h"

double MiROUND(double val, int div) // 4 decimales significativos
{
	return round(val * div) / div;
}

/****************************************************************************************
 ############################## EAT ############################
******************************************************************************************/
void ArrayPrint(double* array, int nArray)
{
	int i;
	for (i = 0; i < nArray; i++)
		printf("[%3d] -> %.5f\n", i, array[i]);
}

void ArrayPrintInt(int* array, int nArray)
{
	int i;
	for (i = 0; i < nArray; i++)
		printf("[%3d] -> %3d\n", i, array[i]);
}

double MaxDF(DFrameNum* df, int* index, int nIndex, int y)
{
	int i;
	double max;

	if (y < 0 || y >= df->nCols)
		EATErr("MaxDF [y-col error]");

	max = df->data[index[0]][y];
	for (i = 1; i < nIndex; i++)
		if (max < df->data[index[i]][y])
			max = df->data[index[i]][y];
	return max;
}

double QuadErr(DFrameNum* df, int* index, int nIndex, int* Y, double* yEst, int nY)
{
	int iCol, row;
	double R = 0, aux;


	for (iCol = 0; iCol < nY; iCol++)
	{
		if (Y[iCol] < 0 || Y[iCol] >= df->nCols)
			EATErr("SqrErr [y-col error]");

		for (row = 0; row < nIndex; row++)
		{
			aux = df->data[index[row]][Y[iCol]] - yEst[iCol];
			R += aux * aux;
		}
	}
	return MiROUND((R / (nY * df->nRows)), 10000);
}


int ComparePareto(const void* a, const void* b)
{
	int x, cont1 = 0, cont2 = 0;
	NEAT* t1 = (NEAT*)a;
	NEAT* t2 = (NEAT*)b;

	for (x = 0; x < t1->nX; x++)
	{
		if (t1->a[x] < t2->b[x])
			cont1++;
		if (t2->a[x] < t1->b[x])
			cont2++;
	}
	if (cont1 == t1->nX)
		return -1;
	if (cont2 == t1->nX)
		return 1;
	return 0;
}

int CompareDouble(const void* a, const void* b)
{
	double fa = *(const double*)a;
	double fb = *(const double*)b;
	return (fa > fb) - (fa < fb);
}

int IsValueInArray(double val, double* array, int nArray)
{
	int i;
	for (i = 0; i < nArray; i++)
		if (array[i] == val)
			return i;
	return -1;
}

int GetSortArrayFormIndex(double* array, DFrameNum* df, int column, int* index, int nIndex)
{
	int row, nArray = 0;

	array[nArray] = df->data[index[0]][column];
	nArray++;

	for (row = 1; row < nIndex; row++)
		if (IsValueInArray(df->data[index[row]][column], array, nArray) == -1)
		{
			array[nArray] = df->data[index[row]][column];
			nArray++;
		}
	qsort(array, nArray, sizeof(double), CompareDouble);
	return nArray;
}

int* NumValsLeft(DFrameNum* df, int* tIndex, int nIndex, int xi, double s, int* nIndexL)
{
	int i;
	int* indL = NULL;
	(*nIndexL) = 0;
	char aux[200];

	for (i = 0; i < nIndex; i++)
		if (df->data[tIndex[i]][xi] < s)
			(*nIndexL)++;

	if ((*nIndexL) == 0 || !(indL = (int*)malloc(sizeof(int) * (*nIndexL))))
	{
		sprintf(aux, "NumValsLeft [memory fault] (n=%d)", (*nIndexL));
		EATErr(aux);
	}

	(*nIndexL) = 0;
	for (i = 0; i < nIndex; i++)
		if (df->data[tIndex[i]][xi] < s)
		{
			indL[(*nIndexL)] = tIndex[i];
			(*nIndexL)++;
		}

	return indL;
}

int* NumValsRight(DFrameNum* df, int* tIndex, int nIndex, int xi, double s, int* nIndexR)
{
	int i;
	int* indR = NULL;
	(*nIndexR) = 0;

	for (i = 0; i < nIndex; i++)
		if (df->data[tIndex[i]][xi] >= s)
			(*nIndexR)++;

	indR = (int*)malloc(sizeof(int) * (*nIndexR));

	if ((*nIndexR) == 0 || !indR)
		EATErr("NumValsRight [memory fault]");

	(*nIndexR) = 0;
	for (i = 0; i < nIndex; i++)
		if (df->data[tIndex[i]][xi] >= s) {
			indR[(*nIndexR)] = tIndex[i];
			(*nIndexR)++;
		}

	return indR;
}

int IsFinalNode(NEAT* t, DFrameNum* df, int* X, int numStop)
{
	int row, xi;

	if (t->nIndex <= numStop)
		return 1;

	for (row = 1; row < t->nIndex; row++)
	{
		for (xi = 0; xi < t->nX; xi++)
		{
			if (df->data[t->index[0]][X[xi]] != df->data[t->index[row]][X[xi]])
				return 0;
		}
	}
	return 1;
}

void EstimEAT(NEAT* tData, DFrameNum* df, VLIST* leaves, int* Y, int* indexL, int nIndexL, double* estL)
{
	int i;
	double maxL;
	double yInfLeft[100];

	VListIter iter;
	VTN* n;

	if (tData->nY > 100)
		EATErr("EstimEAT [memory fault for 'yInfLeft']");

	for (i = 0; i < tData->nY; i++)
		yInfLeft[i] = 0;

	if (nIndexL == 0)
		for (i = 0; i < tData->nY; i++)
			estL[i] = INFINITY;
	else
	{
		listRefMoveFirst(leaves, &iter);
		while ((n = (VTN*)listRefMoveNext(leaves, &iter)))
		{
			if (ComparePareto(tData, ((NEAT*)n->data)) == 1)
				for (i = 0; i < tData->nY; i++)
					if (yInfLeft[i] < ((NEAT*)n->data)->y[i])
						yInfLeft[i] = ((NEAT*)n->data)->y[i];
		}
		for (i = 0; i < tData->nY; i++)
		{
			maxL = MaxDF(df, indexL, nIndexL, Y[i]);
			estL[i] = (maxL >= yInfLeft[i]) ? maxL : yInfLeft[i];
		}
	}
}

void ErrEAT(NEAT* tData, DFrameNum* df, VLIST* leaves, int* Y, int xCol, double s,
	double* errL, double* errR, double* estL)
{
	double bAux = tData->b[xCol];
	int nIndexL, nIndexR;
	int* indexL = NumValsLeft(df, tData->index, tData->nIndex, xCol, s, &nIndexL);
	int* indexR = NumValsRight(df, tData->index, tData->nIndex, xCol, s, &nIndexR);

	tData->b[xCol] = s; // tData simula su hijo izquierdo 'TL'
	EstimEAT(tData, df, leaves, Y, indexL, nIndexL, estL);
	tData->b[xCol] = bAux; // tData vuelve a ser 'father'

	(*errL) = QuadErr(df, indexL, nIndexL, Y, estL, tData->nY);
	(*errR) = QuadErr(df, indexR, nIndexR, Y, tData->y, tData->nY); // estimR = Father

	free(indexL);
	free(indexR);
}

void EATNewSons(NEAT **TL, NEAT **TR, int nX, int nY)
{
	if (!(*TR = (NEAT*)malloc(sizeof(NEAT))))
		EATErr("EATNewSons [memory fault-1]");
	if (!(*TL = (NEAT*)malloc(sizeof(NEAT))))
		EATErr("EATNewSons [memory fault-2]");
	NEATSet(*TR, nX, nY, 0);
	NEATSet(*TL, nX, nY, 0);
}

void Split(NEAT* tData, VLIST* leaves, DFrameNum* df, int* X, int* Y, NEAT **TL, NEAT **TR)
{
	double err, errMin = INFINITY;
	int i, i2, row, xi;

	int xiRes = 0;
	double sRes = 0.0;

	double errL, errR;
	double* yMaxLeft, * yEstimL;

	EATNewSons(TL, TR, tData->nX, tData->nY);

	yEstimL = (double*)malloc(sizeof(double) * tData->nY);
	yMaxLeft = (double*)malloc(sizeof(double) * tData->nY);
	if (!yEstimL || !yMaxLeft)
		EATErr("Split [memory fault]");
	for (i = 0; i < tData->nY; i++)
		yMaxLeft[i] = 0.0;


	// Recorre todas las variables (columnas)
	for (xi = 0; xi < tData->nX; xi++)
	{
		tData->nArray = GetSortArrayFormIndex(tData->array, df, X[xi], tData->index, tData->nIndex);
		tData->varInfo[xi].SL = tData->varInfo[xi].SR = INFINITY;
		tData->varInfo[xi].S = -1;

		// El siguiente bucle empieza en '1' porque la rama izquierda
		// ( < array[i]) debe tener almenos 1 elemento (si empezara en
		// 0, en la primera iteracion la rama izquierda estaria vacia)
		for (i = 1; i < tData->nArray; i++)
		{
			ErrEAT(tData, df, leaves, Y, X[xi], tData->array[i], &errL, &errR, yEstimL);
			err = errL + errR;

			// R y s menor alternativo para cad xi
			if ((tData->varInfo[xi].SL + tData->varInfo[xi].SR) > err)
			{
				tData->varInfo[xi].SL = errL;
				tData->varInfo[xi].SR = errR;
				tData->varInfo[xi].S = tData->array[i];
			}

			// Si el Error del split s en xi es menor se realiza el split con esa 's'
			if (err < errMin)
			{
				errMin = err;
				xiRes = xi;
				sRes = tData->array[i];
				for (i2 = 0; i2 < tData->nY; i2++)
					yMaxLeft[i2] = yEstimL[i2];
			}
		}
	}

	NEATCpySon(*TL, tData);
	NEATCpySon(*TR, tData);

	for (i = 0; i < tData->nY; i++)
	{
		(*TL)->y[i] = yMaxLeft[i];
		(*TR)->y[i] = tData->y[i];
	}

	(*TL)->index = NumValsLeft(df, tData->index, tData->nIndex, X[xiRes], sRes, &((*TL)->nIndex));
	if ( !((*TL)->array = (double*)malloc(sizeof(double)*(*TL)->nIndex)) )
		EATErr("EATErr: malloc TL array\n");

	(*TL)->b[xiRes] = sRes;
	(*TL)->R = QuadErr(df, (*TL)->index, (*TL)->nIndex, Y, (*TL)->y, tData->nY);

	(*TR)->index = NumValsRight(df, tData->index, tData->nIndex, X[xiRes], sRes, &((*TR)->nIndex));
	if ( !((*TR)->array = (double*)malloc(sizeof(double)*(*TR)->nIndex)) )
		EATErr("EATErr: malloc TR array\n");
	(*TR)->a[xiRes] = sRes;
	(*TR)->R = QuadErr(df, (*TR)->index, (*TR)->nIndex, Y, (*TR)->y, tData->nY);

	(*TL)->idF = (*TR)->idF = tData->id;

	tData->errMin = errMin;
	tData->xi = X[xiRes];
	tData->s = sRes;

	free(yEstimL);
	free(yMaxLeft);
}

// ******************************* ALPHA y EAT **********************************
int IsContent(NEAT* t1, NEAT* t2)
{
	int i;
	for (i = 0; i < t1->nX; i++)
	{
		if (t1->a[i] > t2->a[i])
			return 0;
		if (t1->b[i] < t2->b[i])
			return 0;
	}
	return 1;
}

int IsPareto(VTREE* tree, NEAT* i)
{
	VTN* t;
	int order;

	VTreeIter iter;
	treeIterSet(tree, &iter, LEVELS, START);

	while ((t = treeIterNextNode(&iter)))
	{
		if (((NEAT*)t->data) == i || t->left != NULL ||
			IsContent(i, ((NEAT*)t->data))) // t->data contenido dentro de i
			continue;

		order = ComparePareto(i, ((NEAT*)t->data));
		if (order == -1 && (i->y <= ((NEAT*)t->data)->y))
			return 1;
		if (order == 1 && (i->y >= ((NEAT*)t->data)->y))
			return 1;
	}
	return 0;
}

void RBranch(VTN* t, VTREE* tree, double* errBranch, int* numLeaves)
{
	double errLeft, errRight;
	int numLeft, numRight;

	if (t->left == NULL)
	{
		*errBranch = ((NEAT*)t->data)->R;
		*numLeaves = 1;
		return; // fin recursividad
	}
	RBranch(t->left, tree, &errLeft, &numLeft);
	RBranch(t->right, tree, &errRight, &numRight);

	*errBranch = (errLeft + errRight);
	*numLeaves = (numLeft + numRight);
}

double Alpha(VTREE* tree)
{
	VTN* t, * tMin = NULL;
	double alpha;
	double errBranch;
	double alphaMin = INFINITY;
	int numLeaves;

	VTreeIter iter;
	treeIterSet(tree, &iter, LEVELS, START);

	while ((t = treeIterNextNode(&iter)))
	{
		if ((NEAT*)t->left == NULL) // descartar nodos hoja
			continue;
		//if(!IsPareto(tree, ((NEAT*)t->data)))
		//	continue;

		RBranch(t, tree, &errBranch, &numLeaves);
		alpha = (((NEAT*)t->data)->R - errBranch) / (numLeaves - 1);
		if (alpha < alphaMin)
		{
			alphaMin = alpha;
			tMin = t;
		}
	}
	return alphaMin;
}

VTN* SelectFather(VLIST* leaves, int num)
{
	int pos = 0;
	VTN* t;

	t = (VTN*)listGetRef(leaves, pos);
	listNulData(leaves, t);
	listDel(leaves, pos);

	return t;
}

void EATStart(DFrameNum* df, int* X, int nX, int* Y, int nY,
              VTREE* tree, VLIST* leaves, VLIST* treeAlphaList)
{
	int pos, iCol;
	NEAT* t0;
	TREEALPHA* TAlpha = (TREEALPHA*)malloc(sizeof(TREEALPHA));

	if (!TAlpha)
		EATErr("EATStart [memory fault (TAlpha)]");

	// INICIALIZANDO NODO RAIZ ****************************************************
	if (!(t0 = (NEAT*)malloc(sizeof(NEAT))))
		EATErr("EAT [memory fault-2]");
	t0->id = 0;
	t0->idF = -1;
	NEATSet(t0, nX, nY, df->nRows);

	// inicializar 'a' con los valores manimos...
	for (iCol = 0; iCol < nX; iCol++)
		t0->a[iCol] = dfNumColumMinVal(df, X[iCol], &pos);

	// Calcular la estimacion (max) para el nodo raiz
	for (iCol = 0; iCol < nY; iCol++)
		t0->y[iCol] = dfNumColumMaxVal(df, Y[iCol], &pos);

	t0->R = QuadErr(df, t0->index, t0->nIndex, Y, t0->y, nY);
	treeAppendRoot(tree, t0);

	// TreeAlphList, primer arbol = Nodo Raiz (size=1)
	TAlpha->alpha = Alpha(tree);
	TAlpha->score = INFINITY;
	TAlpha->errFolders = 0.0;
	TAlpha->tree = treeNew(NEATPrint, NEATNew, NEATDel, NEATCpy);
	treeCopy(TAlpha->tree, tree);
	listAppend(treeAlphaList, TAlpha);

	// Inicializar lista de hojas "leaves" con el nodo raiz
	listAppend(leaves, tree->first);
}

int CheckLeaves(VLIST* leaves, int numStop)
{
	int n, numNoFinalLeaves = 0;
	VListIter iter;
	void* data;

	listRefMoveLast(leaves, &iter);
	for (n = 0; n < leaves->size; n++)
	{
		if (((NEAT*)(((VTN*)(iter->data))->data))->nIndex > numStop)
		{
			numNoFinalLeaves++;
			data = iter->data;
			iter->data = NULL;
			listDelIter(leaves, &iter, -1);
			listAdd(leaves, 0, data);
		}
		else
			listRefMovePrev(leaves, &iter);
	}
	return numNoFinalLeaves;
}

void EAT(DFrameNum* df, int* X, int nX, int* Y, int nY, int numStop, VTREE* tree, VLIST* treeAlphaList)
{
	int numNoFinalLeaves = 0;
	NEAT* TL, * TR;
	VTN* t;
	NEAT* tData;

	VLIST *leaves = listNew(NEATPrintVTN, NEATNewVTN, NEATDelVTN, NEATCpyVTN);
	TREEALPHA TAlpha;

	// Inicialize tree structure
	EATStart(df, X, nX, Y, nY, tree, leaves, treeAlphaList);
	numNoFinalLeaves = leaves->size; // 'leaves' contiene al noda raiz que SÍ es no-final

	TAlpha.tree = tree;
	TAlpha.score = INFINITY;
	TAlpha.alpha = ((TREEALPHA*)(treeAlphaList->first->data))->alpha;
	TAlpha.errFolders = ((TREEALPHA*)(treeAlphaList->first->data))->errFolders;

	while (numNoFinalLeaves)
	{
		// SELECT FATHER .....
		t = SelectFather(leaves, numNoFinalLeaves);
		tData = (NEAT*)t->data;
		numNoFinalLeaves--;

		Split(tData, leaves, df, X, Y, &TL, &TR);
		TL->id = tree->size;
		TR->id = tree->size + 1;

		treeAppendSons(t, TL, TR, tree);

		if (IsFinalNode(TR, df, X, numStop))
		{
			listAppend(leaves, t->right); // Nodo final: Se pone al final de 'leaves'
			TAlpha.errFolders += TR->R;
		}
		else
		{
			listAdd(leaves, 0, t->right); // Nodo no-final: Se pone al principio de 'leaves'
			numNoFinalLeaves++;
		}
		if (IsFinalNode(TL, df, X, numStop))
		{
			listAppend(leaves, t->left); // Nodo final: Se pone al final de 'leaves'
			TAlpha.errFolders += TL->R;
		}
		else
		{
			listAdd(leaves, 0, t->left); // Nodo no-final: Se pone al principio de 'leaves'
			numNoFinalLeaves++;
		}

		TAlpha.alpha = Alpha(tree);
		listAddCopy(treeAlphaList, 0, &TAlpha);
	}
}


int CheckEAT(VTREE* tree)
{
	VTreeIter it1, it2;
	VTN* t1, * t2;
	NEAT* n1, * n2;
	int resComp, check = 1;
	int i, nY = ((NEAT*)tree->first->data)->nY;

	treeIterSet(tree, &it1, LEVELS, START);
	while ((t1 = treeIterNextNode(&it1)))
	{
		if (t1->left == NULL) // es nodo hoja
		{
			n1 = (NEAT*)t1->data;
			treeIterSet(tree, &it2, LEVELS, START);
			while ((t2 = treeIterNextNode(&it2)))
			{
				if (t2->left == NULL) // es nodo hoja
				{
					n2 = (NEAT*)t2->data;
					resComp = ComparePareto(n1, n2);
					for (i = 0; i < nY; i++)
					{
						if (resComp == -1)
						{
							if (n1->y[i] <= n2->y[i])
							{
								//printf("OK");
							}
							else
							{
								printf("ERR (%d)\n", resComp);
								NEATPrint(n1);
								NEATPrint(n2);
								NEATPrint((NEAT*)(t2->father->data));
								printf("-----------------------------------------\n\n");
								check = 0;
								break;
							}
						}
						else if (resComp == 1)
						{
							if (n1->y[i] >= n2->y[i])
							{
								//printf("OK");
							}
							else
							{
								printf("ERR (%d)\n", resComp);
								NEATPrint(n1);
								NEATPrint(n2);
								printf("-----------------------------------------\n\n");
								check = 0;
								break;
							}
						}
						else
						{
							//printf("ni fu no fa...");
						}
					}
				}
			}
		}
	}
	return check;
}

/****************************************************************************************
 ########################## Pruning EAT ##########################
*****************************************************************************************/
double* Predictor(VTREE* tree, double* reg)
{
	VTN* ti = tree->first; // Nodo raiz del arbol

	while (ti->left != NULL) // Mientras 'ti' no sea nodo hoja
	{
		if (reg[((NEAT*)ti->data)->xi] < ((NEAT*)ti->data)->s)
			ti = ti->left;
		else
			ti = ti->right;
	}

	return ((NEAT*)ti->data)->y;
}

void TreesForRCV(DFrameNum* df, int* X, int nX, int* Y, int nY, int folder, int numStop, VLIST** TAiv)
{
	int v, N = df->nRows;
	int numRowsFolder = N / folder;
	DFrameNum notLv; // datos para Trainig (sub 'v')

	VTREE Tiv;
	treeIniAll(&Tiv, NEATPrint, NEATNew, NEATDel, NEATCpy);

	dfNumSetPartition(&notLv, df, (N - numRowsFolder));
	for (v = 0; v < folder; v++)
	{
		TAiv[v] = listNew(TreeAlphaPrint, TreeAlphaNew, TreeAlphaDel, TreeAlphaCpy);
		dfNumPartitionRest(&notLv, df, v, numRowsFolder);
		
		EAT(&notLv, X, nX, Y, nY, numStop, &Tiv, TAiv[v]);

		treeFree(&Tiv);
	}
	free(notLv.data);
}

double RCV(DFrameNum* Lv, int* Y, int nY, double alphaIprim, int folder,
	VLIST** TAiv, TREEALPHA** BestTivs)
{
	int reg, v, i;
	double* predictor, Rcv = 0.0;
	VListIter iter;

	TREEALPHA* Tiv, * TivAux;

	for (v = 0; v < folder; v++)
	{
		Tiv = (TREEALPHA*)TAiv[v]->first->data;
		listRefMoveFirst(TAiv[v], &iter);
		while ((TivAux = listRefMoveNext(TAiv[v], &iter)) != NULL && TivAux->tree->size > 1)
		{
			if (TivAux->alpha <= alphaIprim)
				Tiv = TivAux;
		}

		BestTivs[v] = Tiv;
		//TEST +++++++++++++++++++++++++++++
		for (reg = 0; reg < Lv[v].nRows; reg++)
		{
			predictor = Predictor(Tiv->tree, Lv[v].data[reg]);

			for (i = 0; i < nY; i++)
				Rcv += (Lv[v].data[reg][Y[i]] - predictor[i])
				* (Lv[v].data[reg][Y[i]] - predictor[i]);
		}
	}
	return Rcv;
}

// *****************************  Pruning EAT *********************************
TREEALPHA* EATPruning(VLIST* treeAlphaList, DFrameNum* df,
	int* X, int nX, int* Y, int nY, int folder, int numStop)
{
	int v, N = df->nRows;
	double alphaIprim;
	int numRowsFolder = N / folder;

	DFrameNum* Lv = (DFrameNum*)malloc(sizeof(DFrameNum) * folder);
	if (!Lv)
		EATErr("EATPruning [memory fault-1]");
	for (v = 0; v < folder; v++)
	{
		dfNumSetPartition(&Lv[v], df, numRowsFolder);
		dfNumPartition(&Lv[v], df, v, numRowsFolder);
	}

	//******************************************************************************
	// PRUNING
	TREEALPHA* ta1, * ta2, * Tk;
	VListIter iter;
	VLIST** TAiv = (VLIST**)malloc(sizeof(VLIST*) * folder);
	TREEALPHA** BestTivs = (TREEALPHA**)malloc(sizeof(TREEALPHA*) * folder);
	TREEALPHA** BestTivsAux = (TREEALPHA**)malloc(sizeof(TREEALPHA*) * folder);

	if (TAiv == NULL || BestTivs == NULL || BestTivsAux == NULL)
		EATErr("TAiv / BestTivs / BestTivsAux is NULL");

	TreesForRCV(df, X, nX, Y, nY, folder, numStop, TAiv);

	listRefMoveFirst(treeAlphaList, &iter);
	Tk = ta1 = listRefMoveNext(treeAlphaList, &iter);

	BestTivs[0] = NULL;
	while (ta1 != NULL && (ta2 = listRefMoveNext(treeAlphaList, &iter)) != NULL && ta2->tree->size > 1)
	{
		alphaIprim = sqrt(ta1->alpha * ta2->alpha);
		ta1->score = RCV(Lv, Y, nY, alphaIprim, folder, TAiv, BestTivsAux) / N / nY;

		if (Tk->score > ta1->score || BestTivs[0] == NULL)
		{
			Tk = ta1; // TREEALPHA con el menor score
			for (v = 0; v < folder; v++)
				BestTivs[v] = BestTivsAux[v];
		}
		ta1 = ta2;
	}

	//******************************************************************************
	// SERule (vuelve a hacer Cross-Validation... solo Validation)
	int reg, i;
	double SE, s2 = 0.0; // 's' al cuadrado
	double* predictor;
	double dif1, dif2;

	for (v = 0; v < folder; v++)
	{

		for (reg = 0; reg < numRowsFolder; reg++)
		{
			predictor = Predictor(BestTivs[v]->tree, Lv[v].data[reg]);
			for (i = 0; i < nY; i++)
			{
				dif1 = (Lv[v].data[reg][Y[i]] - predictor[i]);
				dif2 = dif1 * dif1 - Tk->score;
				s2 += dif2 * dif2;
			}
		}
	}

	for (v = 0; v < folder; v++)
		free(Lv[v].data);
	free(Lv);
	SE = sqrt(s2 / N / N / nY / nY);

	//******************************************************************************
	// Seleccionar arbol definitivo: el de menor tamanno con un score con holgura SE
	double margin = Tk->score + SE;

	printf("Tk->score=%f (size:%d)\n", Tk->score, Tk->tree->size);
	printf("SE = %f\n", SE);
	printf("margin = %f\n", margin);

	listRefMoveLast(treeAlphaList, &iter);
	while ((Tk = listRefMovePrev(treeAlphaList, &iter)) != NULL)
		if (Tk->score <= margin)
			break;

	listNulData(treeAlphaList, Tk);
	listFree(treeAlphaList);
	for (v = 0; v < folder; v++)
		listFree(TAiv[v]);
	free(BestTivsAux);
	free(BestTivs);
	free(TAiv);
	return Tk;
}
/****************************************************************************************/

