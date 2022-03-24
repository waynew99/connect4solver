# connect4solver

A connect4 solver in C, using static analysis. Future improvement: Min-Max.

Directory contents:
match.py      - the main match playing script
Makefile      - Of course
connect4.c    - the connect4 player using static analysis
c4randc.c     - a dumb connect4 player who plays randomly
in1.txt, in2.txt, in3.txt  - sample board encodings for testing, e.g. type  ./connect4 < in3.txt

Usage:
1. `make`
2. then run the match.py script:
   - ./match.py                           - shows usage
  - ./match.py John Lily                 - lets two humans play against each other
  - ./match.py John connect4             - lets human play against a computer player
  - ./match.py connect4 c4randc          - plays match between two computer players 
  - ./match.py -q John connect4          - 'quiet' mode, only shows final score
  - ./match.py -q c4constc c4randpas 10  - plays 10 games in 'quiet' mode, shows total final score

Starter code obtained from cs313-f21 Programming Lnguages at Middlebury College.
