#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "00lib/distrib.h"
#include "00lib/data.h"
#include "00lib/vtree.h"
#include "00lib/eatStructure.h"
#include "eat.h"
#include "scores.h"
#include "generateData.h"

int montecarlo_mono() {

	//int N_Inputs[5] = { 3, 6, 9, 12, 15 };
	//int N_DMU[5] = { 50, 100, 150, 200, 500};
	int n_inputs[3] = { 6, 9, 12 }; // vector con num variables de entrada
	int n_DMU[2] = { 50 , 100 }; // vector con num de DMUs
	int n_experiments = 5; // num de experimentos

	// VAR DATASET
	int seed = 0;
	int nRows; // num de DMUs
	int nCols; // num de variables
	int nX; // num variable de input
	int nY = 1; // num varibales de output
	int* X; // vector con el indice de las variables de input
	int Y[] = { 0 }; // vector con el indice de las variables de output
	char** header; // cabecera del dataframe
	DFrameNum* df; // data frame

	// VAR EAT
	int numStop = 5;
	int folder = 5;
	int J = 5; //EATBoost depth tree
	VLIST* treeAlphaList;
	VTREE* tree;
	TREEALPHA* Tk;

	// VAR MEDIDAS ERROR
	double mse, bias = 0;
	double eatResta = 0;
	double* input;
	double* pred;

	// VAR ESCRIBIR RESULTADOS
	FILE* fichero_RAM;
	char* filename = "monteCarlo-mono.csv";
	fichero_RAM = fopen(filename, "wb+");
	if (!fichero_RAM) {
		printf("ERROR [file open???]");
		return 1;
	}
	fprintf(fichero_RAM, "Inputs;Outputs;DMUs;Experimento;MSE;Bias");

	// VAR AUXILIARES
	int k = 0;
	char str[3];

	srand(time(NULL));


	for (int i = 0; i < sizeof(n_inputs)/sizeof(int); i++) { // num inputs
		for (int j = 0; j < sizeof(n_DMU)/sizeof(int); j++) { // num DMUs
			for (int n = 0; n < n_experiments; n++) { // experimentos
				
				printf("EXPERIMENTO nX: %d - n_DMU: %d - it: %d\n", n_inputs[i], n_DMU[j], n);

				// GENERAR EL DATASET PARA EL EXPERIMENTO
				seed = rand();
				nX = n_inputs[i];
				nRows = n_DMU[j];
				nCols = nX + 1 + 1; // nX + y real + y dios
				X = (int*)malloc(nX * sizeof(int));
				for (k = 0; k < nX; k++) {
					X[k] = k;
				}
				Y[0] = nX;
				header = (char**)malloc(nCols * sizeof(char*));
				header[nX] = "Y1";
				header[nX + 1] = "YD";
				for (k = 0; k < nX; k++) {
					sprintf(str, "X%d", k);
					char* str2 = strdup(str);
					header[k] = str2;
				}
				df = dfNumNew(nRows, nCols, header);
				int errorDF = generate_data_mono(seed, nRows, nX, nY, df);
				if (errorDF) {
					printf("ERROR [generate_data_mono]");
					return 2;
				}
				//dfNumPrint(df);

				// CALCULAR EAT
				treeAlphaList  = listNew(TreeAlphaPrint, TreeAlphaNew, TreeAlphaDel, TreeAlphaCpy);
				tree = treeNew(NEATPrint, NEATNew, NEATDel, NEATCpy);

				EAT(df, X, nX, Y, nY, numStop, tree, treeAlphaList);
				//treePrint(tree);
				if (!CheckEAT(tree)) {
					printf("ERROR [deep EAT]");
					return 3;
				}

				Tk = EATPruning(treeAlphaList, df, X, nX, Y, nY, folder, numStop);
				//treePrint(Tk->tree);
				if (!CheckEAT(Tk->tree)) {
					printf("ERROR [pruned EAT]");
					return 4;
				}
				

				// CALCULAR MSE Y BIAS
				mse = 0;
				bias = 0;
				input = (double*)malloc(nX * sizeof(double));
				for (k = 0; k < nRows; k++) {
					for (int t = 0; t < nX; t++) {
						input[t] = df->data[k][t];
					}
					pred = Predictor(Tk->tree, input);
					eatResta = df->data[k][nX] - pred[0];
					mse += pow(eatResta, 2);
					bias += fabs(eatResta);
				}
				mse /= nRows;
				bias /= nRows;

				printf("MSE: %f\n", mse);
				printf("Bias: %f\n\n", bias);

				//ESCRIBIR RESULTADOS
				fprintf(fichero_RAM, "%d;%d;%d;%d;%f;%f\n", nX, nY, nRows, n+1, mse, bias);

			}
		}
	}

	// FREES
	free(X);
	free(header);
	free(input);
	dfNumFree(df);
	treeFree(tree);
	treeFree(Tk->tree);
	listFree(treeAlphaList);

	return 0;
}


int montecarlo_multi() {

	//int N_DMU[5] = { 50, 100, 150, 200, 500};
	int n_DMU[2] = { 50 , 100 }; // vector con num de DMUs
	float n_frontier[2] = { 1, 0.9 };
	int n_experiments = 5; // num de experimentos

	// VAR DATASET
	int seed = 0;
	int nRows; // num de DMUs
	int nCols = 6; // num de variables = 2 inputs + 2 outputs + 2 output dios
	int nX = 2; // num variable de input
	int nY = 2; // num varibales de output
	int X[2] = { 0 , 1 }; // vector con el indice de las variables de input
	int Y[2] = { 2, 3 }; // vector con el indice de las variables de output
	char* headerComplete[] = { "X1", "X2", "Y1", "Y2", "Y1D", "Y2D" }; // cabecera del dataframe
	char* headerTheoric[] = { "X1", "X2", "Y1D", "Y2D" };
	float frontier = 0;
	float noise = 0;
	DFrameNum* df; // data frame
	DFrameNum* dfTheoric; // data frame

	// VAR EAT
	int numStop = 5;
	int folder = 5;
	int J = 5; //EATBoost depth tree
	VLIST* treeAlphaList;
	VTREE* tree;
	TREEALPHA* Tk;

	// VAR MEDIDAS ERROR
	double mse, bias = 0;
	double eatResta = 0;
	double* score_pred;
	double* score_theoric;

	// VAR ESCRIBIR RESULTADOS
	FILE* fichero_RAM;
	char* filename = "monteCarlo-multi.csv";
	fichero_RAM = fopen(filename, "wb+");
	if (!fichero_RAM) {
		printf("ERROR [file open???]");
		return 1;
	}
	fprintf(fichero_RAM, "Inputs;Outputs;Frontier;Noise;DMUs;Experimento;MSE;Bias\n");

	// VAR AUXILIARES
	int k = 0;

	srand(time(NULL));


	for (int j = 0; j < sizeof(n_DMU) / sizeof(int); j++) { // num DMUs
		for (int i = 0; i < sizeof(n_frontier) / sizeof(float); i++) {
			for (int n = 0; n < n_experiments; n++) { // experimentos

				printf("EXPERIMENTO n_DMU: %d - frontier: %f, it: %d\n", n_DMU[j], n_frontier[i], n+1);

				// GENERAR EL DATASET PARA EL EXPERIMENTO
				seed = rand();
				nRows = n_DMU[j];
				frontier = n_frontier[i];
				
				// df completo
				df = dfNumNew(nRows, nCols, headerComplete);
				int errorDF = generate_data_multi(seed, nRows, nX, nY, df, frontier, noise);
				if (errorDF) {
					printf("ERROR [generate_data_multi]");
					return 2;
				}
				//dfNumPrint(df);

				// df god
				dfTheoric = dfNumNew(nRows, nCols-2, headerTheoric);
				dfNumCpyColumn(dfTheoric, df, 0, 0);
				dfNumCpyColumn(dfTheoric, df, 1, 1);
				dfNumCpyColumn(dfTheoric, df, 2, 4);
				dfNumCpyColumn(dfTheoric, df, 3, 5);
				//dfNumPrint(dfTheoric);


				// CALCULAR EAT PREDICCION
				treeAlphaList = listNew(TreeAlphaPrint, TreeAlphaNew, TreeAlphaDel, TreeAlphaCpy);
				tree = treeNew(NEATPrint, NEATNew, NEATDel, NEATCpy);

				EAT(df, X, nX, Y, nY, numStop, tree, treeAlphaList);
				//treePrint(tree);
				if (!CheckEAT(tree)) {
					printf("ERROR [deep EAT]");
					return 3;
				}

				Tk = EATPruning(treeAlphaList, df, X, nX, Y, nY, folder, numStop);
				//treePrint(Tk->tree);
				if (!CheckEAT(Tk->tree)) {
					printf("ERROR [pruned EAT]");
					return 4;
				}

				// score prediccion
				score_pred = (double*)malloc(nRows * sizeof(double));
				EAT_BBC_output(nRows, nX, nY, df, Tk->tree, score_pred);


				// CALCULAR EAT PREDICCION
				treeAlphaList = listNew(TreeAlphaPrint, TreeAlphaNew, TreeAlphaDel, TreeAlphaCpy);
				tree = treeNew(NEATPrint, NEATNew, NEATDel, NEATCpy);

				EAT(dfTheoric, X, nX, Y, nY, numStop, tree, treeAlphaList);
				//treePrint(tree);
				if (!CheckEAT(tree)) {
					printf("ERROR [deep EAT]");
					return 3;
				}

				Tk = EATPruning(treeAlphaList, df, X, nX, Y, nY, folder, numStop);
				//treePrint(Tk->tree);
				if (!CheckEAT(Tk->tree)) {
					printf("ERROR [pruned EAT]");
					return 4;
				}

				// score prediccion
				score_theoric = (double*)malloc(nRows * sizeof(double));
				EAT_BBC_output(nRows, nX, nY, df, Tk->tree, score_theoric);

				// CALCULAR MSE Y BIAS
				mse = 0;
				bias = 0;				

				for (k = 0; k < nRows; k++) {
					eatResta = score_theoric[k] - score_pred[k];
					mse += pow(eatResta, 2);
					bias += fabs(eatResta);
				}
				mse /= nRows;
				bias /= nRows;

				printf("MSE: %f\n", mse);
				printf("Bias: %f\n\n", bias);

				//ESCRIBIR RESULTADOS
				fprintf(fichero_RAM, "%d;%d;%f;%d;%d;%d;%f;%f\n", nX, nY, frontier, noise, nRows, n + 1, mse, bias);

			}
		}
	}
	

	// FREES
	dfNumFree(df);
	dfNumFree(dfTheoric);
	treeFree(tree);
	treeFree(Tk->tree);
	listFree(treeAlphaList);

	return 0;
}