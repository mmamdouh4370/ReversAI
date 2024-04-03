#ifndef TEAM_07_H_INCLUDED
#define TEAM_07_H_INCLUDED

#include "reversi_functions.h"

position* team07Move(const enum piece board[][SIZE], enum piece mine, int secondsleft);
int miniMaxMobility(const enum piece board[][SIZE], position *choices, int numMoves, enum piece mine);
int miniMaxPoints(const enum piece board[][SIZE], position *choices, int numMoves, enum piece mine);
enum boolean playIsCorner(position* choices, int i);
enum boolean edgeTaken(const enum piece board[][SIZE]);

#endif // TEAM_07_H_INCLUDED