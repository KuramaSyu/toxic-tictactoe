/*
A more toxic version of TicTacToe from Paul Zenker - 3IT24-1 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

/* 
0 - unused cell
1 - player 1 - human
2 - player 2 - bot
*/
struct board {
    int board[3][3];
    int current_player;
    int winner;
};

/* defines a cell of the board. Score is needed
for the text saying, whether its going to be a draw
*/
struct cell {
    int row;
    int col;
    int score;
};
typedef struct cell cell;
typedef struct board board;

/* protopytes */
board new_board(void);
void print_game_over_ascii_art(void);
void print_draw_ascii_art(void);
void make_turn(board *b);
int has_winner(board *b, int player);
int is_board_full(board *b);
int min(int a, int b);
int max(int a, int b);
int minimax(board *b, int depth, int is_maximising);
cell best_move(board *b);
void print_board(board *b);
cell ask_move(board *b);


/* generates a board with random beginner */
board new_board(void) {
    board b;
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            b.board[i][j] = 0;
        }
    }
    srand(time(NULL));
    b.current_player = (rand() % 2) + 1;
    b.winner = 0;
    return b;
}

/* starts the game. make_turn() calls itself as long as the game is not over. */
int main(void) {
    board b = new_board();
    printf("Player 1 is human, Player 2 is bot.\n");
    print_board(&b);
    make_turn(&b);
    return 0;
} 

/* calculates fastest win for slowest lose for bot or asks the user 
where to place the tick */
void make_turn(board *b) {
    cell c;
    
    if (b->current_player == 2) {
        /* bot turn */
        c = best_move(b);
        b->board[c.row][c.col] = 2;
    } else {
        /* player turn */
        c = ask_move(b);
        b->board[c.row][c.col] = 1;
    }
    if (has_winner(b, b->current_player)) {
        /* Game has reached an ending state */
        /* It's impossible for player to win, except the code has flaws */
        /* -> So I wont even add a winning screen for the player */
        print_board(b);
        print_game_over_ascii_art();
    } else if (is_board_full(b)) {
        /* draw because board is full */
        print_board(b);
        printf("It's a draw! - again\n");
        print_draw_ascii_art();
    } else {
        /* change players and start next turn */
        print_board(b);
        if (c.score > 0) {
            /* if the worst possible outcome is already positiv, then its over */
            printf("It's basically over. You can stop playing. (%d moves until I definitly win)\n", (10 - c.score));
        }
        if (c.score == 0) {
            /* the worst outcome is 0 (draw) - basically all the time */
            printf("It's basically a draw already if we don't make flaws.\n");
        }
        b->current_player = (b->current_player == 1) ? 2 : 1;
        make_turn(b);
    }

    return;
}


/* 
Whether or not the <player> has won the game with 
the given board <b>

returns:
--------
0 - false
1 - true
*/
int has_winner(board *b, int player) {
    int i;
    for (i = 0; i < 3; i++) {
        /* player has a winning row */
        if (b->board[i][0] == player && b->board[i][1] == player && b->board[i][2] == player) {
            return 1;
        }
        /* player has a winning col */
        if (b->board[0][i] == player && b->board[1][i] == player && b->board[2][i] == player) {
            return 1;
        }
    }
    /* player has a winning diagonal */
    if (b->board[0][0] == player && b->board[1][1] == player && b->board[2][2] == player) {
        return 1;
    }
    /* player has a winning diagonal */
    if (b->board[0][2] == player && b->board[1][1] == player && b->board[2][0] == player) {
        return 1;
    }
    return 0;
}

/* returns whether (1) or not (0) the board is full */
int is_board_full(board *b) {
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (b->board[i][j] == 0) {
                return 0;
            }
        }
    }
    return 1;
}

/* Function to return the minimum of two numbers */ 
int min(int a, int b) {
    return (a < b) ? a : b;
}

/* Function to return the maximum of two numbers */ 
int max(int a, int b) {
    return (a > b) ? a : b;
}

/* retunrs an integer between -9 and +9.
negative: player CAN win (depending on skill)
0: best case for player is a draw
positve: it's a guaranteed loss for the player 
*/
int minimax(board *b, int depth, int is_maximising) {
    int i, j, score, best_score;
    
    /* player win */
    if (has_winner(b, 1) == 1) {
        return depth - 10; /* prefer slower losses */
    }
    /* bot win */
    if (has_winner(b, 2) == 1) {
        return 10 - depth;  /* perver faster wins */
    }
    
    /* draw */
    if (is_board_full(b)) {
        return 0;
    }

    if (is_maximising) {
        best_score = -99;
        for (i = 0; i<3; i++) {
            for (j = 0; j<3; j++) {
                if (b->board[i][j] == 0) {
                    /* slot is empty, test with ai slot */
                    b->board[i][j] = 2; 
                    score = minimax(b, depth + 1, 0);
                    b->board[i][j] = 0;
                    best_score = max(best_score, score);
                }
            }
        }
    } else {
        best_score = 99;
        for (i = 0; i<3; i++) {
            for (j = 0; j<3; j++) {
                if (b->board[i][j] == 0) {
                    /* slot is empty, test with player slot */
                    b->board[i][j] = 1; 
                    score = minimax(b, depth + 1, 1);
                    b->board[i][j] = 0;
                    best_score = min(best_score, score);
                }
            }
        }
    }
    return best_score;
}

/* iterates through every free cell, calculating the minimax score for it.
retuns the cell with the best score */
cell best_move(board *b) {
    int i, j, best_score, score;
    cell c;
    best_score = -99;
    for (i = 0; i<3; i++) {
        for (j = 0; j<3; j++) {
            if (b->board[i][j] == 0) {
                /* slot is empty, test with player slot */
                b->board[i][j] = 2; 
                score = minimax(b, 0, 0);
                b->board[i][j] = 0;
                if (score > best_score) {
                    best_score = score;
                    c.row = i;
                    c.col = j; 
                    c.score = score;
                }
            }
        }
    }
    return c;
}

/* clears the terminal */
void clear_terminal(void) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/* clears the board and prints the tic tac toe baord */
void print_board(board *b) {
    int row, col;
    clear_terminal();
    printf("     |  A  |  B  |  C  |\n");
    printf("  -------------------------\n");
    for (row = 0; row < 3; row++) {
        printf("  %d  ", row + 1);
        for (col = 0; col < 3; col++) {
            char c = ' ';
            if (b->board[row][col] == 1) {
                c = 'X';
            } else if (b->board[row][col] == 2) {
                c = 'O';
            }
            printf("|  %c  ", c);
        }
        printf("|\n  -------------------------\n");
    }
}

/* ask for the next cell */
cell ask_move(board *b) {
    cell c;
    char col;
    int row;
    printf("Enter your move (e.g., A1, B2): ");
    scanf(" %c%d", &col, &row);
    /* make col upper */
    col = toupper(col);
    if (col != 'A' && col != 'B' && col != 'C') {
        printf("Invalid column. Please enter A, B, or C.\n");
        return ask_move(b);
    }
    if (row < 1 || row > 3) {
        printf("Invalid row. Please enter 1, 2, or 3.\n");
        return ask_move(b);
    }
    if (b->board[row-1][col-'A'] != 0) {
        printf("Take anther cell\n");
        return ask_move(b);
    }
    /* map A to 1, b to 2, c to 3 */ 
    c.col = col - 'A';  /* substracting ASCCI A results in number 0-2 */
    c.row = row - 1;
    c.score = -10; /* just init it. For the player the score does not matter */
    return c;
}


/* Ascii arts */

void print_game_over_ascii_art(void) {
    printf("\n");
    printf("╭━━━━-╮\n");
    printf("╰┃ ┣▇━▇\n");
    printf(" ┃ ┃  ╰━▅╮ Game Over!\n");
    printf(" ╰┳╯ ╰━━┳╯ You Lost\n");
    printf("  ╰╮ ┳━━╯ Super Easy\n");
    printf(" ▕▔▋ ╰╮╭━╮ Nice Tutorial\n");
    printf("╱▔╲▋╰━┻┻╮╲╱▔▔▔╲\n");
    printf("▏  ▔▔▔▔▔▔▔  O O┃\n");
    printf("╲╱▔╲▂▂▂▂╱▔╲▂▂▂╱\n");
    printf(" ▏╳▕▇▇▕ ▏╳▕▇▇▕\n");
    printf(" ╲▂╱╲▂╱ ╲▂╱╲▂╱\n");
    return;
}

void print_draw_ascii_art(void) {
    printf("\n");
    printf("+-------------------------------------------+\n");
    printf("| _______                                   |\n");
    printf("||       \\                                  |\n");
    printf("|| $$$$$$$\\  ______   ______   __   __   __ |\n");
    printf("|| $$  | $$ /      \\ |      \\ |  \\ |  \\ |  \\|\n");
    printf("|| $$  | $$|  $$$$$$\\ \\$$$$$$\\| $$ | $$ | $$|\n");
    printf("|| $$  | $$| $$   \\$$/      $$| $$ | $$ | $$|\n");
    printf("|| $$__/ $$| $$     |  $$$$$$$| $$_/ $$_/ $$|\n");
    printf("|| $$    $$| $$      \\$$    $$ \\$$   $$   $$|\n");
    printf("| \\$$$$$$$  \\$$       \\$$$$$$$  \\$$$$$\\$$$$ |\n");
    printf("+-------------------------------------------+\n");
}
