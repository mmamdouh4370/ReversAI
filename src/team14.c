#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include "team14.h"
#include "reversi_functions.h"

int minMaxAB(const enum piece board[][SIZE], enum piece mine, int depth, position * curMove, int alpha, int beta);

// Minmax function with alpha beta pruning for time saving
// Give better score/weight to better moves (corner control), mobility ?
// Negamax algo ?!, better board store as int 64/8?, bitboards wtf  a
position* team14Move(const enum piece board[][SIZE], enum piece mine, int secondsleft){
    int numMoves;
    position * allMoves = getPossibleMoves(board, mine, &numMoves);

    int bestScoreInd = -1, bestScore = -1, curScore;

    for (int i = 0; i < numMoves; i++){

        if (!isValidMove(board, &allMoves[i], mine)) continue;

        enum piece myBoard[SIZE][SIZE];
        copy(myBoard, board);

        //Run moves calc score here

        executeMove(myBoard, &allMoves[i], mine);
        curScore = minMaxAB(myBoard, mine, 3, NULL, INT_MIN, INT_MAX);


        if (curScore > bestScore){
            bestScoreInd = i;
            bestScore = curScore;
        }
    }
    
    position* res = malloc(sizeof(position));
    res->x = allMoves[bestScoreInd].x;
    res->y = allMoves[bestScoreInd].y;

    free(allMoves);

    return res;
}

int minMaxAB(const enum piece board[][SIZE], enum piece mine, int depth, position * curMove, int alpha, int beta){
    if (depth == 0){
        return score(board, mine) - score(board, opposite(mine));
    }

    int numMoves;
    position * possibleMoves = getPossibleMoves(board, mine, &numMoves);

    if (numMoves == 0){
        free(possibleMoves);
        return score(board, mine) - score(board, opposite(mine));
    }

    if (mine == BLACK){

        int best = INT_MIN;

        for (int i = 0; i < numMoves; i++){
            if (!isValidMove(board, &possibleMoves[i], mine)) continue;

            enum piece copy[SIZE][SIZE];
            copyBoard(copy, board);

            executeMove(copy, &possibleMoves[i], mine);
            int score = minMax(copy, opposite(mine), depth - 1, alpha, beta, NULL);

            if (score > best) {
                best = score;
                if (curMove != NULL) {
                    curMove->x = possibleMoves[i].x;
                    curMove->y = possibleMoves[i].y;
                }
            }

            if (score > alpha){
                alpha = score;
            }

            if (alpha >= beta) break;

        }
        
        free(possibleMoves);
        return best;

    } else {
        int best = INT_MAX;

        for (int i = 0; i < numMoves; i++){
            if (!isValidMove(board, &possibleMoves[i], mine)) continue;

            enum piece copy[SIZE][SIZE];
            copyBoard(copy, board);

            executeMove(copy, &possibleMoves[i], mine);
            int score = minMax(copy, opposite(mine), depth - 1, alpha, beta, NULL);

            if (score < best) {
                best = score;
                if (curMove != NULL) {
                    curMove->x = possibleMoves[i].x;
                    curMove->y = possibleMoves[i].y;
                }
            }

            if (score < beta){
                beta = score;
            }

            if (score >= beta) break;

        }
        
        free(possibleMoves);
        return best;
    } 

}