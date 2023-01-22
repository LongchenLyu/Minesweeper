all: minesweeper

minesweeper: minesweeper.c minesweeper.h
	gcc -o minesweeper.o minesweeper.c -lncurses

clean:
	$(RM) minesweeper.o
