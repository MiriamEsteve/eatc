/*
DATA FRAME...
*/

#ifndef _DATA_FRAME__
#define _DATA_FRAME__



/************************************************************************************************************************
 ################################ Lectura de un fichero -> #################################
 ########################### matriz de datos de tipo DOUBLE  ##############################
**************************************************************************************************************************/
// *********************** Estructura DataFrame *****************************
typedef struct
{
	int nRows, nCols;
	char **header;	 // Array de cadenas de caracteres
	double **data;	 // Matriz de doubles
} DFrameNum;

// ************************** Funciones DataFrame ***************************
void DATAErr(char *err); // Mensajes de error DataFrame

// ********************* Funciones DataFrame NUMÉRICO ***********************
void dfNumIni(DFrameNum *df);
DFrameNum* dfNumNew(int rows, int cols, char **head);
DFrameNum* dfNumLoad(char *fileName, char *sep);
void dfNumDel(DFrameNum *df);
void dfNumFree(DFrameNum *df);
void dfNumPrint(DFrameNum *df);
void dfNumPrintFile(DFrameNum *df, char *nameFile);

void dfNumRandomize(DFrameNum *df); // BARAJAR registros
void dfNumSetPartition(DFrameNum *df1, DFrameNum *df2, int nRowsPart);
void dfNumPartition(DFrameNum *df1, DFrameNum *df2, int v, int length);
void dfNumPartitionRest(DFrameNum *df1, DFrameNum *df2, int v, int length);
void dfNumPartitionID(DFrameNum *df1, DFrameNum *df2, int ini, int length);
void dfNumPartitionIDRest(DFrameNum *df1, DFrameNum *df2, int ini, int length);
void dfNumBaging(DFrameNum *df1, DFrameNum *df2);
void dfNumPartitionDel(DFrameNum *df1);

double dfNumColumMaxVal(DFrameNum *df, int col, int *pos);
double dfNumColumMaxValIndex(DFrameNum *df, int *index, int nIndex, int col);
double dfNumColumMinVal(DFrameNum *df, int col, int *pos);
double dfNumColumMinValIndex(DFrameNum *df, int *index, int nIndex, int col);
double dfNumColumAvgVal(DFrameNum *df, int col);
double dfNumColumAvgValIndex(DFrameNum *df, int *index, int nIndex, int col);

void dfNumSetColumn(DFrameNum *df, int col, double *arr);
void dfNumAddColumn(DFrameNum *df, int pos, char *name, double val);
void dfNumCpyColumn(DFrameNum *df, DFrameNum *df2, int pos, int pos2);

#endif

