CC = gcc
CFLAGS = -W -Wall -O2 -g

BIN = c4randc connect4

all: $(BIN)

r: connect4
	./connect4

d: connect4
	sudo gdb connect4

clean:
	rm -rf $(BIN) *.o core* __pycache__ *.dSYM
