#include "00lib/data.h"
#include "00lib/vtree.h"
#include <stdlib.h>

// ESCENARIO MONTECARLO
void EscMontecarlo(DFrameNum *df)
{
	
}

// ESCENARIO COBB DOUGLAS
void EscCobbDouglas(DFrameNum *df)
{
	int numX=df->nCols-2; // se resta 'y', 'yD'
	int y =df->nCols-2;
	int yD=df->nCols-1;

	double alpha = 0.5/numX;
	double lambda = 3.0;
	int r, c;

	for(r=0 ; r<df->nRows ; r++)
	{
		// Función de 'dios'
		df->data[r][yD]=1;
		for(c=0 ; c<numX ; c++)
			df->data[r][yD] *= pow( df->data[r][c] , alpha);

		// Ineficiencia
		df->data[r][y] = df->data[r][yD] * exp( log(1-(rand()/(1.0+RAND_MAX)))/lambda );
	}
}

// CONSTRUIR DATAFRAME ALEATORIO SEGUN 'ESCENARIO'
DFrameNum* MakeDF(int nRows, int nCols, void  (*funcEscenario)(DFrameNum*))
{
	DFrameNum *df;
	double *arr;
	char **head, aux[50];
	int y = nCols-2;
	int yD= nCols-1;
	int c;

	arr=(double*)malloc(sizeof(double)*nRows);
	head=(char**)malloc(sizeof(char*) *nCols);
	if(!arr || !head)
	{
		printf("\n\n*** ERROR MAKEFILE-1\n\n");
		exit(0);
	}

	// CABECERA DEL DATAFRAME: X0 X1 X2 ... y yD
	for(c=0 ; c<y ; c++)
	{
		sprintf(aux, "X%d", c);
		if( !(head[c]=strdup(aux)) )
		{
			printf("\n\n*** ERROR MAKEFILE-2\n\n");
			exit(0);
		}
	}
	head[y]=strdup("y");
	head[yD]=strdup("yD");
	if(!head[y] || !head[yD])
	{
		printf("\n\n*** ERROR MAKEFILE-3\n\n");
		exit(0);
	}

	// CREA Y COMPLETA DATOS EN EL DATAFRAME
	df = dfNumNew(nRows, nCols, head);
	for(c=0 ; c<y ; c++)
	{
		RandUniform(0, 1, arr, nRows);
		dfNumSetColumn(df, c, arr);
	}
	funcEscenario(df); // Llama a la función para rellenar 'y'/'yD' segun escenario

	return df;
}

// CÁLCULOS FDH
double FDH(DFrameNum *df, double *xArr, int nArr, int y)
{
	int i, n, newMax;
	double yMax = -1.0/0.0;

	if(nArr != df->nCols-2)
	{
		printf("ERROR FDH, fallo en los datos de entrada...");
		exit(0);
	}

	for(n=0 ; n<df->nRows ; n++)
	{
		newMax=1;
		for(i=0 ; i<nArr ; i++)
		{
			if(i<y)
			{
				if(df->data[n][i] > xArr[i])
				{
					newMax=0;
					break;
				}
			}
			else if(i>y)
			{
				if(df->data[n][i+1] > xArr[i])
				{
					newMax=0;
					break;
				}
			}	
		}
		if(newMax && yMax < df->data[n][y])
			yMax=df->data[n][y];
	}
	return yMax;
}

// AÑADIR COLUMNA FDH
void FDHPredictor(DFrameNum *df, int y, char *nameCol)
{
	int r;
	int nX = y;
	int yFDH = df->nCols;

	dfNumAddColumn(df, yFDH, nameCol, 0.0);
	for(r=0 ; r<df->nRows ; r++)
		df->data[r][yFDH] = FDH(df, df->data[r], nX, y);
}

void EATPredictor(DFrameNum *df, VTREE *tree, char *nameCol)
{
	int r;
	int yTREE = df->nCols;

	dfNumAddColumn(df, yTREE, nameCol, 0.0);
	for(r=0 ; r<df->nRows ; r++)
	{
		//df->data[r][yTREE] = Predictor(tree, df->data[r]);
	}
}

DFrameNum* MakeDF_X2Y2(int nRows, double min, double max,
                 double minZ, double maxZ, double border, int noise)
{
	DFrameNum *df;
	int i;
	double lnX1, lnX2, op1, op2, op3, lnY1ast;
	double *z, *halfNormal, *normal1, *normal2;
	int nIndex, *index;
	char *header[] = {"X1", "X2", "Y1", "Y2"};

	// Crear df con 4 columnas, 2 inputs y 2 outputs
	df = dfNumNew(nRows, 4, header);
	z = (double*)malloc(sizeof(double)*nRows);

	// Calcular X1
	RandUniform(min, max, z, nRows);
	dfNumSetColumn(df, 0, z);
	// Calcular X2
	RandUniform(min, max, z, nRows);
	dfNumSetColumn(df, 1, z);

	// Calcular Z
	RandUniform(minZ, maxZ, z, nRows);

	for(i=0 ; i<nRows ; i++)
	{
		lnX1 = log(df->data[i][0]);
		lnX2 = log(df->data[i][1]);
		
		op1 = -1.0 + 0.5 * z[i] + 0.25 * z[i] * z[i] - 1.5*lnX1;
		op2 = -0.6 * lnX2 + 0.2* lnX1*lnX1 + 0.05 * lnX2*lnX2 - 0.1 * lnX1 *lnX2;
		op3 = 0.05 * lnX1 * z[i] - 0.05 * lnX2 * z[i];
		lnY1ast = -op1 -op2 -op3;
		
		// Y1
		df->data[i][2] = exp(lnY1ast);
		// Y2
		df->data[i][3] = exp(lnY1ast+z[i]);
	}

	// UDMs en la frontera: 'border' -> % de DMUs que hay en la frontera
	if(border>0.0 && border <1.0)
	{
		nIndex = df->nRows*border;
		index = GetRandomIndex(0, df->nRows, nIndex, 0);

		halfNormal = (double*)malloc(sizeof(double)*nIndex);
		RandExpNormal(0, sqrt(0.3), halfNormal, nIndex);
		if(noise)
		{
			normal1 = (double*)malloc(sizeof(double)*nIndex);
			normal2 = (double*)malloc(sizeof(double)*nIndex);
			RandExpNormal(0, sqrt(0.01), normal1, nIndex);
			RandExpNormal(0, sqrt(0.01), normal2, nIndex);
		}

		for(i=0 ; i<nIndex ; i++)
			if(noise)
			{
				// Y1
				df->data[ index[i] ][2] /= (halfNormal[i]*normal1[i]);
				// Y2
				df->data[ index[i] ][3] /= (halfNormal[i]*normal2[i]);
			}
			else
			{
				// Y1
				df->data[ index[i] ][2] /= halfNormal[i];
				// Y2
				df->data[ index[i] ][3] /= halfNormal[i];
			}


		if(noise)
		{
			free(normal1);
			free(normal2);
		}

		free(halfNormal);
		free(index);
	}
	
	free(z);
	return df;
}






