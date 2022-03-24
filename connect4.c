/*
 * connect4.c
 * A (kinda) smart player using static analysis.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <limits.h>

#define ROWS 6
#define COLS 7

#define TRUE 1
#define FALSE 0

#define SIZE 10 // malloc string length

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

void setBoard(int r, int c, int val)
{
    assert(r<ROWS && c<COLS && (val==1 || val==2 || val==0), "setBoard failed due to invalid r, c, or val.");
    board[r][c] = val;
}

// 0 for empty, 1 for 'X', 2 for 'O'
int getBoard(int r, int c)
{
    assert(r<ROWS && c<COLS, "getBoard failed due to invalid r, c, or val.");
    return board[r][c];
}

// 0 for empty, 1 for 'X', 2 for 'O', 3 for out-of-bound
int tryGetBoard(int r, int c)
{
	return (r<ROWS && c<COLS) ? board[r][c] : 3;
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

// Find the horizontal pattern string of length l that includes [r][c]
char *patternCheckHorizontal(int i, int l, int r, int c)
{
    char *current = malloc(SIZE);
    char *int_str = malloc(SIZE);
	sprintf(current, "");
    int firstCoor = c - l + i + 1;
	int lastCoor = c + i;
	if (firstCoor>=0 && lastCoor<COLS) {
		for (int j=firstCoor; j<=lastCoor; j++) {
            sprintf(int_str, "%d", tryGetBoard(r, j));
            strcat(current, int_str);
		}
	}
	free(int_str);
    return current;
}

// Find the vertical pattern string of length l that includes [r][c]
char *patternCheckVertical(int i, int l, int r, int c)
{
    char *current = malloc(SIZE);
    char *int_str = malloc(SIZE);
	sprintf(current, "");
    int firstCoor = r - l + i + 1;
	int lastCoor = r + i;
    if (firstCoor>=0 && lastCoor<ROWS) {
		for (int j=firstCoor; j<=lastCoor; j++) {
            sprintf(int_str, "%d", tryGetBoard(j, c));
            strcat(current, int_str);
		}
	}
	free(int_str);
    return current;
}

// Find the lower-left to upper-right pattern string of length l that includes [r][c]
char *patternCheckLLUR(int i, int l, int r, int c)
{
    char *current = malloc(SIZE);
    char *int_str = malloc(SIZE);
	sprintf(current, "");
    int firstRowCoor = r - l + i + 1;
	int lastRowCoor = r + i;
    int firstColCoor = c - l + i + 1;
	int lastColCoor = c + i;
	if (firstRowCoor>=0 && lastRowCoor<ROWS && firstColCoor>=0 && lastColCoor<COLS) {
		for (int a=0; a<l; a++) {
            sprintf(int_str, "%d", tryGetBoard(firstRowCoor+a, firstColCoor+a));
            strcat(current, int_str);
		}
    }
	free(int_str);
    return current;
}

// Find the lower-right to upper-left pattern string of length l that includes [r][c]
char *patternCheckLRUL(int i, int l, int r, int c)
{
    char *current = malloc(SIZE);
    char *int_str = malloc(SIZE);
	sprintf(current, "");
    int firstRowCoor = r - l + i + 1;
	int lastRowCoor = r + i;
    int firstColCoor = c + l - i - 1;
	int lastColCoor = c - i;
	if (firstRowCoor>=0 && lastRowCoor<ROWS && firstColCoor<COLS && lastColCoor>=0) {
		for (int a=0; a<l; a++) {
            sprintf(int_str, "%d", tryGetBoard(firstRowCoor+a, firstColCoor-a));
            strcat(current, int_str);
		}
    }
	free(int_str);
    return current;
}

// Checks if [row][col] belongs to the targetPattern
int patternCheck(int row, int col, char *targetPattern)
{
    int i;
    int l = strlen(targetPattern);

    char *horizontal;
    char *vertical;
	char *LLUR;
	char *LRUL;

    for (i=0; i<l; i++) {
		int found = FALSE;

        horizontal = patternCheckHorizontal(i, l, row, col);
        vertical = patternCheckVertical(i, l, row, col);
		LLUR = patternCheckLLUR(i, l, row, col);
		LRUL = patternCheckLRUL(i, l, row, col);

        if (strcmp(horizontal, targetPattern)==0 ||
			strcmp(vertical, targetPattern)==0 ||
			strcmp(LLUR, targetPattern)==0 ||
			strcmp(LRUL, targetPattern)==0) {
                found = TRUE;
			}

		free(horizontal);
		free(vertical);
		free(LLUR);
		free(LRUL);
		if (found) {
			return TRUE;
		}
	}
    return FALSE;
}

// Checks if the winning move is able to be placed having a base beneath it.
// Returns FALSE(0) if having insuffieciet base(empty cell beneath),
// Otherwise return the piece beneath it, 1/2 for the two players, 3 for out of bound.
int sufficientBaseForWinning(int row, int col, char *targetPattern)
{	
	int i;
    int l = 4; //length of targetPattern
	int pattern_num = targetPattern[0]=='0'; // 0 means pattern xxx0, 1 means pattern 0xxx
	int win_r=-1, win_c=-1;

    char *horizontal;
    char *vertical;
	char *LLUR;
	char *LRUL;

    for (i=0; i<l; i++) {
        horizontal = patternCheckHorizontal(i, l, row, col);
        vertical = patternCheckVertical(i, l, row, col);
		LLUR = patternCheckLLUR(i, l, row, col);
		LRUL = patternCheckLRUL(i, l, row, col);

		if (strcmp(horizontal, targetPattern)==0) {
			if (pattern_num) {
				// look left 
				win_r = row;
				win_c = col-l+i+1;
			} else {
				// look right
				win_r = row;
				win_c = col+i;
			}
			break;
		} else if (strcmp(vertical, targetPattern)==0) {
			// no need to check vertical, for sure it has a base
			free(horizontal);
			free(vertical);
			free(LLUR);
			free(LRUL);
			return TRUE;
		} else if (strcmp(LLUR, targetPattern)==0) {
			if (pattern_num) {
				win_r = row-l+i+1;
				win_c = col-l+i+1;
			} else {
				win_r = row+i;
				win_c = col+i;
			}
			break;
		} else if (strcmp(LRUL, targetPattern)==0) {
			if (pattern_num) {
				win_r = row-l+i+1;
				win_c = col+l-i-1;
			} else {
				win_r = row+i;
				win_c = col-i;
			}
			break;
		}
	}

	free(horizontal);
	free(vertical);
	free(LLUR);
	free(LRUL);
	if (win_r>-1 && win_c>-1) {
		return tryGetBoard(win_r-1, win_c);
	} else {
		assert(0, "Problem, suffienct base checking");
		exit(1);
	}
}

/*
 * Static valuation is done to board[r][c] for the given player.
 * Diffent scenarios are given diffent weights as following.
 * 12: Attack xxxx
 * 11: Defend oooo
 * 10: Attack xxx_ || _xxx with sufficient base on the winning move
 * 9: Attack _x^x_ (double two special checking)
 * 8: Attack xxx_ || _xxx with insufficient base on the winning move
 * 7: Defend _o^o_ (double two special checking)
 * 6: defend ooo_ || _ooo with sufficient base on the winning move
 * 5: defend ooo_ || _ooo with insufficient base on the winning move
 * 3: Defend _oo_
 * 2: Attack xx
 * 1: Defend oo
 */
int getHint(int r, int c, int player_num)
{
	assert(player_num==1 || player_num==2, "player_num is neither 1 or 2, invalid");

	int val = -1;
	int oppo_num = 3-player_num;
	char self[2];
	char opponent[2];
	char pattern[10];
	char pattern1[10];

	sprintf(self, "%d", player_num);
	sprintf(opponent, "%d", oppo_num);
 
	// 1: Defend oo
	sprintf(pattern, "%s%s", opponent, opponent);
	setBoard(r, c, oppo_num);
	if (patternCheck(r, c, pattern)) val = 1;
	setBoard(r, c, 0);

	// 2: Attack xx
	sprintf(pattern, "%s%s", self, self);
	setBoard(r, c, player_num);
	if (patternCheck(r, c, pattern)) val = 2;
	setBoard(r, c, 0);

	// 3: Defend _oo_
	sprintf(pattern, "0%s%s0", opponent, opponent);
	setBoard(r, c, oppo_num);
	if (patternCheck(r, c, pattern)) val = 3;
	setBoard(r, c, 0);

	// 4: Attack _xx_
	sprintf(pattern, "0%s%s0", self, self);
	setBoard(r, c, player_num);
	if (patternCheck(r, c, pattern)) val = 4;
	setBoard(r, c, 0);

	// 5, 6: defend ooo_ || _ooo
	sprintf(pattern, "%s%s%s0", opponent, opponent, opponent);
	sprintf(pattern1, "0%s%s%s", opponent, opponent, opponent);
	setBoard(r, c, oppo_num);
	if (patternCheck(r, c, pattern)) {
		// whether with sufficient base on the winning move
		val = sufficientBaseForWinning(r, c, pattern) ? 6 : 5;
	} else if (patternCheck(r, c, pattern1)) {
		val = sufficientBaseForWinning(r, c, pattern1) ? 6 : 5;
	}
	setBoard(r, c, 0);

	// 7: Defend _o^o_ (double two special checking)
	// TODO: if time allows, implement this.

	// 9: Attack _x^x_ (double two special checking)
	// TODO: if time allows, implement this.

	// 8, 10: 8: Attack xxx_ || _xxx with insufficient base on the winning move
	sprintf(pattern, "%s%s%s0", self, self, self);
	sprintf(pattern1, "0%s%s%s", self, self, self);
	setBoard(r, c, player_num);
	if (patternCheck(r, c, pattern)) {
		// whether with sufficient base on the winning move
		val = sufficientBaseForWinning(r, c, pattern) ? 10 : 8;
	} else if (patternCheck(r, c, pattern1)) {
		val = sufficientBaseForWinning(r, c, pattern1) ? 10 : 8;
	}
	setBoard(r, c, 0);

	// 11: Defend oooo
	sprintf(pattern, "%s%s%s%s", opponent, opponent, opponent, opponent);
	setBoard(r, c, oppo_num);
	if (patternCheck(r, c, pattern)) val = 11;
	setBoard(r, c, 0);

	// 12: Attack xxxx
	sprintf(pattern, "%s%s%s%s", self, self, self, self);
	setBoard(r, c, player_num);
	if (patternCheck(r, c, pattern)) val = 12;
	setBoard(r, c, 0);

	return val;
}

// takes in the current player number, and make the best move for that player
int make_move(int my_player_num)
{
	assert(my_player_num==1 || my_player_num==2, "my_player_num in make_move() is not valid.");

    int best_col = -1;
	int best_val = -INT_MAX;
	int hints[7];
	int upper_check_hint;

	for (int c=0; c<COLS; c++) {
		// Initialize each value to -INT_MAX 
		// to prevent making a move at an already full column.
		hints[c] = -INT_MAX; 
		for (int r=0; r<ROWS; r++) {
			if (getBoard(r, c)==0) {
				hints[c] = getHint(r, c, my_player_num);

				// check if putting at column c gives base to an opponent winning move
				if (r<ROWS-1 && hints[c]!=12) {
					upper_check_hint = getHint(r+1, c, 3-my_player_num);
					if (upper_check_hint==12) {
						// This is higher than -INT_MAX
						// because allowing an opponent to win is better than
						// making an illegal move.
						hints[c] = -1; 
					}
				}

				if (hints[c]>best_val) {
					best_val = hints[c];
					best_col = c;
				}
				break;
			}
		}
	}


	// if there is a draw, choose the one closest to the middle.
	for (int i=0; i<COLS; i++) {
		if (hints[i]==best_val) {
			if (abs(i-3) < abs(best_col-3)) {
				best_col = i;
			}
		}
	}
	return best_col>=-1 ? best_col+1 : 3;
}

int main()
{
    int p = read_board();
    printf("%d\n", make_move(p));
    return 0;
}
