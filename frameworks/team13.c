#include <stdlib.h>
#include <time.h>

#include <limits.h>
#include <stdio.h>

#include "team13.h"
#include "reversi_functions.h"

int tL = 1, bL = 1, tR = 1, bR = 1;

int boardWeights[SIZE][SIZE] = {
    // {120, -20,  20,   5,   5,  20, -20, 120},   
    // {-20, -40,  -5,  -5,  -5,  -5, -40, -20},   
    // {20,  -5,  15,   3,   3,  15,  -5,  20},
    // {5,  -5,   3,   3,   3,   3,  -5,   5},
    // {5,  -5,   3,   3,   3,   3,  -5,   5},
    // {20,  -5,  15,   3,   3,  15,  -5,  20},
    // {20, -40,  -5,  -5,  -5,  -5, -40, -20},  
    // {120, -20,  20,   5,   5,  20, -20, 120},

    {100, -10, 11,   6,   6,  11, -10, 100},   
    {-10, -20,  1,  2,  2,  1, -20, -10},   
    {10,  1,  5,   4,   4,  5,  1, 10},
    {6,  2,   4,   2,   2,   4,  2, 6},
    {6,  2,   4,   2,   2,   4,  2, 6},
    {10,  1,  5,   4,   4,  5,  1, 10},
    {-10, -20,  1,  2,  2,  1, -20, -10},  
    {100, -10,  11,   6,   6,  11, -10, 100},
  
};

int minMaxAB(const enum piece board[][SIZE], enum piece mine, enum piece opp, int depth, int alpha, int beta, int isMax);
int eval(const enum piece board[][SIZE], enum piece mine);
void adjustCornerWeights(enum piece board[][SIZE], enum piece mine);

position* team13Move(const enum piece board[][SIZE], enum piece mine, int secondsleft){
    int numMoves;
    position * allMoves = getPossibleMoves(board, mine, &numMoves);

    int bestScoreInd = 0, bestScore = INT_MIN, curScore;

    //Adjust corner weights, seemingly is good for black but worse for white ??!?
    adjustCornerWeights(board, mine);

    for (int i = 0; i < numMoves; i++){

        if (!isValidMove(board, &allMoves[i], mine)) continue;

        enum piece myBoard[SIZE][SIZE];
        copy(myBoard, board);        
        
        executeMove(myBoard, &allMoves[i], mine);

        // if (secondsleft < 30){
        //     curScore = minMaxAB(myBoard, mine, opposite(mine), 4, -INT_MAX, INT_MAX, 0);
        // } else if (39 > secondsleft && secondsleft > 30){
        //     curScore = minMaxAB(myBoard, mine, opposite(mine), 6, -INT_MAX, INT_MAX, 0);    
        // } else {
        //     curScore = minMaxAB(myBoard, mine, opposite(mine), 5, -INT_MAX, INT_MAX, 0);
        // }

        if (secondsleft > 35){
            curScore = minMaxAB(myBoard, mine, opposite(mine), 7, -INT_MAX, INT_MAX, 0); 
        } else {
            curScore = minMaxAB(myBoard, mine, opposite(mine), 4, -INT_MAX, INT_MAX, 0);
        }


        //printf("%d ", curScore);
        if (curScore > bestScore){
            bestScoreInd = i;
            bestScore = curScore;
        }
    }
    
    position * res = malloc(sizeof(position));
    res->x = allMoves[bestScoreInd].x;
    res->y = allMoves[bestScoreInd].y;
    free(allMoves);
    
    printf("%d\n", secondsleft);

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
        return eval(board, mine) - eval(board, opp);
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

    // Stability
    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            if (board[i][j] == mine){
                total += boardWeights[i][j];    
            }
        }
    }

    // Mobility, adjust aggresiveness/defesiveness
    int numMoves;
    int numOppMoves;
    position * possibleMoves = getPossibleMoves(board, mine, &numMoves);
    position * possibleOppsMoves = getPossibleMoves(board, opposite(mine), &numOppMoves);
    free(possibleMoves);

    if (numMoves > 10) {
        total += (numMoves * 4);
        total -= (numOppMoves * 3);
    } else if (numMoves > 5) {
        total += (numMoves * 3);
        total -= (numOppMoves * 2);
    } else if (numMoves > 2) {
        total += (numMoves * 2);
        total -= (numOppMoves);
    } else {
        total += numMoves;
    }

    return total;
}

void adjustCornerWeights(enum piece board[][SIZE], enum piece mine){
    if (tL && board[0][0] == mine){
        tL = 0;
        boardWeights[0][1] = 11;
        boardWeights[1][0] = 10;
        boardWeights[1][1] = 1;
    }
    if (bL && board[SIZE-1][0] == mine){
        bL = 0;
        boardWeights[SIZE-2][0] = 11;
        boardWeights[SIZE-1][1] = 10;
        boardWeights[SIZE-2][1] = 1;
    }
    if (tR && board[0][SIZE-1] == mine){
        tR = 0;
        boardWeights[0][SIZE-2] = 11;
        boardWeights[1][SIZE-1] = 10;
        boardWeights[1][SIZE-2] = 1;
    }
    if (bR && board[SIZE-1][SIZE-1] == mine){
        bR = 0;
        boardWeights[SIZE-1][SIZE-2] = 11;
        boardWeights[SIZE-2][SIZE-1] = 10;
        boardWeights[SIZE-2][SIZE-2] = 1;
    }
}

// Minmax function with alpha beta pruning for time saving
// Give better score/weight to better moves (corner control), mobility ?
// Negamax algo ?!, better board store as int 64/8?, bitboards wtf  a