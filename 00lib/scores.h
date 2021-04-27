#include "eat.h"
#include "00lib/data.h"

#ifndef SCORES
#define SCORES

// Score FDH (save result in fdh_scores)
void FDH_BBC_ouput(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, double* fdh_scores);

// Score DEA output (save result in dea_scores)
void DEA_BBC_output(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, double* dea_scores);

// Scores EAT (save result in eat_scores)
void EAT_BBC_output(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, VTREE* tree, double* eat_scores);
void EAT_BBC_input(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, VTREE* tree, double* eat_scores);
void CEAT_BBC_output(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, VTREE* tree, double* eat_scores);
void EAT_DDF(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, VTREE* tree, double* eat_scores);
void EAT_Russell_input(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, VTREE* tree, double* eat_scores);
void EAT_Russell_ouput(int N_DMU, int N_Inputs, int N_Outputs, DFrameNum* df, VTREE* tree, double* eat_scores);

#endif 