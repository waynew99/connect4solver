/* c4randc.c
 * A dumb random player.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define ROWS 6
#define COLS 7

// global variable for the board
// uses 0 for empty cells, 1 and 2 for player 1 and 2
int board[ROWS][COLS] = {{0}};

// maps board contents 0, 1, 2 to characters
char sym[] = {' ', 'X', 'O'};

// simple assert function, quits program with error message if assertion fails
void assert(int cond, char *msg)
{
    if (!cond) {
	fprintf(stderr, "failed assertion %s\n", msg);
	exit(1);
    }
}

// seed random number generator based on on current time in microseconds and board state
void randseed() {
    struct timeval t1;
    gettimeofday(&t1, NULL);
    unsigned int s = t1.tv_usec * t1.tv_sec;
    int row, col;
    for (col = 0; col < COLS; col++) {
	for (row = 0; row < ROWS; row++) {
	    int b = board[row][col];
	    s += (b+1) * (13*row + 37*col);
	}
    }
    srand(s);
}

// read encoded board from stdin:
// 7 strings of 'X' and 'O', representing columns from bottom up
// followed by single X/O representing player to move
// returns player num (1=X, 2=O)
int read_board()
{
    int row = 0, col = 0;
    char c;
    while (col < COLS) {
	c = getchar();
        if (c == '\r') { // carriage return (on Windows)
            continue;
        }
	if (c == 'X' || c == 'O') {
	    assert(row < ROWS, "row < ROWS");
	    board[row++][col] = (c == 'X' ? 1 : 2);
	} else if (c == '\n') {
	    row = 0;
	    col++;
	} else {
	    assert(0, "input symbols must be 'X', 'O', or '\\n'");
	}
    }
    // read one more symbol indicating whose move it is
    c = getchar();
    assert(c == 'X' || c == 'O', "last input symbol must 'X' or 'O'");
    randseed(); // seed random number generator based on board state and current time
    return (c == 'X' ? 1 : 2);
}

// prints board (for debugging)
void print_board()
{
    int row, col;
    for (row = ROWS-1; row >= 0; row--) {
	printf("| ");
	for (col = 0; col < COLS; col++)
	    printf("%c ", sym[board[row][col]]);
	printf("|\n");
    }
    printf("+---------------+\n");
    printf("  0 1 2 3 4 5 6  \n");
}

// make a random move by selecting a non-full column
int make_move()
{
    int r = rand();
    int col;
    for (col = 0; col < COLS; col++) {
	int c = (col + r) % COLS;
	if (board[ROWS-1][c] == 0)
	    return c+1;  // columns are numbered starting at 1
    }
    assert(0, "should find at least one non-full column");
    return 0;
}

int main()
{
    read_board();

    //int p = read_board();
    //printf("player %d's turn\n", p);
    //print_board();
    printf("%d\n", make_move());
    return 0;
}
