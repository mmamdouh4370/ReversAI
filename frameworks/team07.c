// Team 07: Ronan & Zen

#include <stdlib.h>
#include <time.h>

#include "reversi_functions.h"
#include "team07.h"

position *team07Move(const enum piece board[][SIZE], enum piece mine, int secondsleft) {
  enum boolean edgePlayed = FALSE;
  
  position *res = malloc(sizeof(position)); // Allocate our chosen move to return

  // Get list of possible moves
  int numMoves = 0;
  position *choices = getPossibleMoves(board, mine, &numMoves);

  // Store the best move and its corresponding index
  int bestI = -1, max = -1;

  // Priority 1: If a possible move would be a corner of the board, immediately select it and break the loop
  for (int i = 0; i < numMoves; i++) {
    if( playIsCorner(choices, i) ) {
      bestI = i;
      res->x = choices[i].x;
      res->y = choices[i].y;
      free(choices);
      return res;
    }
  }  
  
  // Priority 2: If a possible move results in the other player not being able to move, immediately select it
  for(int j = 0; j < numMoves; j++){
    // Resets the copied board each loop
    enum piece copyBoard[SIZE][SIZE];
    copy(copyBoard, board);

    executeMove(copyBoard, &choices[j], mine); // Execute the move on our copied board

    // Check if opposing player cannot move
    if (!canMove(copyBoard, opposite(mine))) {
      res->x = choices[j].x;
      res->y = choices[j].y;
      free(choices);
      return res;
    }
  }

  // Priority 3: Maximize possible mobility from all possible moves until an edge has been taken
  if (!edgePlayed && !edgeTaken(board)) {
    bestI = miniMaxMobility(board, choices, numMoves, mine);
  }
  
  // Priority 4: Iff an edge has already been taken, just obtain the best move by minimax
  else{
    edgePlayed = TRUE;
    bestI = miniMaxPoints(board, choices, numMoves, mine);
  }
  // Copy our move into position struct to return
  res->x = choices[bestI].x;
  res->y = choices[bestI].y;

  // Free the array.
  free(choices);

  // Return selected move.
  return res;
}

// Returns the index of the move that will grant us the highest MINIMUM number of possible moves on our next turn
int miniMaxMobility(const enum piece board[][SIZE], position *choices, int numMoves, enum piece mine){
  int foundNonCorner = 0;
  int highestNumMoves = -1;
  int bestI = -1;
  position *best = malloc(sizeof(position));
  
  // Declare a T/F array that stores whether or not move i can result in the opponent getting a corner and initialize it to all F
  int canResultInEnemyCorner[numMoves];
  for(int idx = 0; idx < numMoves; idx++){
    canResultInEnemyCorner[idx] = 0;
  }

  // Go through our moves
  for (int i = 0; i < numMoves; i++) {

    // Make a copy of the current board
    enum piece copyBoard[SIZE][SIZE];
    copy(copyBoard, board);

    executeMove(copyBoard, &choices[i], mine); // Execute the move on our copied board

    int num_opp_moves = 0;
    position *opponent_choices = getPossibleMoves(copyBoard, opposite(mine), &num_opp_moves); // Stores the opponents possible moves given our move
    
    // Set default minimum num. of moves we can play after move i is performed to an arbitrarily high number
    // and look through possible opponent moves to determine it
    int minAfterI = 999;
    for (int j = 0; j < num_opp_moves; j++) {
      // If any possible opp. move j is a corner, then switch the flag for move i to true
      // If we already found a non-corner move, then we can break out of this loop and skip to move i+1
      if ( playIsCorner(opponent_choices, j) ) {
        canResultInEnemyCorner[i] = 1;
        if (foundNonCorner)
          break;
      }  

      // Create a board in which the opponent's move can be tested, resets each loop
      enum piece copyOppBoard[SIZE][SIZE];
      copy(copyOppBoard, copyBoard);
  
      executeMove(copyOppBoard, &opponent_choices[j], opposite(mine)); // Execute an opponent's possible move
      
      // If we found a new minimum number of moves resulting from move i, update minimum
      int newNumMoves;
      position *our_new_moves = getPossibleMoves(copyOppBoard, mine, &newNumMoves);
      if (newNumMoves < minAfterI)
          minAfterI = newNumMoves;
      free(our_new_moves);
    }
    if (canResultInEnemyCorner[i] == 0) foundNonCorner = 1; // If the move cannot result in the opponent getting a corner, flag its index as such
    
    // If the number of possible moves that can result in the opponent getting a corner is equal to the total number of moves, flag allMovesCan as such
    int numThatCan = 0, allMovesCan = 0;
    for (int k = 0; k<numMoves; k++)
      numThatCan += canResultInEnemyCorner[k];
    if (numThatCan == numMoves)
      allMovesCan = 1;

    // If highestNumMoves has yet to be updated, or if all possible moves can result in the opponent getting a corner, go through with updating highestNumMoves normally
    // Otherwise, ensure that highestNumMoves is only updated if the the possible move can NOT result in the opponent getting a corner
    if ( ( highestNumMoves == -1 || allMovesCan || !canResultInEnemyCorner[i] ) && ( minAfterI >= highestNumMoves) ) {
      // If we found an equal case, randomly pick one of the two choices to preserve
      if (minAfterI == highestNumMoves) {
        int choice = rand() % 2;
        if (choice == 1)
          bestI = i;
      }
      else {
      highestNumMoves = minAfterI;
      bestI = i;
      }
    }
    
  
    free(opponent_choices); // Free opponent choices array
  }

  return bestI;
}

// Returns the index of best move that will create the highest disparity in points between us and the opponent
int miniMaxPoints(const enum piece board[][SIZE], position *choices, int numMoves, enum piece mine) {
  int max_disparity = -100;
  int foundNonCorner = 0;
  int bestI = -1;
  position *best = malloc(sizeof(position));

  // Declare a T/F array that stores whether or not move i can result in the opponent getting a corner and initialize it to all F
  int canResultInEnemyCorner[numMoves];
  for(int idx = 0; idx < numMoves; idx++){
    canResultInEnemyCorner[idx] = 0;
  }

  // Go through our moves
  for (int i = 0; i < numMoves; i++) {
    int curr_opp_score = 0;
    int curr_my_score = 0;
    int max_opp_gain = -1;

    // Make a copy of the current board
    enum piece copyBoard[SIZE][SIZE];
    copy(copyBoard, board);

    executeMove(copyBoard, &choices[i], mine); // Execute the move on our copied board

    int num_opp_moves = 0;
    position *opponent_choices = getPossibleMoves(copyBoard, opposite(mine), &num_opp_moves); // Stores the opponents possible moves given our move

    int start_score_opp = score(copyBoard, opposite(mine)); // Stores the starting score for opponent after our move

    // Look through opponent moves, using a greedy algorithm
    for (int j = 0; j < num_opp_moves; j++) {
      // If any possible opp. move j is a corner, then switch the flag for move i to true
      // If we already found a non-corner move, then we can break out of this loop and skip to move i+1
      if ( playIsCorner(opponent_choices, j) ) {
        canResultInEnemyCorner[i] = 1;
        if (foundNonCorner)
          break;
      }

      // Create a board in which the opponent's move can be tested, resets each loop
      enum piece copyOppBoard[SIZE][SIZE];
      copy(copyOppBoard, copyBoard);

      executeMove(copyOppBoard, &opponent_choices[j], opposite(mine)); // Execute an opponent's possible move

      // If we found a new maximum score for the opponent, update their max. gain
      if ((score(copyOppBoard, opposite(mine)) - start_score_opp) > max_opp_gain) {
        max_opp_gain = score(copyOppBoard, opposite(mine)) - start_score_opp;
        curr_opp_score = score(copyOppBoard, opposite(mine));
        curr_my_score = score(copyOppBoard, mine);
      }
    }
    if (canResultInEnemyCorner[i] == 0) foundNonCorner = 1; // If the move cannot result in the opponent getting a corner, flag its index as such
    
    // Calculate disparity
    int curr_disparity = curr_my_score - curr_opp_score;

    // If the number of possible moves that can result in the opponent getting a corner is equal to the total number of moves, flag allMovesCan as such
    int numThatCan = 0, allMovesCan = 0;
    for (int k = 0; k<numMoves; k++)
      numThatCan += canResultInEnemyCorner[k];
    if (numThatCan == numMoves)
      allMovesCan = 1;

    // If max_disparity has yet to be updated, or if all possible moves can result in the opponent getting a corner, go through with updating max_disparity normally
    // Otherwise, ensure that max_disparity is only updated if the the possible move can NOT result in the opponent getting a corner
    if ( ( max_disparity == -100 || allMovesCan || !canResultInEnemyCorner[i] ) && (curr_disparity >= max_disparity) ) {
      // If we found an equal case, randomly pick one of the two choices to preserve
      if (curr_disparity == max_disparity) {
        int choice = rand() % 2;
        if (choice == 1)
          bestI = i;
      }
      else {
      max_disparity = curr_disparity;
      bestI = i;
      }
    }

    free(opponent_choices); // Free opponent choices array
  }

  return bestI;
}

// Iff the play is a corner move, return TRUE / FALSE if otherwise
enum boolean playIsCorner(position* choices, int i){
  if ( (choices[i].x == (SIZE - 1) || choices[i].x == 0) && (choices[i].y == 0 || choices[i].y == (SIZE -1)) ) return TRUE;
  else return FALSE;
}

// Iff an edge piece has been taken in the match, return TRUE / FALSE if otherwise
enum boolean edgeTaken(const enum piece board[][SIZE]){
  for(int i = 0; i < SIZE; i++){
    if(board[0][i] != '_' || board[SIZE - 1][i] != '_' || board[i][0] != '_' || board[i][SIZE - 1] != '_')
      return TRUE;
  }
  return FALSE;
}