#include <stdlib.h>
#include <time.h>

#include "team14.h"
#include "reversi_functions.h"


// Minmax function with alpha beta pruning for time saving
// Give better score/weight to better moves (corner control), mobility ?
// Negamax algo ?!, better board store as int 64/8?, bitboards wtf  a
position* team14Move(const enum piece board[][SIZE], enum piece mine, int secondsleft){
    int numMoves;
    position * allMoves = getPossibleMoves(board, mine, &numMoves);

    int bestScoreInd = -1, bestScore = -1, curScore;

    for (int i = 0; i < numMoves; i++){

        enum piece myBoard[SIZE][SIZE];
        copy(myBoard, board);

        //Run moves calc score here
        curScore = count(myBoard, mine);

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