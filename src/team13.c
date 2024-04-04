/* TODO */
// Improve native reversi functions
// Add bitboards
// Comment and clean code
// Remove any unnecesary functions

#include <stdlib.h>
#include <time.h>

#include <limits.h>
#include <stdio.h>
#include <stdint.h>

#include "team13.h"
#include "reversi_functions.h"

int tL = 1, bL = 1, tR = 1, bR = 1;

 
int boardWeights[SIZE][SIZE] = {
    {100, -10, 11,   6,   6,  11, -10, 100},   
    {-10, -20,  1,  2,  2,  1, -20, -10},   
    {10,  1,  5,   4,   4,  5,  1, 10},
    {6,  2,   4,   2,   2,   4,  2, 6},
    {6,  2,   4,   2,   2,   4,  2, 6},
    {10,  1,  5,   4,   4,  5,  1, 10},
    {-10, -20,  1,  2,  2,  1, -20, -10},  
    {100, -10,  11,   6,   6,  11, -10, 100},
  
};

int team13MinMaxAB(const enum piece board[][SIZE], enum piece mine, enum piece opp, int depth, int alpha, int beta, int isMax);
int team13Eval(const enum piece board[][SIZE], enum piece mine);
void team13AdjustCornerWeights(const enum piece board[][SIZE], enum piece mine);

position* team13Move(const enum piece board[][SIZE], enum piece mine, int secondsleft){
    int numMoves;
    position * allMoves = getPossibleMoves(board, mine, &numMoves);

    int bestScoreInd = 0, bestScore = INT_MIN, curScore;

    //Adjust corner weights, seemingly is good for black but worse for white ??!?
    team13AdjustCornerWeights(board, mine);

    int numEmpty = score(board, EMPTY);
    int curDepth;
    if (numEmpty > 40){
        //printf("early ");
        curDepth = 3; 
    } else if (numEmpty > 25){
        //printf("midish");
        curDepth = 3;
    } else if (numEmpty > 10){
        //printf("mid ");
        curDepth = 4;
    } else {
        //printf("late ");
        curDepth = 5;
    }
     

    for (int i = 0; i < numMoves; i++){

        enum piece myBoard[SIZE][SIZE];
        copy(myBoard, board);        
        
        executeMove(myBoard, &allMoves[i], mine);

        curScore = team13MinMaxAB(myBoard, mine, opposite(mine), curDepth, INT_MIN, INT_MAX, 0); 

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

int team13MinMaxAB(const enum piece board[][SIZE], enum piece mine, enum piece opp, int depth, int alpha, int beta, int isMax){
    if (depth == 0 || gameOver(board)){
       return team13Eval(board, mine) - team13Eval(board, opp);
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
        return team13Eval(board, mine) - team13Eval(board, opp);
    }

    enum piece tmpBoard[SIZE][SIZE];
    copy(tmpBoard, board);

    if (isMax){

        int best = INT_MIN;
        
        for (int i = 0; i < numMoves; i++){      

            executeMove(tmpBoard, &possibleMoves[i], mine); 
            int curScore = team13MinMaxAB(tmpBoard, mine, opp, depth - 1, alpha, beta, 0);
            
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
            int curScore = team13MinMaxAB(tmpBoard, mine, opp, depth - 1, alpha, beta, 1);

            if (curScore < best) best = curScore;

            if (beta < best) beta = best;
            if (beta <= alpha) break;

        }

        free(possibleMoves);
        return best;
    } 
}

int team13Eval(const enum piece board[][SIZE], enum piece mine){
    int total = 0;

    // Mobility, adjust aggresiveness/defesiveness
    int numMoves;
    int numOppMoves;
    position * possibleMoves = getPossibleMoves(board, mine, &numMoves);
    position * possibleOppsMoves = getPossibleMoves(board, opposite(mine), &numOppMoves);
    free(possibleMoves);
    free(possibleOppsMoves);


    // Stability/num of pieces
    if (score(board, EMPTY) < 5){
        for (int i = 0; i < SIZE; i++){
            for (int j = 0; j < SIZE; j++){
                if (board[i][j] == mine) total++;
            }
        }
    } else {
        for (int i = 0; i < SIZE; i++){
            for (int j = 0; j < SIZE; j++){
                if (board[i][j] == mine){
                    total += boardWeights[i][j];    
                }
            }
        } 

        if (numMoves > numOppMoves) {
            total += (numMoves);
            total -= (numOppMoves * 2);
        } else{
            total += (numMoves * 2);
            total -= (numOppMoves);
        }
        
    }

    return total;
}

void team13AdjustCornerWeights(const enum piece board[][SIZE], enum piece mine){
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

