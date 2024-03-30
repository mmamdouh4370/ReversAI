#include <stdlib.h>
#include <time.h>

#include <limits.h>
#include <stdio.h>

#include "team13.h"
#include "reversi_functions.h"



int boardWeights[SIZE][SIZE] = {
    {120, -20,  20,   5,   5,  20, -20, 120},   
    {-20, -40,  -5,  -5,  -5,  -5, -40, -20},   
    {20,  -5,  15,   3,   3,  15,  -5,  20},
    {5,  -5,   3,   3,   3,   3,  -5,   5},
    {5,  -5,   3,   3,   3,   3,  -5,   5},
    {20,  -5,  15,   3,   3,  15,  -5,  20},
    {20, -40,  -5,  -5,  -5,  -5, -40, -20},  
    {120, -20,  20,   5,   5,  20, -20, 120},

    // {4, -3,  2,   2,   2,  2, -3, 4},   
    // {-3, -4,  -1,  -1,  -1,  -1, -4, -3},   
    // {2,  -1,  1,   0,   0,  1,  -1, 2},
    // {2,  -1,   0,   1,   1,   0,  -1, 2},
    // {2,  -1,   0,   1,   1,   0,  -1, 2},
    // {2,  -1,  1,   0,   0,  1,  -1, 2},
    // {-3, -4,  -1,  -1,  -1,  -1, -4, -3},  
    // {4, -3,  2,   2,   2,  2, -3, 4},
  
};

int minMaxAB(const enum piece board[][SIZE], enum piece mine, enum piece opp, int depth, int alpha, int beta, int isMax);
int eval(const enum piece board[][SIZE], enum piece mine);

position* team13Move(const enum piece board[][SIZE], enum piece mine, int secondsleft){
    int numMoves;
    position * allMoves = getPossibleMoves(board, mine, &numMoves);

    int bestScoreInd = 0, bestScore = INT_MIN, curScore;

    for (int i = 0; i < numMoves; i++){

        if (!isValidMove(board, &allMoves[i], mine)) continue;

        enum piece myBoard[SIZE][SIZE];
        copy(myBoard, board);        
        
        
        executeMove(myBoard, &allMoves[i], mine);
        
        curScore = minMaxAB(myBoard, mine, opposite(mine), 4, -INT_MAX, INT_MAX, 0);

        //printf("%d ", curScore);
        if (curScore > bestScore){
            bestScoreInd = i;
            bestScore = curScore;
        }
    }
    
    position* res = malloc(sizeof(position));
    res->x = allMoves[bestScoreInd].x;
    res->y = allMoves[bestScoreInd].y;
    free(allMoves);
    printf("\n");

    return res;
}

int minMaxAB(const enum piece board[][SIZE], enum piece mine, enum piece opp, int depth, int alpha, int beta, int isMax){
    if (depth == 0 || gameOver(board)){
       return eval(board, mine) - eval(board, opp);
    }
    
    int numMoves;
    position * possibleMoves;
    if (isMax){
        possibleMoves = getPossibleMoves(board, mine, &numMoves);
    } else {
        possibleMoves = getPossibleMoves(board, opp, &numMoves);
    }

    if (numMoves == 0){
        free(possibleMoves);
        return score(board, mine) - score(board, opp);
    }

    enum piece tmpBoard[SIZE][SIZE];
    copy(tmpBoard, board);

    if (isMax){
        int best = -INT_MAX;
        
        for (int i = 0; i < numMoves; i++){                        
            executeMove(tmpBoard, &possibleMoves[i], mine); 
            int curScore = minMaxAB(tmpBoard, mine, opp, depth - 1, alpha, beta, 0);
            
            if (curScore > best) best = curScore;

            if (alpha < best) alpha = best;
            if (beta <= alpha) break;
        }
        
        free(possibleMoves);
        return best;
    } else {
        int best = INT_MAX;
        
        for (int i = 0; i < numMoves; i++){        
            executeMove(tmpBoard, &possibleMoves[i], opp);
            int curScore = minMaxAB(tmpBoard, mine, opp, depth - 1, alpha, beta, 1);

            if (curScore < best) best = curScore;

            if (beta < best) beta = best;
            if (beta <= alpha) break;
        }

        free(possibleMoves);
        return best;
    } 
}

int eval(const enum piece board[][SIZE], enum piece mine){
    int total = 0;
    
    for (int i=0; i<SIZE; i++){
        if (board[0][i] == mine) total += 2;
        if (board[i][0] == mine) total += 2;
        if (board[SIZE-1][i] == mine) total += 2;
        if (board[i][SIZE-1] == mine) total += 2;
    }
    for (int i=1; i<SIZE-1; i++){
        for (int j=1; j<SIZE-1; j++){
            if (board[i][j] == mine) total++;
        }
    }

    // for (int i=0; i<SIZE; i++){
    //     for (int j=0; j<SIZE; j++){
    //         if (board[i][j] == mine){
    //             if (i == 0 || i == SIZE-1 || j == 0 || j == SIZE-1) total += 2;
    //             total++;
    //         }
    //     }
    // }

    // for (int i=0; i<SIZE; i++){
    //     for (int j=0; j<SIZE; j++){
    //         if (board[i][j] == mine){
    //             total += boardWeights[i][j];    
    //         }
    //     }
    // }


    return total;
}

// Minmax function with alpha beta pruning for time saving
// Give better score/weight to better moves (corner control), mobility ?
// Negamax algo ?!, better board store as int 64/8?, bitboards wtf  a