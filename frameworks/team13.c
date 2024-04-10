// Team 13: Mohammed Mamdouh

#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdio.h>

#include "team13.h"
#include "reversi_functions.h"

// Flags for when we adjusting surronding corner weights
int tL = 1, bL = 1, tR = 1, bR = 1;

// Static board weights used to evaluate "strength" of a move, the negatives surronding the corner are adjusted once we take the corner
int boardWeights[SIZE][SIZE] = {
    {150, -20, 15,   8,   8,  15, -20, 150},   
    {-20, -30,  1,  2,  2,  1, -30, -20},   
    {15,  1,  5,   4,   4,  5,  1, 15},
    {8,  2,   4,   2,   2,   4,  2, 8},
    {8,  2,   4,   2,   2,   4,  2, 8},
    {15,  1,  5,   4,   4,  5,  1, 15},
    {-20, -30,  1,  2,  2,  1, -30, -20},  
    {150, -20,  15,   8,   8,  15, -20, 150},
  
};

/*
Our driving algorithm for looking ahead and planning out moves. This is a basic minmax function that relies on alpha beta pruning to 
save heavily on runtime, ending unpromising outcomes early on instead of playing them out until a gameOver or max depth hit. Our basic
flow here is to alternate between maximizing our score and minimizing an opponets score on a tmpBoard, updating our best score
and alpha beta values ending a run early if our two values intersect.
*/
int team13MinMaxAB(const enum piece board[][SIZE], enum piece mine, enum piece opp, int depth, int alpha, int beta, int isMax);

/*
Handles evaluating a game score with heuristics. We evaluate the board score using our board weights, and mobility depending on if we want 
to play more aggresive or defensive.
*/
int team13Eval(const enum piece board[][SIZE], enum piece mine);

// Once we take a corner, the area around it should no longer be negative as we do not risk losing the corner by taking its adjacent spots.
void team13AdjustCornerWeights(const enum piece board[][SIZE], enum piece mine);

/*
Handle main logic for finding moves and returning the best one. Our flow is: get all possible moves, adjust corner weights if a previous move
prompts that, calculate our depth based on gamestate (how many empty squares are left), loop through all possible moves calling minMaxAB on
them and saving the best move from its score, then finally we return this move.
*/
position * team13Move(const enum piece board[][SIZE], enum piece mine, int secondsleft){
    // Get moves.
    int numMoves;
    position * allMoves = getPossibleMoves(board, mine, &numMoves);

    // Setup tracker for best score, and a cur score to compare against best score.
    int bestScoreInd = 0, bestScore = INT_MIN, curScore;

    team13AdjustCornerWeights(board, mine);

    // Adjust depth based on state of game
    int numEmpty = score(board, EMPTY);
    int curDepth;
    if (numEmpty > 50){
        printf("e ");
        curDepth = 4; 
    } else if (numEmpty > 10){
        printf("m ");
        curDepth = 5;
    } else {
        printf("l ");
        curDepth = 8;
    }

    // Loop through all possible moves
    for (int i = 0; i < numMoves; i++){
        // Setup tmp board to use for testing moves.
        enum piece myBoard[SIZE][SIZE];
        copy(myBoard, board);        
        
        // Try out our current move on our tmpBoard.
        executeMove(myBoard, &allMoves[i], mine);

        // Score this move via our minmax function
        curScore = team13MinMaxAB(myBoard, mine, opposite(mine), curDepth, INT_MIN, INT_MAX, 0); 
       printf("%d, ", curScore);

        // Save the index of this move if it beats out our current best score.
        if (curScore > bestScore){
            bestScoreInd = i;
            bestScore = curScore;
        }

    }
    
    // Malloc a new position and save our best move to it, freeing our previous possible moves.
    position * res = malloc(sizeof(position));
    res->x = allMoves[bestScoreInd].x;
    res->y = allMoves[bestScoreInd].y;
    free(allMoves);
    
    printf("%d\n", secondsleft);

    // Return our best move.
    return res;
}

int team13MinMaxAB(const enum piece board[][SIZE], enum piece mine, enum piece opp, int depth, int alpha, int beta, int isMax){
    // Base case, if we hit the end of our depth or the game has ended return our board evaluation.
    if (depth == 0 || gameOver(board)){
       return team13Eval(board, mine) - team13Eval(board, opp);
    }
    
    // Get possible moves for the minimizer or maximizer
    int numMoves;
    position * possibleMoves;
    if (isMax){
        possibleMoves = getPossibleMoves(board, mine, &numMoves);
    } else {
        possibleMoves = getPossibleMoves(board, opp, &numMoves);
    }

    // Second base case, we have no possible moves left and must evaluate the board
    if (numMoves == 0){
        free(possibleMoves);
        return team13Eval(board, mine) - team13Eval(board, opp);
    }

    // Set up a new tmp board, we dont want to mess with the original as we need to backtrack
    enum piece tmpBoard[SIZE][SIZE];
    copy(tmpBoard, board);

    // Swap between maximizer and minimizer
    if (isMax){
        // Set up best, keep it low here to set it to the first bigger score.
        int best = INT_MIN;
        
        // Loop through all moves
        for (int i = 0; i < numMoves; i++){      
            // Test out the move, and score it recursively with our minMaxAB.
            executeMove(tmpBoard, &possibleMoves[i], mine); 
            int curScore = team13MinMaxAB(tmpBoard, mine, opp, depth - 1, alpha, beta, 0);
            
            // Set best to our cur score if it is larger.
            if (curScore > best) best = curScore;

            // Handle alpha beta, breaking out of our loop if alpha is smaller then beta.
            if (alpha < best) alpha = best;
            if (beta <= alpha) break;

        }
        
        // Free our moves and return the best score.
        free(possibleMoves);
        return best;

    } else {
        // Set up best, keep it high here to set it to the first smaller score.
        int best = INT_MAX;
        
        // Loop through all moves
        for (int i = 0; i < numMoves; i++){   
            // Test out the move, and score it recursively with our minMaxAB.
            executeMove(tmpBoard, &possibleMoves[i], opp);
            int curScore = team13MinMaxAB(tmpBoard, mine, opp, depth - 1, alpha, beta, 1);

            // Set best to our cur score if it is smaller.
            if (curScore < best) best = curScore;

            // Handle alpha beta, breaking out of our loop if alpha is smaller then beta.
            if (beta > best) beta = best;
            if (beta <= alpha) break;

        }

        // Free our moves and return the best score.
        free(possibleMoves);
        return best;
    } 
}

int team13Eval(const enum piece board[][SIZE], enum piece mine){
    int total = 0;

    // Get the number of moves for each player, used for mobility.
    int numMoves;
    int numOppMoves;
    position * possibleMoves = getPossibleMoves(board, mine, &numMoves);
    position * possibleOppsMoves = getPossibleMoves(board, opposite(mine), &numOppMoves);
    free(possibleMoves);
    free(possibleOppsMoves);


    // Stability/num of pieces, when there are very few empty spots left do a primitive score count.
    if (score(board, EMPTY) < 10){
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

        // if (numMoves > numOppMoves) {
        //     total += (numMoves * 0.5);
        //     total -= (numOppMoves);
        // } else{
        //     total += (numMoves);
        //     total -= (numOppMoves * 0.5);
        // }

        // Mobility, only account for it before late game.
        
        
    }

    // if (mine == BLACK){
    //     printf("black %d, ", total);
    // }
    return total;
}

void team13AdjustCornerWeights(const enum piece board[][SIZE], enum piece mine){
    if (tL && board[0][0] != EMPTY){
        tL = 0;
        boardWeights[0][1] = 20;
        boardWeights[1][0] = 20;
        boardWeights[1][1] = 1;
    }
    if (bL && board[SIZE-1][0] != EMPTY){
        bL = 0;
        boardWeights[SIZE-2][0] = 20;
        boardWeights[SIZE-1][1] = 20;
        boardWeights[SIZE-2][1] = 1;
    }
    if (tR && board[0][SIZE-1] != EMPTY){
        tR = 0;
        boardWeights[0][SIZE-2] = 20;
        boardWeights[1][SIZE-1] = 20;
        boardWeights[1][SIZE-2] = 1;
    }
    if (bR && board[SIZE-1][SIZE-1] != EMPTY){
        bR = 0;
        boardWeights[SIZE-1][SIZE-2] = 20;
        boardWeights[SIZE-2][SIZE-1] = 20;
        boardWeights[SIZE-2][SIZE-2] = 1;
    }
}

