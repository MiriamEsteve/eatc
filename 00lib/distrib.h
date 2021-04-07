/*
Librería DISTRIBUCIONES DE PROBABILIDAD
=======================================
*/

#ifndef _DISTRIB___
#define _DISTRIB___

int* GetRandomIndex(int min, int max, int num, int rep); // 'rep' booleano: con o sin repetición
void SetRandomIndex(int min, int max, int num, int rep, int *index);

void RandUniform(double min, double max, double *arr, int n);
void RandNormal(double med, double desv, double *arr, int n);
void RandExpUniform(double min, double max, double *arr, int n);
void RandExpNormal(double med, double desv, double *arr, int n);


#endif
