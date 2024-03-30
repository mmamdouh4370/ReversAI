// Arup Guha
// 2/10/2019
// Framework for COP 3502 Final Project: Reversi

/*** Note: To compile do:

gcc -o reversi.exe reversi.c reversi_functions.c team13.c team21.c

Of course, replace 20 and 21 with whichever teams you want to play
and modify the code that calls those functions accordingly.

***/

/*** Also, there is a facility to play human v human or human v computer.
     Just edit the call from main to play the version you want to.
     Also, the computer v computer version currently waits one second
     between moves (doesn't count against a player) so viewers can sort of
     see the moves. Feel free to comment this out during testing. It's
     currently on lines 148 and 149.
***/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "reversi_functions.h"
#include "team13.h"
#include "team21.h"
#include "reversi.h"

int main(void) {
    computerVComputer();
    return 0;
}

void humanVHuman() {

    // Initialize and print the board. Black goes first.
    enum piece board[SIZE][SIZE];
    initBoard(board);
    printBoard(board);
    enum piece player = BLACK;

    // Go till the game is done.
    while (!gameOver(board)) {

        // We can flip the team if the current team can't move.
        if (!canMove(board, player))
            player = opposite(player);

        // Get this player's move and execute it.
        position* mymove = getUserMove(board, player);
        executeMove(board, mymove, player);
        free(mymove);

        // Print the result and go to the other player.
        printBoard(board);
        player = opposite(player);
    }

    // Get scores.
    int blackScore = score(board, BLACK);
    int whiteScore = score(board, WHITE);

    // Print scores.
    printf("Black's score is %d\n", blackScore);
    printf("White's score is %d\n", whiteScore);

    // Output the result.
    if (blackScore > whiteScore)
        printf("Black wins by %d points.\n", blackScore-whiteScore);
    else if (whiteScore > blackScore)
        printf("White wins by %d points.\n", whiteScore-blackScore);
    else
        printf("It's a tie!\n");
}

void computerVComputer() {

    // Initialize and print the board. Black goes first.
    enum piece board[SIZE][SIZE];
    initBoard(board);
    printBoard(board);
    enum piece player = BLACK;

    int blackTime = MAXTIME;
    int whiteTime = MAXTIME;
    enum boolean blackOutOfTime = FALSE;
    enum boolean whiteOutOfTime = FALSE;

    // Go till the game is done.
    while (!gameOver(board)) {

        // We can flip the team if the current team can't move.
        if (!canMove(board, player))
            player = opposite(player);

        position* mymove = NULL;

        // Black Computer Player
        if (player == BLACK) {

            // Do the move and time it.
            int startT = time(0);
            mymove = team13Move(board, player, blackTime);
            int endT = time(0);

            // Update time.
            blackTime = blackTime - (endT - startT);
            printf("The black team selected row %d, column %d\n", mymove->x, mymove->y);
        }

        // White Computer Player
        else {

            // Do the move and time it.
            int startT = time(0);
            mymove = team21Move(board, player, whiteTime);
            int endT = time(0);

            // Update time.
            whiteTime = whiteTime - (endT - startT);
            printf("The white team selected row %d, column %d\n", mymove->x, mymove->y);
        }

        // Execute then free move.
        executeMove(board, mymove, player);
        free(mymove);

        // Black ran out of time.
        if (blackTime < 0) {
            printf("Sorry, the Black Player ran out of time and loses automatically.\n");
            blackOutOfTime = TRUE;
            break;
        }

        // White ran out of time.
        if (whiteTime < 0) {
            printf("Sorry, the White Player ran out of time and loses automatically.\n");
            whiteOutOfTime = TRUE;
            break;
        }

        // Print the result and go to the other player.
        printBoard(board);
        player = opposite(player);

        // This waits about one second, so we can see the moves scrolling.
        int curT = time(0);
        while (time(0) == curT);
    }

    if (!blackOutOfTime && !whiteOutOfTime) {

        // Get scores.
        int blackScore = score(board, BLACK);
        int whiteScore = score(board, WHITE);

        // Print scores.
        printf("Black's score is %d\n", blackScore);
        printf("White's score is %d\n", whiteScore);

        // Output the result.
        if (blackScore > whiteScore)
            printf("Black wins by %d points.\n", blackScore-whiteScore);
        else if (whiteScore > blackScore)
            printf("White wins by %d points.\n", whiteScore-blackScore);
        else
            printf("It's a tie!\n");
    }

    // Black player ran out of time.
    else if (blackOutOfTime) {
        printf("White wins because the Black Player ran out of time.\n");
    }
    // White player ran out of time.
    else {
        printf("Black wins because the White Player ran out of time.\n");
    }
}

void humanVComputer() {

    // Initialize and print the board. Black goes first.
    enum piece board[SIZE][SIZE];
    initBoard(board);
    printBoard(board);
    enum piece player = BLACK;

    int compTime = MAXTIME;
    enum boolean outOfTime = FALSE;

    // Go till the game is done.
    while (!gameOver(board)) {

        // We can flip the team if the current team can't move.
        if (!canMove(board, player))
            player = opposite(player);

        position* mymove = NULL;

        // Hard-coded human first.
        if (player == BLACK)
            mymove = getUserMove(board, player);

        // Computer Second.
        else {

            // Do the move and time it.
            int startT = time(0);
            mymove = team13Move(board, player, compTime);
            int endT = time(0);

            // Update time.
            compTime = compTime - (endT - startT);
            printf("The computer selected row %d, column %d\n", mymove->x, mymove->y);
        }

        // Execute then free move.
        executeMove(board, mymove, player);
        free(mymove);

        // Time issue.
        if (compTime < 0) {
            printf("Sorry, the computer ran out of time and loses automatically.\n");
            outOfTime = TRUE;
            break;
        }

        // Print the result and go to the other player.
        printBoard(board);
        player = opposite(player);
    }

    if (!outOfTime) {

        // Get scores.
        int blackScore = score(board, BLACK);
        int whiteScore = score(board, WHITE);

        // Print scores.
        printf("Black's score is %d\n", blackScore);
        printf("White's score is %d\n", whiteScore);

        // Output the result.
        if (blackScore > whiteScore)
            printf("Black wins by %d points.\n", blackScore-whiteScore);
        else if (whiteScore > blackScore)
            printf("White wins by %d points.\n", whiteScore-blackScore);
        else
            printf("It's a tie!\n");
    }

    // Ran out of time.
    else {
        printf("Computer has defaulted and lost.\n");
    }
}

