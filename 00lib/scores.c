#include <stdio.h>
#include <ilcplex/cplex.h>
#include "eat.h"


/////////////////
// SCORE FDH  //
////////////////

// BBC OUTPUT
void FDH_BBC_ouput(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, double* fdh_scores) {

    // ESCRIBIR EL PROBLEMA EN DISCO

    int i, j, r;

    FILE* fichero_RAM;
    char* filename = "FDH_BBC_output.lp";
    fichero_RAM = fopen(filename, "wb+");
    fprintf(fichero_RAM, "Maximize\n");

    // Funcion objetivo
    fprintf(fichero_RAM, "obj:\t");
    fprintf(fichero_RAM, "phi");

    // Restricciones
    fprintf(fichero_RAM, "\nSubject To");

    // 2.1
    for (i = 0;i < N_Inputs;i++) {
        fprintf(fichero_RAM, "\n i%d: ", i + 1); // nombre de la restriccion
        for (j = 0;j < N_DMU;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", df->data[j][i], j + 1);
        fprintf(fichero_RAM, " <=  %f   ", df->data[0][i]);
    }

    // 2.2
    for (r = N_Inputs;r < N_Outputs+N_Inputs;r++) {
        fprintf(fichero_RAM, "\n r%d: ", r + 1); // nombre de la restriccion
        fprintf(fichero_RAM, "- %f phi ", df->data[0][r]);
        for (j = 0;j < N_DMU;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", df->data[j][r], j + 1);
        fprintf(fichero_RAM, ">= 0 ");
    }

    // 2.3
    fprintf(fichero_RAM, "\n rl: ");
    for (j = 0;j < N_DMU;j++)
        fprintf(fichero_RAM, " +  l%d  ", j + 1);
    fprintf(fichero_RAM, " = 1  ");

    // 2.4
    fprintf(fichero_RAM, "\nBinaries ");
    for (j = 0;j < N_DMU;j++)
        fprintf(fichero_RAM, "\n l%d ", j + 1);


    fprintf(fichero_RAM, "\nEnd\n");

    fclose(fichero_RAM);

    //printf("\nFin escritura en disco del problema FDH");


    // RESOLVER PARA TODAS LAS DMUs

    CPXENVptr env;
    CPXLPptr  lp_RAM;
    double d_temp = 10000.;
    int status = 0;

    env = CPXopenCPLEX(&status); if (status)printf("\n ERROR abrir CPLEX");

    CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_OFF); //mostrar mensajes de control

    // CPXsetdblparam(env, CPX_PARAM_TILIM, 7200.0);  // tiempo maximo que puede estar intentando resolver el problema

    // crear el problema en CPLEX
    lp_RAM = CPXcreateprob(env, &status, "mod_lp"); if (status)printf("\nERROR %d crear problema", status);
    // leer el fichero en disco y usar como problema
    status = CPXreadcopyprob(env, lp_RAM, filename, NULL);if (status)printf("\nERROR %d leer  problema", status);

    //printf("\n\n solucion para todas las DMUs FDH");

    for (j = 0;j < N_DMU;j++)
    {
        // Modificamos problema par la DMU j
        for (i = 0;i < N_Inputs;i++) {
            status = CPXchgcoef(env, lp_RAM, i, -1, df->data[j][i]);
        }
        for (r = N_Inputs;r < N_Outputs+N_Inputs;r++) {
            //status = CPXchgcoef(env, lp_RAM, r + N_Inputs, -1, outputs[j][r]);
            status = CPXchgcoef(env, lp_RAM, r, 0, -df->data[j][r]);
        }

        // Resolvemos con CPLEX
        status = CPXmipopt(env, lp_RAM);if (status)printf("\n ERROR resolver problema DMU %d con código de error %d", j + 1, status);

        status = CPXgetobjval(env, lp_RAM, &d_temp);if (status)printf("\n ERROR obtener solucion DMU %d ", j + 1);
        // if (j == 1) CPXwriteprob(env, lp_RAM, "FDH_BBC_output.lp", NULL);

        fdh_scores[j] = d_temp;
        //printf("\n La solucion para la DMU %d es %f", j + 1, d_temp);

    }

}


///////////////////////
// SCORE DEA OUTPUT //
//////////////////////

// BBC OUTPUT
void DEA_BBC_output(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, double* dea_scores) {

    // ESCRIBIR EL PROBLEMA EN DISCO

    int i, j, r;

    FILE* fichero_RAM;
    char* filename = "DEA_DF.lp";
    fichero_RAM = fopen(filename, "wb+");
    fprintf(fichero_RAM, "Maximize\n");

    // Funcion objetivo
    fprintf(fichero_RAM, "obj:\t");
    fprintf(fichero_RAM, "phi");

    // Restricciones
    fprintf(fichero_RAM, "\nSubject To");

    // 2.1
    for (i = 0;i < N_Inputs;i++) {
        fprintf(fichero_RAM, "\n i%d: ", i + 1); // nombre de la restriccion
        for (j = 0;j < N_DMU;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", df->data[j][i], j + 1);
        fprintf(fichero_RAM, " <=  %f   ", df->data[0][i]);
    }

    // 2.2
    for (r = N_Inputs;r < N_Inputs+N_Outputs;r++) {
        fprintf(fichero_RAM, "\n r%d: ", r + 1); // nombre de la restriccion
        fprintf(fichero_RAM, "- %f phi ", df->data[0][r]);
        for (j = 0;j < N_DMU;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", df->data[j][r], j + 1);
        fprintf(fichero_RAM, ">= 0 ");
    }

    // 2.3
    fprintf(fichero_RAM, "\n rl: ");
    for (j = 0;j < N_DMU;j++)
        fprintf(fichero_RAM, " +  l%d  ", j + 1);
    fprintf(fichero_RAM, " = 1  ");

    // 2.4
    fprintf(fichero_RAM, "\nBound ");
    for (j = 0;j < N_DMU;j++)
        fprintf(fichero_RAM, "\n 0 <= l%d <= 1 ", j + 1);


    fprintf(fichero_RAM, "\nEnd\n");

    fclose(fichero_RAM);

    //printf("\nFin escritura en disco del problema DEA");


    // RESOLVER PARA TODAS LAS DMUs

    CPXENVptr env;
    CPXLPptr  lp_RAM;
    double d_temp = 10000.;
    int status = 0;

    env = CPXopenCPLEX(&status); if (status)printf("\n ERROR abrir CPLEX");

    CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_OFF); //mostrar mensajes de control

    // crear el problema en CPLEX
    lp_RAM = CPXcreateprob(env, &status, "mod_lp"); if (status)printf("\nERROR %d crear problema", status);
    // leer el fichero en disco y usar como problema
    status = CPXreadcopyprob(env, lp_RAM, filename, NULL);if (status)printf("\nERROR %d leer  problema", status);



    //printf("\n\n solucion para todas las DMUs DEA");

    for (j = 0;j < N_DMU;j++)
    {
        // Modificamos problema par la DMU j
        for (i = 0;i < N_Inputs;i++) {
            status = CPXchgcoef(env, lp_RAM, i, -1, df->data[j][i]);
        }
        for (r = N_Inputs;r < N_Inputs+N_Outputs;r++) {
            //status = CPXchgcoef(env, lp_RAM, r + N_Inputs, -1, outputs[j][r]);
            status = CPXchgcoef(env, lp_RAM, r, 0, -df->data[j][r]);
        }

        // Resolvemos con CPLEX
        status = CPXprimopt(env, lp_RAM);if (status)printf("\n ERROR resolver problema DMU %d con código de error %d", j + 1, status);

        status = CPXgetobjval(env, lp_RAM, &d_temp);if (status)printf("\n ERROR obtener solucion DMU %d ", j + 1);
        // if (j == 31) CPXwriteprob(env, lp_RAM, "DEA_DF_1.lp", NULL);

        dea_scores[j] = d_temp;
        //printf("\n La solucion para la DMU %d es %f", j + 1, d_temp);

    }

}


///////////////////////////////
// FUNCIONES AUXILIARES EAT //
//////////////////////////////

// Genera el numero de hojas del arbol
int getNumLeaves(VTREE* tree) {
    VTreeIter it1;
    VTN* t1;
    int numLeaves = 0;

    treeIterSet(tree, &it1, LEVELS, START);
    while ((t1 = treeIterNextNode(&it1)))
    {
        if (t1->left == NULL) // es nodo hoja
        {
            numLeaves++;
        }
    }
    return numLeaves;
}

// Generar los a
double** getAtreeTk(VTREE* tree, int numLeaves)
{
    VTreeIter it1;
    VTN* t1;
    NEAT* n1;
    int nX = ((NEAT*)tree->first->data)->nX;

    // matriz para los a
    double** atreeTk = (double**)malloc(numLeaves * sizeof(double*));
    for (int j = 0; j < numLeaves; j++)  atreeTk[j] = (double*)malloc(nX * sizeof(double));

    treeIterSet(tree, &it1, LEVELS, START);
    int j = 0;
    while ((t1 = treeIterNextNode(&it1)))
    {
        if (t1->left == NULL) // es nodo hoja
        {
            n1 = (NEAT*)t1->data;

            for (int i = 0; i < nX; i++) {
                atreeTk[j][i] = n1->a[i];
            }
            j++;
        }
    }
    return atreeTk;
}

// Generar los d(a)
double** getYtreeTk(VTREE* tree, int numLeaves)
{
    VTreeIter it1;
    VTN* t1;
    NEAT* n1;
    int nY = ((NEAT*)tree->first->data)->nY;

    // matriz para los a
    double** ytreeTk = (double**)malloc(numLeaves * sizeof(double*));
    for (int j = 0; j < numLeaves; j++)  ytreeTk[j] = (double*)malloc(nY * sizeof(double));

    treeIterSet(tree, &it1, LEVELS, START);
    int j = 0;
    while ((t1 = treeIterNextNode(&it1)))
    {
        if (t1->left == NULL) // es nodo hoja
        {
            n1 = (NEAT*)t1->data;

            for (int i = 0; i < nY; i++) {
                ytreeTk[j][i] = n1->y[i];
            }
            j++;
        }
    }
    return ytreeTk;
}


///////////////////////
// SCORE EAT OUTPUT //
//////////////////////

// BCC OUPUT
void EAT_BBC_output(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, VTREE* tree, double* eat_scores) {

    // ESCRIBIR EL PROBLEMA EN DISCO

    int i, j, r;

    FILE* fichero_RAM;
    char* filename = "EAT_BBC_output.lp";
    fichero_RAM = fopen(filename, "wb+");
    fprintf(fichero_RAM, "Maximize\n");

    // Funcion objetivo
    fprintf(fichero_RAM, "obj:\t");
    fprintf(fichero_RAM, "phi");

    // Restricciones
    fprintf(fichero_RAM, "\nSubject To");

    // Get atreeTk and yTreeTk
    int numLeaves = getNumLeaves(tree);
    double** atreeTk = getAtreeTk(tree, numLeaves);
    /*printf("\nList a \n");
    for (int i = 0; i < numLeaves; i++) {
        for (int j = 0; j < N_Inputs; j++) {
            printf(" %f ", atreeTk[i][j]);
        }
        printf("\n");
    }*/
    double** ytreeTk = getYtreeTk(tree, numLeaves);
    /*printf("List y \n");
    for (int i = 0; i < numLeaves; i++) {
        for (int j = 0; j < N_Outputs; j++) {
            printf(" %f ", ytreeTk[i][j]);
        }
        printf("\n");
    }*/


    // 2.1
    for (i = 0;i < N_Inputs;i++) {
        fprintf(fichero_RAM, "\n i%d: ", i + 1); // nombre de la restriccion
        for (j = 0;j < numLeaves;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", atreeTk[j][i], j + 1);
        fprintf(fichero_RAM, " <=  %f   ", df->data[0][i]);
    }

    // 2.2
    for (r = 0;r < N_Outputs;r++) {
        fprintf(fichero_RAM, "\n r%d: ", r + 1); // nombre de la restriccion
        fprintf(fichero_RAM, "- %f phi ", df->data[0][r+N_Inputs]);
        for (j = 0;j < numLeaves;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", ytreeTk[j][r], j + 1);
        fprintf(fichero_RAM, ">= 0 ");
    }

    // 2.3
    fprintf(fichero_RAM, "\n rl: ");
    for (j = 0;j < numLeaves;j++)
        fprintf(fichero_RAM, " +  l%d  ", j + 1);
    fprintf(fichero_RAM, " = 1  ");

    // 2.4
    fprintf(fichero_RAM, "\nBinaries ");
    for (j = 0;j < numLeaves;j++)
        fprintf(fichero_RAM, "\n l%d ", j + 1);


    fprintf(fichero_RAM, "\nEnd\n");

    fclose(fichero_RAM);

    //printf("\nFin escritura en disco del problema FDH");


    // RESOLVER PARA TODAS LAS DMUs

    CPXENVptr env;
    CPXLPptr  lp_RAM;
    double d_temp = 10000.;
    int status = 0;

    env = CPXopenCPLEX(&status); if (status)printf("\n ERROR abrir CPLEX");

    CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_OFF); //mostrar mensajes de control

    // CPXsetdblparam(env, CPX_PARAM_TILIM, 7200.0);  // tiempo maximo que puede estar intentando resolver el problema

    // crear el problema en CPLEX
    lp_RAM = CPXcreateprob(env, &status, "mod_lp"); if (status)printf("\n ERROR %d crear problema", status);
    // leer el fichero en disco y usar como problema
    status = CPXreadcopyprob(env, lp_RAM, filename, NULL);if (status)printf("\n ERROR %d leer  problema", status);

    //printf("\n\n solucion para todas las DMUs FDH");

    for (j = 0;j < N_DMU;j++)
    {
        // Modificamos problema par la DMU j
        for (i = 0;i < N_Inputs;i++) {
            status = CPXchgcoef(env, lp_RAM, i, -1, df->data[j][i]); // -1 =  lado derecho de la ec.
        }
        for (r = N_Inputs;r < N_Inputs+N_Outputs;r++) {
            //status = CPXchgcoef(env, lp_RAM, r + N_Inputs, -1, outputs[j][r]);
            status = CPXchgcoef(env, lp_RAM, r, 0, -df->data[j][r]);
        }

        // Resolvemos con CPLEX
        status = CPXmipopt(env, lp_RAM);if (status)printf("\n ERROR resolver problema DMU %d con código de error %d", j + 1, status);

        status = CPXgetobjval(env, lp_RAM, &d_temp);if (status)printf("\n ERROR obtener solucion DMU %d ", j + 1);
        // if (j == 31) CPXwriteprob(env, lp_RAM, "EAT_BBC_output_1.lp", NULL);

        eat_scores[j] = d_temp;
        //printf("\n La solucion para la DMU %d es %f", j + 1, d_temp);

    }

}


// BCC INPUT
void EAT_BBC_input(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, VTREE* tree, double* eat_scores) {

    // ESCRIBIR EL PROBLEMA EN DISCO PARA DMU 0
    int i, j, r;

    FILE* fichero_RAM;
    char* filename = "EAT_BBC_input.lp";
    fichero_RAM = fopen(filename, "wb+");
    fprintf(fichero_RAM, "Minimize\n");

    // Funcion objetivo
    fprintf(fichero_RAM, "obj:\t");
    fprintf(fichero_RAM, "phi");

    // Restricciones
    fprintf(fichero_RAM, "\nSubject To");

    // Get atreeTk and yTreeTk
    int numLeaves = getNumLeaves(tree);
    double** atreeTk = getAtreeTk(tree, numLeaves);
    /*printf("\nList a \n");
    for (int i = 0; i < numLeaves; i++) {
        for (int j = 0; j < N_Inputs; j++) {
            printf(" %f ", atreeTk[i][j]);
        }
        printf("\n");
    }*/
    double** ytreeTk = getYtreeTk(tree, numLeaves);
    /*printf("List y \n");
    for (int i = 0; i < numLeaves; i++) {
        for (int j = 0; j < N_Outputs; j++) {
            printf(" %f ", ytreeTk[i][j]);
        }
        printf("\n");
    }*/


    // 2.1
    for (i = 0;i < N_Inputs;i++) {
        fprintf(fichero_RAM, "\n i%d: ", i + 1); // nombre de la restriccion
        fprintf(fichero_RAM, " -  %f phi", df->data[0][i]);
        for (j = 0;j < numLeaves;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", atreeTk[j][i], j + 1);
        fprintf(fichero_RAM, " <=  0   ");
    }

    // 2.2
    for (r = 0;r < N_Outputs;r++) {
        fprintf(fichero_RAM, "\n r%d: ", r + 1); // nombre de la restriccion
        for (j = 0;j < numLeaves;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", ytreeTk[j][r], j + 1);
        fprintf(fichero_RAM, ">= %f ", df->data[0][r + N_Inputs]);
    }

    // 2.3
    fprintf(fichero_RAM, "\n rl: ");
    for (j = 0;j < numLeaves;j++)
        fprintf(fichero_RAM, " +  l%d  ", j + 1);
    fprintf(fichero_RAM, " = 1  ");

    // 2.4
    fprintf(fichero_RAM, "\nBinaries ");
    for (j = 0;j < numLeaves;j++)
        fprintf(fichero_RAM, "\n l%d ", j + 1);


    fprintf(fichero_RAM, "\nEnd\n");

    fclose(fichero_RAM);


    // RESOLVER PARA TODAS LAS DMUs

    CPXENVptr env;
    CPXLPptr  lp_RAM;
    double d_temp = 10000.;
    int status = 0;

    env = CPXopenCPLEX(&status); if (status)printf("\n ERROR abrir CPLEX");

    CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_OFF); //mostrar mensajes de control

    // CPXsetdblparam(env, CPX_PARAM_TILIM, 7200.0);  // tiempo maximo que puede estar intentando resolver el problema

    // crear el problema en CPLEX
    lp_RAM = CPXcreateprob(env, &status, "mod_lp"); if (status)printf("\n ERROR %d crear problema", status);
    // leer el fichero en disco y usar como problema
    status = CPXreadcopyprob(env, lp_RAM, filename, NULL);if (status)printf("\n ERROR %d leer  problema", status);

    //printf("\n\n solucion para todas las DMUs FDH");

    for (j = 0;j < N_DMU;j++)
    {
        // Modificamos problema par la DMU j
        for (i = 0;i < N_Inputs;i++) {
            status = CPXchgcoef(env, lp_RAM, i, 0, -df->data[j][i]); // -1 =  lado derecho de la ec.
        }
        for (r = N_Inputs;r < N_Inputs + N_Outputs;r++) {
            //status = CPXchgcoef(env, lp_RAM, r + N_Inputs, -1, outputs[j][r]);
            status = CPXchgcoef(env, lp_RAM, r, -1, df->data[j][r]);
        }

        // Resolvemos con CPLEX
        status = CPXmipopt(env, lp_RAM);if (status)printf("\n ERROR resolver problema DMU %d con código de error %d", j + 1, status);

        status = CPXgetobjval(env, lp_RAM, &d_temp);if (status)printf("\n ERROR obtener solucion DMU %d con código de error %d", j + 1, status);
        // if (j == 31) CPXwriteprob(env, lp_RAM, "EAT_BBC_input_1.lp", NULL);

        eat_scores[j] = d_temp;
        //printf("\n La solucion para la DMU %d es %f", j + 1, d_temp);

    }

}

// Continuous BCC OUPUT
void CEAT_BBC_output(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, VTREE* tree, double* eat_scores) {

    // ESCRIBIR EL PROBLEMA EN DISCO

    int i, j, r;

    FILE* fichero_RAM;
    char* filename = "CEAT_BBC_output.lp";
    fichero_RAM = fopen(filename, "wb+");
    fprintf(fichero_RAM, "Maximize\n");

    // Funcion objetivo
    fprintf(fichero_RAM, "obj:\t");
    fprintf(fichero_RAM, "phi");

    // Restricciones
    fprintf(fichero_RAM, "\nSubject To");

    // Get atreeTk and yTreeTk
    int numLeaves = getNumLeaves(tree);
    double** atreeTk = getAtreeTk(tree, numLeaves);
    printf("\nList a \n");
    for (int i = 0; i < numLeaves; i++) {
        for (int j = 0; j < N_Inputs; j++) {
            printf(" %f ", atreeTk[i][j]);
        }
        printf("\n");
    }
    double** ytreeTk = getYtreeTk(tree, numLeaves);
    printf("List y \n");
    for (int i = 0; i < numLeaves; i++) {
        for (int j = 0; j < N_Outputs; j++) {
            printf(" %f ", ytreeTk[i][j]);
        }
        printf("\n");
    }


    // 2.1
    for (i = 0;i < N_Inputs;i++) {
        fprintf(fichero_RAM, "\n i%d: ", i + 1); // nombre de la restriccion
        for (j = 0;j < numLeaves;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", atreeTk[j][i], j + 1);
        fprintf(fichero_RAM, " <=  %f   ", df->data[0][i]);
    }

    // 2.2
    for (r = 0;r < N_Outputs;r++) {
        fprintf(fichero_RAM, "\n r%d: ", r + 1); // nombre de la restriccion
        fprintf(fichero_RAM, "- %f phi ", df->data[0][r + N_Inputs]);
        for (j = 0;j < numLeaves;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", ytreeTk[j][r], j + 1);
        fprintf(fichero_RAM, ">= 0 ");
    }

    // 2.3
    fprintf(fichero_RAM, "\n rl: ");
    for (j = 0;j < numLeaves;j++)
        fprintf(fichero_RAM, " +  l%d  ", j + 1);
    fprintf(fichero_RAM, " = 1  ");

    // 2.4
    fprintf(fichero_RAM, "\nBound ");
    for (j = 0;j < numLeaves;j++)
        fprintf(fichero_RAM, "\n 0 <= l%d <= 1 ", j + 1);


    fprintf(fichero_RAM, "\nEnd\n");

    fclose(fichero_RAM);

    //printf("\nFin escritura en disco del problema FDH");


    // RESOLVER PARA TODAS LAS DMUs

    CPXENVptr env;
    CPXLPptr  lp_RAM;
    double d_temp = 10000.;
    int status = 0;

    env = CPXopenCPLEX(&status); if (status)printf("\n ERROR abrir CPLEX");

    CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_OFF); //mostrar mensajes de control

    // CPXsetdblparam(env, CPX_PARAM_TILIM, 7200.0);  // tiempo maximo que puede estar intentando resolver el problema

    // crear el problema en CPLEX
    lp_RAM = CPXcreateprob(env, &status, "mod_lp"); if (status)printf("\n ERROR %d crear problema", status);
    // leer el fichero en disco y usar como problema
    status = CPXreadcopyprob(env, lp_RAM, filename, NULL);if (status)printf("\nERROR %d leer  problema", status);

    //printf("\n\n solucion para todas las DMUs FDH");

    for (j = 0;j < N_DMU;j++)
    {
        // Modificamos problema par la DMU j
        for (i = 0;i < N_Inputs;i++) {
            status = CPXchgcoef(env, lp_RAM, i, -1, df->data[j][i]); // -1 =  lado derecho de la ec.
        }
        for (r = N_Inputs;r < N_Inputs + N_Outputs;r++) {
            //status = CPXchgcoef(env, lp_RAM, r + N_Inputs, -1, outputs[j][r]);
            status = CPXchgcoef(env, lp_RAM, r, 0, -df->data[j][r]);
        }

        // Resolvemos con CPLEX
        status = CPXprimopt(env, lp_RAM);if (status)printf("\n ERROR resolver problema DMU %d con código de error %d", j + 1, status);

        status = CPXgetobjval(env, lp_RAM, &d_temp);if (status)printf("\n ERROR obtener solucion DMU %d ", j + 1);
        //if (j == 31) CPXwriteprob(env, lp_RAM, "CEAT_BBC_output_1.lp", NULL);

        eat_scores[j] = d_temp;
        //printf("\n La solucion para la DMU %d es %f", j + 1, d_temp);

    }

}


// DDF: Directional Distance Function
void EAT_DDF(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, VTREE* tree, double* eat_scores) {

    // ESCRIBIR EL PROBLEMA EN DISCO

    int i, j, r;

    FILE* fichero_RAM;
    char* filename = "EAT_DDF.lp";
    fichero_RAM = fopen(filename, "wb+");
    fprintf(fichero_RAM, "Maximize\n");

    // Funcion objetivo
    fprintf(fichero_RAM, "obj:\t");
    fprintf(fichero_RAM, "beta");

    // Restricciones
    fprintf(fichero_RAM, "\nSubject To");

    // Get atreeTk and yTreeTk
    int numLeaves = getNumLeaves(tree);
    double** atreeTk = getAtreeTk(tree, numLeaves);
    double** ytreeTk = getYtreeTk(tree, numLeaves);

    // 2.1
    for (i = 0;i < N_Inputs;i++) {
        fprintf(fichero_RAM, "\n i%d: ", i + 1); // nombre de la restriccion
        fprintf(fichero_RAM, "+ %f beta ", df->data[0][i]);
        for (j = 0;j < numLeaves;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", atreeTk[j][i], j + 1);
        fprintf(fichero_RAM, " <=  %f   ", df->data[0][i]);
    }

    // 2.2
    for (r = 0;r < N_Outputs;r++) {
        fprintf(fichero_RAM, "\n r%d: ", r + 1); // nombre de la restriccion
        fprintf(fichero_RAM, "- %f beta ", df->data[0][r + N_Inputs]);
        for (j = 0;j < numLeaves;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", ytreeTk[j][r], j + 1);
        fprintf(fichero_RAM, ">=  %f ", df->data[0][r + N_Inputs]);
    }

    // 2.3
    fprintf(fichero_RAM, "\n rl: ");
    for (j = 0;j < numLeaves;j++)
        fprintf(fichero_RAM, " +  l%d  ", j + 1);
    fprintf(fichero_RAM, " = 1  ");

    // 2.4
    fprintf(fichero_RAM, "\nBinaries ");
    for (j = 0;j < numLeaves;j++)
        fprintf(fichero_RAM, "\n l%d ", j + 1);


    fprintf(fichero_RAM, "\nEnd\n");

    fclose(fichero_RAM);

    //printf("\nFin escritura en disco del problema FDH");


    // RESOLVER PARA TODAS LAS DMUs

    CPXENVptr env;
    CPXLPptr  lp_RAM;
    double d_temp = 10000.;
    int status = 0;

    env = CPXopenCPLEX(&status); if (status)printf("\n ERROR abrir CPLEX");

    CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_OFF); //mostrar mensajes de control

    // CPXsetdblparam(env, CPX_PARAM_TILIM, 7200.0);  // tiempo maximo que puede estar intentando resolver el problema

    // crear el problema en CPLEX
    lp_RAM = CPXcreateprob(env, &status, "mod_lp"); if (status)printf("\nERROR %d crear problema", status);
    // leer el fichero en disco y usar como problema
    status = CPXreadcopyprob(env, lp_RAM, filename, NULL);if (status)printf("\nERROR %d leer  problema", status);

    //printf("\n\n solucion para todas las DMUs FDH");

    for (j = 0;j < N_DMU;j++)
    {
        // Modificamos problema par la DMU j
        for (i = 0;i < N_Inputs;i++) {
            status = CPXchgcoef(env, lp_RAM, i, -1, df->data[j][i]); // -1 =  lado derecho de la ec.
            status = CPXchgcoef(env, lp_RAM, i, 0, df->data[j][i]); 
        }
        for (r = N_Inputs;r < N_Inputs + N_Outputs;r++) {
            //status = CPXchgcoef(env, lp_RAM, r + N_Inputs, -1, outputs[j][r]);
            status = CPXchgcoef(env, lp_RAM, r, -1, df->data[j][r]);
            status = CPXchgcoef(env, lp_RAM, r, 0, -df->data[j][r]);
        }

        // Resolvemos con CPLEX
        status = CPXmipopt(env, lp_RAM);if (status)printf("\n ERROR resolver problema DMU %d con código de error %d", j + 1, status);

        status = CPXgetobjval(env, lp_RAM, &d_temp);if (status)printf("\n ERROR obtener solucion DMU %d ", j + 1);
        //if (j == 31) CPXwriteprob(env, lp_RAM, "EAT_DDF_1.lp", NULL);

        eat_scores[j] = d_temp;
        //printf("\n La solucion para la DMU %d es %f", j + 1, d_temp);

    }

}


// RUSELL INPUT
void EAT_Russell_input(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, VTREE* tree, double* eat_scores) {

    // ESCRIBIR EL PROBLEMA EN DISCO PARA DMU 0
    int i, j, r;

    FILE* fichero_RAM;
    char* filename = "EAT_Russell_input.lp";
    fichero_RAM = fopen(filename, "wb+");
    fprintf(fichero_RAM, "Minimize\n");

    // Funcion objetivo
    fprintf(fichero_RAM, "obj:\t");
    for (j = 0;j < N_Inputs;j++)
        fprintf(fichero_RAM, " +  %f o%d ", 1./N_Inputs, j + 1);

    // Restricciones
    fprintf(fichero_RAM, "\nSubject To");

    // Get atreeTk and yTreeTk
    int numLeaves = getNumLeaves(tree);
    double** atreeTk = getAtreeTk(tree, numLeaves);
    double** ytreeTk = getYtreeTk(tree, numLeaves);

    // 2.1
    for (i = 0;i < N_Inputs;i++) {
        fprintf(fichero_RAM, "\n i%d: ", i + 1); // nombre de la restriccion
        fprintf(fichero_RAM, " -  %f o%d", df->data[0][i], i+1);
        for (j = 0;j < numLeaves;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", atreeTk[j][i], j + 1);
        fprintf(fichero_RAM, " <=  0   ");
    }

    // 2.2
    for (r = 0;r < N_Outputs;r++) {
        fprintf(fichero_RAM, "\n r%d: ", r + 1); // nombre de la restriccion
        for (j = 0;j < numLeaves;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", ytreeTk[j][r], j + 1);
        fprintf(fichero_RAM, ">= %f ", df->data[0][r + N_Inputs]);
    }

    // 2.3
    fprintf(fichero_RAM, "\n rl: ");
    for (j = 0;j < numLeaves;j++)
        fprintf(fichero_RAM, " +  l%d  ", j + 1);
    fprintf(fichero_RAM, " = 1  ");

    // 2.5
    fprintf(fichero_RAM, "\nBound ");
    for (j = 0;j < N_Inputs;j++)
        fprintf(fichero_RAM, "\n 0 <= o%d <= 1 ", j + 1);

    // 2.4
    fprintf(fichero_RAM, "\nBinaries ");
    for (j = 0;j < numLeaves;j++)
        fprintf(fichero_RAM, "\n l%d ", j + 1);



    fprintf(fichero_RAM, "\nEnd\n");

    fclose(fichero_RAM);


    // RESOLVER PARA TODAS LAS DMUs

    CPXENVptr env;
    CPXLPptr  lp_RAM;
    double d_temp = 10000.;
    int status = 0;

    env = CPXopenCPLEX(&status); if (status)printf("\n ERROR abrir CPLEX");

    CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_OFF); //mostrar mensajes de control

    // CPXsetdblparam(env, CPX_PARAM_TILIM, 7200.0);  // tiempo maximo que puede estar intentando resolver el problema

    // crear el problema en CPLEX
    lp_RAM = CPXcreateprob(env, &status, "mod_lp"); if (status)printf("\nERROR %d crear problema", status);
    // leer el fichero en disco y usar como problema
    status = CPXreadcopyprob(env, lp_RAM, filename, NULL);if (status)printf("\nERROR %d leer  problema", status);

    //printf("\n\n solucion para todas las DMUs FDH");

    for (j = 0;j < N_DMU;j++)
    {
        // Modificamos problema par la DMU j
        for (i = 0;i < N_Inputs;i++) {
            status = CPXchgcoef(env, lp_RAM, i, i, -df->data[j][i]); 
        }
        for (r = N_Inputs;r < N_Inputs + N_Outputs;r++) {
            status = CPXchgcoef(env, lp_RAM, r, -1, df->data[j][r]);
        }


        // Resolvemos con CPLEX
        status = CPXmipopt(env, lp_RAM);if (status)printf("\n ERROR resolver problema DMU %d con código de error %d", j + 1, status);

        status = CPXgetobjval(env, lp_RAM, &d_temp);if (status)printf("\n ERROR obtener solucion DMU %d con código de error %d", j + 1, status);
        if (j == 0) CPXwriteprob(env, lp_RAM, "EAT_Russell_input_1.lp", NULL);

        eat_scores[j] = d_temp;
        //printf("\n La solucion para la DMU %d es %f", j + 1, d_temp);

    }

}

// RUSELL OUTPUT
void EAT_Russell_ouput(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, VTREE* tree, double* eat_scores) {

    // ESCRIBIR EL PROBLEMA EN DISCO PARA DMU 0
    int i, j, r;

    FILE* fichero_RAM;
    char* filename = "EAT_Russell_output.lp";
    fichero_RAM = fopen(filename, "wb+");
    fprintf(fichero_RAM, "Maximize\n");

    // Funcion objetivo
    fprintf(fichero_RAM, "obj:\t");
    for (j = 0;j < N_Outputs;j++)
        fprintf(fichero_RAM, " +  %f o%d ", 1. / N_Outputs, j + 1);

    // Restricciones
    fprintf(fichero_RAM, "\nSubject To");

    // Get atreeTk and yTreeTk
    int numLeaves = getNumLeaves(tree);
    double** atreeTk = getAtreeTk(tree, numLeaves);
    double** ytreeTk = getYtreeTk(tree, numLeaves);

    // 2.1
    for (i = 0;i < N_Inputs;i++) {
        fprintf(fichero_RAM, "\n i%d: ", i + 1); // nombre de la restriccion
        for (j = 0;j < numLeaves;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", atreeTk[j][i], j + 1);
        fprintf(fichero_RAM, " <= %f ", df->data[0][i]);
    }

    // 2.2
    for (r = 0;r < N_Outputs;r++) {
        fprintf(fichero_RAM, "\n r%d: ", r + 1); // nombre de la restriccion
        fprintf(fichero_RAM, " -  %f o%d", df->data[0][r + N_Inputs], r + 1);
        for (j = 0;j < numLeaves;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", ytreeTk[j][r], j + 1);
        fprintf(fichero_RAM, ">= 0 ");
    }

    // 2.3
    fprintf(fichero_RAM, "\n rl: ");
    for (j = 0;j < numLeaves;j++)
        fprintf(fichero_RAM, " +  l%d  ", j + 1);
    fprintf(fichero_RAM, " = 1  ");

    // 2.5
    fprintf(fichero_RAM, "\nBound ");
    for (j = 0;j < N_Outputs;j++)
        fprintf(fichero_RAM, "\n o%d >= 1 ", j + 1);

    // 2.4
    fprintf(fichero_RAM, "\nBinaries ");
    for (j = 0;j < numLeaves;j++)
        fprintf(fichero_RAM, "\n l%d ", j + 1);



    fprintf(fichero_RAM, "\nEnd\n");

    fclose(fichero_RAM);


    // RESOLVER PARA TODAS LAS DMUs

    CPXENVptr env;
    CPXLPptr  lp_RAM;
    double d_temp = 10000.;
    int status = 0;

    env = CPXopenCPLEX(&status); if (status)printf("\n ERROR abrir CPLEX");

    CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_OFF); //mostrar mensajes de control

    // CPXsetdblparam(env, CPX_PARAM_TILIM, 7200.0);  // tiempo maximo que puede estar intentando resolver el problema

    // crear el problema en CPLEX
    lp_RAM = CPXcreateprob(env, &status, "mod_lp"); if (status)printf("\nERROR %d crear problema", status);
    // leer el fichero en disco y usar como problema
    status = CPXreadcopyprob(env, lp_RAM, filename, NULL);if (status)printf("\nERROR %d leer  problema", status);

    //printf("\n\n solucion para todas las DMUs FDH");

    for (j = 0;j < N_DMU;j++)
    {
        // Modificamos problema par la DMU j
        for (i = 0;i < N_Inputs;i++) {
            status = CPXchgcoef(env, lp_RAM, i, -1, df->data[j][i]); // -1 =  lado derecho de la ec.
        }
        for (r = N_Inputs;r < N_Inputs + N_Outputs;r++) {
            status = CPXchgcoef(env, lp_RAM, r, r- N_Inputs, -df->data[j][r]);
        }


        // Resolvemos con CPLEX
        status = CPXmipopt(env, lp_RAM);if (status)printf("\n ERROR resolver problema DMU %d con código de error %d", j + 1, status);

        status = CPXgetobjval(env, lp_RAM, &d_temp);if (status)printf("\n ERROR obtener solucion DMU %d con código de error %d", j + 1, status);
        if (j == 0) CPXwriteprob(env, lp_RAM, "EAT_Russell_output_1.lp", NULL);

        eat_scores[j] = d_temp;
        //printf("\n La solucion para la DMU %d es %f", j + 1, d_temp);

    }

}

// WEIGHTED ADDITIVE MODEL
void EAT_WAM(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, VTREE* tree, double* eat_scores) {

    // ESCRIBIR EL PROBLEMA EN DISCO PARA DMU 0
    int i, j, r;

    FILE* fichero_RAM;
    char* filename = "EAT_WAM.lp";
    fichero_RAM = fopen(filename, "wb+");
    fprintf(fichero_RAM, "Maximize\n");

    // Funcion objetivo
    fprintf(fichero_RAM, "obj:\t");
    
    for (j = 0;j < N_Inputs;j++)
        fprintf(fichero_RAM, " +  %f s%d ", 1. / N_Inputs, j + 1); // mismo peso a todas las variables de entrada
    for (j = 0;j < N_Outputs;j++)
        fprintf(fichero_RAM, " +  %f s%d ", 1. / N_Outputs, j + N_Inputs + 1); // mismo peso a todas las variables de salida

    // Restricciones
    fprintf(fichero_RAM, "\nSubject To");

    // Get atreeTk and yTreeTk
    int numLeaves = getNumLeaves(tree);
    double** atreeTk = getAtreeTk(tree, numLeaves);
    double** ytreeTk = getYtreeTk(tree, numLeaves);

    // 2.1
    for (i = 0;i < N_Inputs;i++) {
        fprintf(fichero_RAM, "\n i%d: ", i + 1); // nombre de la restriccion
        fprintf(fichero_RAM, " + s%d", i+1);
        for (j = 0;j < numLeaves;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", atreeTk[j][i], j + 1);
        fprintf(fichero_RAM, " <= %f ", df->data[0][i]);
    }

    // 2.2
    for (r = 0;r < N_Outputs;r++) {
        fprintf(fichero_RAM, "\n r%d: ", r + 1); // nombre de la restriccion
        fprintf(fichero_RAM, " - s%d", r + N_Inputs + 1);
        for (j = 0;j < numLeaves;j++)
            fprintf(fichero_RAM, " +  %f l%d  ", ytreeTk[j][r], j + 1);
        fprintf(fichero_RAM, ">= %f ", df->data[0][r+N_Inputs]);
    }

    // 2.3
    fprintf(fichero_RAM, "\n rl: ");
    for (j = 0;j < numLeaves;j++)
        fprintf(fichero_RAM, " +  l%d  ", j + 1);
    fprintf(fichero_RAM, " = 1  ");

    // 2.5
    fprintf(fichero_RAM, "\nBound ");
    for (j = 0;j < N_Inputs;j++)
        fprintf(fichero_RAM, "\n s%d >= 0 ", j + 1);
    for (j = N_Inputs;j < N_Inputs + N_Outputs;j++)
        fprintf(fichero_RAM, "\n s%d >= 0 ", j + 1);

    // 2.4
    fprintf(fichero_RAM, "\nBinaries ");
    for (j = 0;j < numLeaves;j++)
        fprintf(fichero_RAM, "\n l%d ", j + 1);



    fprintf(fichero_RAM, "\nEnd\n");

    fclose(fichero_RAM);


    // RESOLVER PARA TODAS LAS DMUs

    CPXENVptr env;
    CPXLPptr  lp_RAM;
    double d_temp = 10000.;
    int status = 0;

    env = CPXopenCPLEX(&status); if (status)printf("\n ERROR abrir CPLEX");

    CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_OFF); //mostrar mensajes de control

    // CPXsetdblparam(env, CPX_PARAM_TILIM, 7200.0);  // tiempo maximo que puede estar intentando resolver el problema

    // crear el problema en CPLEX
    lp_RAM = CPXcreateprob(env, &status, "mod_lp"); if (status)printf("\nERROR %d crear problema", status);
    // leer el fichero en disco y usar como problema
    status = CPXreadcopyprob(env, lp_RAM, filename, NULL);if (status)printf("\nERROR %d leer  problema", status);

    //printf("\n\n solucion para todas las DMUs FDH");

    for (j = 0;j < N_DMU;j++)
    {
        // Modificamos problema par la DMU j
        for (i = 0;i < N_Inputs;i++) {
            status = CPXchgcoef(env, lp_RAM, i, -1, df->data[j][i]); // -1 =  lado derecho de la ec.
        }
        for (r = N_Inputs;r < N_Inputs + N_Outputs;r++) {
            status = CPXchgcoef(env, lp_RAM, r, -1, df->data[j][r]);
        }


        // Resolvemos con CPLEX
        status = CPXmipopt(env, lp_RAM);if (status)printf("\n ERROR resolver problema DMU %d con código de error %d", j + 1, status);

        status = CPXgetobjval(env, lp_RAM, &d_temp);if (status)printf("\n ERROR obtener solucion DMU %d con código de error %d", j + 1, status);
        if (j == 0) CPXwriteprob(env, lp_RAM, "EAT_WAM_1.lp", NULL);

        eat_scores[j] = d_temp;
        //printf("\n La solucion para la DMU %d es %f", j + 1, d_temp);

    }

}