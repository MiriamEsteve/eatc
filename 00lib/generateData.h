#include "00lib/data.h"

#ifndef GENERATE_DATA
#define GENERATE_DATA

// Rellena el DF para el caso mono output
int generate_data_mono(int seed, int N_DMU, int nX, int nY, DFrameNum* df);

// Rellena el DF para el caso multi output
// frontier: tanto por uno de DMUs ineficientes (contrario al tanto por uno de DMUs en la frontera)
// noise: ruido aleatorio o no (bool)
int generate_data_multi(int seed, int N_DMU, int nX, int nY, DFrameNum* df, float frontier, int ruido);

#endif 

