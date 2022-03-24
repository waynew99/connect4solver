#!/usr/bin/env python3

# plays matches of connect 4 between two (computer or human) players
# for CS 313 HW 4
# DS 3/5/2015

import os, sys, subprocess, time

# constants:

ROWS = 6
COLS = 7
SYM = [' ', 'X', 'O', '#', '*']   # map ints to board symbols (last two indicate win)
NUM = {' ':0, 'X':1, 'O':2}       # map board symbols to ints
TIMELIMIT = 1.0                   # time limit for comuter players in seconds
OK, ILLEGAL, TIMEOUT = 0, 1, 2    # status codes for checking computer moves

def emptyboard():
    """create a 2D array representing the empty board"""
    board = []
    for row in range(ROWS):
        board.append([0] * COLS)
    return board

def printboard(board, quiet):
    """print the board if not in quiet mode"""
    if quiet:
        return
    print("")
    for row in reversed(range(ROWS)):
        line = '| '
        for col in range(COLS):
            line += SYM[board[row][col]] + ' '
        line += '|'
        print(line)
    print('+---------------+')
    print('  1 2 3 4 5 6 7  ')

def board2str(board):
    """encode the board into a string to be read by computer players"""
    s = ''
    for col in range(COLS):
        for row in range(ROWS):
            b = board[row][col]
            if b > 0:
                s += SYM[b]
        s += '\n'
    return s

def isprogram(player):
    """check if player is an executable program in the current directory"""
    return os.path.isfile(player) and os.access(player, os.X_OK)
    
def applymove(board, col, p):
    """places new game piece into board"""
    for row in range(ROWS):
        if board[row][col] == 0:
            board[row][col] = p+1
            return

def programmove(board, p, player, quiet):
    """make a computer player move"""
    instr = board2str(board)
    instr += SYM[p+1] + '\n'  # add whose turn it is
    proc = subprocess.Popen('./' + player,
                            stdin=subprocess.PIPE,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT,
                            universal_newlines=True)
    if list(sys.version_info)[:2] >= [3, 3]:
        try:  # use timeout feature in python 3.3 and later
            result = proc.communicate(instr, timeout=TIMELIMIT)[0]
        except subprocess.TimeoutExpired:
            proc.terminate() # kill process
            return TIMEOUT # illegal move due to timeout
    else:
        result = proc.communicate(instr)[0]  # run without timeout
        
    lastlinepos = result.strip().rfind('\n')+1
    lastline = result[lastlinepos:]   # get last line of output
    if not quiet:
        time.sleep(.1)
        print(result[:lastlinepos])   # print all but last line of output
        print("%s's move (%s): %s" % (player, SYM[p+1], lastline))

    try:
        col = int(lastline) - 1
    except ValueError:
        col = -1
    if col < 0 or col >= COLS or board[ROWS-1][col] != 0:
        return ILLEGAL  # illegal move
    applymove(board, col, p)
    return OK # valid move

def makemove(board, p, player, quiet):
    """make a move by given player with number p (0 or 1)"""
    if isprogram(player):
        return programmove(board, p, player, quiet)
    col = -1
    # loop until get valid move from human
    while col < 0 or col >= COLS or board[ROWS-1][col] != 0:
        try:
            col = int(input("%s's move (%s): " % (player, SYM[p+1]))) - 1
        except ValueError:
            col = -1
    applymove(board, col, p)
    return OK # human moves are always valid

def mark_win(board, r, c, dr, dc, n):  
    """indicate winning pieces in board"""
    if n > 0:
        board[r][c] += 2  # increment board contents by 2 so that win gets indicated via SYM characters
        mark_win(board, r-dr, c-dc, dr, dc, n-1)

def check_win(board, r0, c0, dr, dc):
    """check if someone has won at given location in given direction"""
    r, c = r0, c0
    n = 0
    old = -1
    WIN = 4
    while r >= 0 and r < ROWS and c >= 0 and c < COLS:
        cur = board[r][c]
        if cur != old:    # starting new "run"
            n = 1
            old = cur
        else:             # continuing previous "run"
            n += 1
            if cur > 0 and n == WIN:  #found WIN non-empties in a row
                mark_win(board, r, c, dr, dc, WIN)
                return True
        r += dr
        c += dc
    return False

def gameover(board):
    """check if someone has won"""
    for r in range(ROWS):  # starting at first column r, 0:
        if check_win(board, r, 0,  0, 1):   # check horizontal
            return True
        if check_win(board, r, 0,  1, 1):   # check diag / from left
            return True
        if check_win(board, r, 0, -1, 1):   # check diag \ from left
            return True
    for c in range(COLS):
        if check_win(board, 0, c, 1, 0):   # check vertical
            return True
        if check_win(board, 0, c, 1, 1):   # check diag / from bottom
            return True
        if check_win(board, ROWS-1, c, -1, 1):   # check diag \ from top
            return True
    return False

def winstats(players, p, n, status, quiet):
    """compute score and print winning stats"""
    if n == ROWS*COLS:
        if not quiet:
            print("It's a tie!")
        score0, score1 =  2, 2
    elif status == ILLEGAL:
        if not quiet:
            print("%s loses (illegal move)" % players[p])
        score0, score1 = 0, 1
    elif status == TIMEOUT:
        if not quiet:
            print("%s loses (time limit exceeded)" % players[p])
        score0, score1 = 0, 1
    else: # regular win
        if not quiet:
            print("%s (%s) wins!" % (players[p], SYM[p+1]))
        score0, score1 = 4, 1
    if p == 1: # if second player, swap scores
        score0, score1 = score1, score0
    if not quiet:
        print("Score:  %s - %d,  %s - %d" % (players[0], score0, players[1], score1))
    return score0, score1
    
def match(players, startplayer, quiet):
    """play one match"""
    board = emptyboard()
    status = OK
    n = 0
    p = 1 - startplayer
    while n < ROWS*COLS and status == OK and not gameover(board):
        p = 1 - p  # alternate players, start with p=startplayer
        printboard(board, quiet)
        status = makemove(board, p, players[p], quiet)
        n += 1
    if status == OK:
        printboard(board, quiet)
    return winstats(players, p, n, status, quiet)

def matches(players, ngames, quiet):
    """play multiple matches"""
    score0, score1 = 0, 0
    startplayer = 0
    for i in range(ngames):
        s0, s1 = match(players, startplayer, quiet)
        score0 += s0
        score1 += s1
        startplayer = 1 - startplayer
    return score0, score1

def main():
    """main program"""
    if len(sys.argv) < 3:
        print("")
        print("  usage: %s [-quiet] <player1> <player2> [<ngames>=1]" % sys.argv[0])
        print("")
        print("  each player can be a program or a name of a human")
        print("")
        return
    quiet = sys.argv[1][:2] == '-q'
    argn = 2 if quiet else 1
    p0, p1 = sys.argv[argn], sys.argv[argn+1]
    ngames = 1
    if len(sys.argv) > argn+2:
        ngames = int(sys.argv[argn+2])
    if not (isprogram(p0) and isprogram(p1)):  # no point in quiet mode if humans play
        quiet = False
    score0, score1 = matches([p0, p1], ngames, quiet)
    if quiet:
        print(score0, score1)
    else:
        print("")
        print("Final Score:  %s - %d,  %s - %d" % (p0, score0, p1, score1))
        print("")

if __name__ == "__main__":
    main()
