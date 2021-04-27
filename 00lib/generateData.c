# include <complex.h>
# include <stdlib.h>
# include <stdio.h>
# include <time.h>
# include <math.h>
#include "00lib/data.h"

////////////////////////////
// DISTRIBUCIONES DE PROB //
////////////////////////////

double rand_uniform_distribution(double rangeLow, double rangeHigh) {
    double myRand = (double)rand() / RAND_MAX;
    double range = rangeHigh - rangeLow;
    double myRand_scaled = (myRand * range) + rangeLow;
    return myRand_scaled;
}

double rand_normal_distribution(double mean, double standardDesviation) {
    // Box–Muller tranformacion
    double v1 = rand_uniform_distribution(0,1);
    double v2 = rand_uniform_distribution(0,1);
    double z = cos(2 * 3.14 * v2) * sqrt(-2. * log(v1));
    return z * standardDesviation + mean;
}


/////////////////////////
// FUNCIONES AUXLIARES //
/////////////////////////

double* generate_column_input(int N_DMU, double rangeLow, double rangeHigh) {
    double* input = (double*)malloc(N_DMU * sizeof(double));
    for (int i = 0; i < N_DMU; i++) {
        input[i] = rand_uniform_distribution(rangeLow, rangeHigh);
    }
    return input;
}

void generate_inputs(int N_DMU, int nX, DFrameNum* df, double rangeLow, double rangeHigh) {
    for (int col = 0; col < nX; col++)
    {
        double* arr = generate_column_input(N_DMU, rangeLow, rangeHigh);
        dfNumSetColumn(df, col, arr);
    }
}

double* generate_column_monooutput_god(int N_DMU, int nX, DFrameNum* df) {
    double* outputDios = (double*)malloc(N_DMU * sizeof(double));
    int i = 0;
    if (nX == 3) {
        for (i = 0; i < N_DMU; i++) {
            outputDios[i] = 3 * log(df->data[i][0]) * log(df->data[i][1]) * log(df->data[i][2]);
        }
    }
    else if (nX == 6) {
        for (i = 0; i < N_DMU; i++) {
            outputDios[i] = 3 * pow(df->data[i][0], 0.05) * pow(df->data[i][1], 0.001) * pow(df->data[i][2], 0.004);
            outputDios[i] *= pow(df->data[i][3], 0.045) * pow(df->data[i][4], 0.1) * pow(df->data[i][5], 0.3);
        }
    }
    else if (nX == 9) {
        for (i = 0; i < N_DMU; i++) {
            outputDios[i] = 3 * pow(df->data[i][0], 0.05) * pow(df->data[i][1], 0.001) * pow(df->data[i][2], 0.004);
            outputDios[i] *= pow(df->data[i][3], 0.005) * pow(df->data[i][4], 0.001) * pow(df->data[i][5], 0.004);
            outputDios[i] *= pow(df->data[i][6], 0.08) * pow(df->data[i][7], 0.1) * pow(df->data[i][8], 0.3);
        }
    }
    else if (nX == 12) {
        for (i = 0; i < N_DMU; i++) {
            outputDios[i] = 3 * pow(df->data[i][0], 0.005) * pow(df->data[i][1], 0.001) * pow(df->data[i][2], 0.004);
            outputDios[i] *= pow(df->data[i][3], 0.005) * pow(df->data[i][4], 0.001) * pow(df->data[i][5], 0.004);
            outputDios[i] *= pow(df->data[i][6], 0.08) * pow(df->data[i][7], 0.05) * pow(df->data[i][8], 0.05);
            outputDios[i] *= pow(df->data[i][9], 0.075) * pow(df->data[i][10], 0.025) * pow(df->data[i][11], 0.2);
        }
    }
    else if (nX == 15) {
        for (i = 0; i < N_DMU; i++) {
            outputDios[i] = 3 * pow(df->data[i][0], 0.005) * pow(df->data[i][1], 0.001) * pow(df->data[i][2], 0.004);
            outputDios[i] *= pow(df->data[i][3], 0.005) * pow(df->data[i][4], 0.001) * pow(df->data[i][5], 0.004);
            outputDios[i] *= pow(df->data[i][6], 0.08) * pow(df->data[i][7], 0.05) * pow(df->data[i][8], 0.05);
            outputDios[i] *= pow(df->data[i][9], 0.05) * pow(df->data[i][10], 0.025) * pow(df->data[i][11], 0.025);
            outputDios[i] *= pow(df->data[i][12], 0.025) * pow(df->data[i][13], 0.025) * pow(df->data[i][14], 0.15);
        }
    }
    return outputDios;
}

double* generate_column_monooutput_inefficiency(int N_DMU, double *outputDios) {
    double* output = (double*)malloc(N_DMU * sizeof(double));
    double u = fabs(rand_normal_distribution(0, 0.4));
    for (int i = 0; i < N_DMU; i++)
    {
        output[i] = outputDios[i] * exp(-u);
    }
    return output;
}


///////////////////
// GENERATE DATA //
///////////////////

int generate_data_mono(int seed, int N_DMU, int nX, int nY, DFrameNum* df) {

    srand(seed); // cambiar la semilla

    int i, j = 0;
    int errorCode = 0;

    if (nY == 1) { // CASOS MONO OUTPUT

        // generar los inputs
        generate_inputs(N_DMU, nX, df, 1, 10);

        // generar los outputs
        if (nX == 3 || nX == 6 || nX == 9 || nX == 12 || nX == 15) {
            double* outputDios = generate_column_monooutput_god(N_DMU, nX, df);
            double* output = generate_column_monooutput_inefficiency(N_DMU, outputDios);
            dfNumSetColumn(df, nX, output);
            dfNumSetColumn(df, nX + 1, outputDios);
        }
        else {
            errorCode = 1;
        }
    }
    else {
        errorCode = 2;
    }

    if (errorCode) {
        printf("\nLa combinacion de %d inputs y %d outputs no es valida \n", nX, nY);
    }
    return errorCode;

}

// frontier: tanto por uno de DMUs ineficientes (contrario al tanto por uno de DMUs en la frontera)
// noise: ruido aleatorio o no (bool)
int generate_data_multi(int seed, int N_DMU, int nX, int nY, DFrameNum* df, float frontier, int noise) {

    srand(seed); // cambiar la semilla

    int i, j, k = 0;
    int errorCode = 0;

    if (nX == 2 && nY == 2) { // CASO MULTI OUPUT

        // PASO 1.1:generar los inputs
        generate_inputs(N_DMU, nX, df, 5, 50);

        // generar los outputs 
        double z, op1, op2, op3 = 0;
        double ln_x1, ln_x2, ln_y1_ast = 0;
        int index = 0;
        double half_normal, normal1, normal2 = 0;
        for (int i = 0; i < N_DMU; i++) {
            // PASO 1.2
            z = rand_uniform_distribution(-1.5, 1.5); //ln (y2*/y1*)
            ln_x1 = log(df->data[i][0]);
            ln_x2 = log(df->data[i][1]);
            // PASO 2 y 3: solucionar la ecuacion
            op1 = -1 + 0.5 * z + 0.25 * pow(z, 2) - 1.5 * ln_x1;
            op2 = -0.6 * ln_x2 + 0.2 * pow(ln_x1, 2) + 0.05 * pow(ln_x2, 2) - 0.1 * ln_x1 * ln_x2;
            op3 = 0.05 * ln_x1 * z - 0.05 * ln_x2 * z;
            ln_y1_ast = -(op1 + op2 + op3);
            // reales
            df->data[i][nX] = exp(ln_y1_ast); // y1*
            df->data[i][nX + 1] = exp(ln_y1_ast + z); //y2*
            // god
            df->data[i][nX + 2] = exp(ln_y1_ast);   // y1*
            df->data[i][nX + 3] = exp(ln_y1_ast + z); //y2*
        }
        // PASO 4:  Ineficiencias en las DMUs
        if (frontier > 0) {
            if (frontier > 1) frontier = 1;
            // seleccionamos indices de DMUs ineficientes
            int numDMUsIneficientes = N_DMU * frontier;
            int* indexDMUsIneficientesput = (int*)malloc(numDMUsIneficientes * sizeof(int));
            int newIndex = 0;
            for (j = 0; j < numDMUsIneficientes; j++) {
                while (!newIndex) {
                    index = (int)rand_uniform_distribution(0, numDMUsIneficientes);
                    for (k = 0; k < j; k++) {
                        if (indexDMUsIneficientesput[k] == index) {
                            break;
                        }
                    }
                    if (k == j) {
                        newIndex = 1;
                        indexDMUsIneficientesput[k] = index;
                    }
                }
                newIndex = 0;
            }
            // half-normal
            half_normal = fabs(rand_normal_distribution(0, pow(0.3, 1 / 2)));
            for (j = 0; j < numDMUsIneficientes; j++) {
                index = indexDMUsIneficientesput[j];
                df->data[index][nX] /= exp(half_normal);
                df->data[index][nX+1] /= exp(half_normal);
                // PASO 5: añadimos el ruido
                if (noise) {
                    normal1 = rand_normal_distribution(0, pow(0.01, 1 / 2));
                    normal2 = rand_normal_distribution(0, pow(0.01, 1 / 2));
                    df->data[index][nX] /= exp(normal1);
                    df->data[index][nX + 1] /= exp(normal2);
                }
            }
        }

    }
    else {
        errorCode = 2;
    }
    if (errorCode) {
        printf("\nLa combinacion de %d inputs y %d outputs no es valida \n", nX, nY);
    }
    return errorCode;
}

