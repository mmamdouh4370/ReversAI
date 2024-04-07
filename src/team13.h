#ifndef TEAM_13_H_INCLUDED
#define TEAM_13_H_INCLUDED

#include "reversi_functions.h"

position * team13Move(const enum piece board[][SIZE], enum piece mine, int secondsleft);
int team13MinMaxAB(const enum piece board[][SIZE], enum piece mine, enum piece opp, int depth, int alpha, int beta, int isMax);
int team13Eval(const enum piece board[][SIZE], enum piece mine);
void team13AdjustCornerWeights(const enum piece board[][SIZE], enum piece mine);
#endif // TEAM_13_H_INCLUDED
