#include <stdio.h>
#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include "minesweeper.h"

int grid[ROW][COL];
int proximity[ROW][COL];
int visited[ROW][COL];
int x = ROW/2 + ROW%2 - 1;
int y = COL/2 + COL%2 - 1;
int ch;
int mined = 0;
int gameOver = 0;

void initializeGrid() {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			grid[i][j] = 0b000;
			/* The 3 digits represents whether the square is known, marked, mined */
			proximity[i][j] = 0;
			visited[i][j] = 0;
		}
	}
}

void initializeMines() {
	srand(time(NULL));
	int planted = 0;
	while (planted < MINES) {
		int i = rand()%ROW;
		int j = rand()%COL;
		if (grid[i][j] == 0b000 && i != x && j != y) {
			grid[i][j] = 0b001;
			for (int r = i - 1; r <= i + 1; r++) {
				for (int c = j - 1; c <= j + 1; c++) {
					if (!(r == i && c == j) && r >= 0 && r < ROW && c >= 0 && c < COL) {
						++proximity[r][c];
					}
				}
			}
			++planted;
		}
	}
}

void printGrid() {
	clear();
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			if (grid[i][j] == 0b000 || grid[i][j] == 0b001) {
				printw("? ");
			}
			else if (grid[i][j] == 0b100) {
				if (proximity[i][j] == 0) {
					printw("  ");
				}
				else {
					if (!gameOver) {
						attron(A_BOLD);
					}
					printw("%d ", proximity[i][j]);
					if (!gameOver) {
						attroff(A_BOLD);
					}
				}
			}
			else if (grid[i][j] == 0b101 || grid[i][j] == 0b111) {
				attron(A_BOLD);
				printw("# ");
				attroff(A_BOLD);
			}
			else if (grid[i][j] == 0b010 || grid[i][j] == 0b011) {
				printw("~ ");
			}
		}
		printw("\n");
	}
	printw("\nPress arrow kyes to navigate, F to mark mine, and SPACE to open square");
	move(x, 2*y);
}

void autOpen(int r, int c) {
	if (!(visited[r][c])) {
		visited[r][c] = 1;
		if (grid[r][c] % 0b010 == 0b000) {
			if (grid[r][c] < 0b100) {
				grid[r][c] =0b100;
			}
			if (proximity[r][c] == 0) {
				for (int i = r - 1; i <= r + 1; i++) {
					for (int j = c - 1; j <= c + 1; j++) {
						if (!(i == r && j == c) && i >= 0 && i < ROW && j >=0 && j < COL) {
							autOpen(i, j);
						}
					}
				}
			}
		}
	}
}

void open() {
	if (grid[x][y] < 0b100 && grid[x][y] % 0b100 < 0b010) {
		if (grid[x][y] % 0b010 == 0b001) {
			for (int i = 0; i < ROW; i++) {
				for (int j = 0; j < COL; j++) {
					if (grid[i][j] % 0b010 == 0b001) {
						grid[i][j] = grid[i][j] + 0b100;
					}
				}
			}
			gameOver = 1;
		}
		else {
			autOpen(x, y);
		}
		printGrid();
	}
}

void mark() {
	if (grid[x][y] < 0b100) {
		if (grid[x][y] % 0b100 < 0b010) {
			grid[x][y] = grid[x][y] + 0b010;
		}
		else {
			grid[x][y] = grid[x][y] - 0b010;
		}
		printGrid();
	}
}

void autMark() {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			if (grid[i][j] == 0b001) {
				grid[i][j] = 0b011;
			}
		}
	}
	printGrid();
}

int gameWon() {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			if (grid[i][j] % 0b010 == 0b000 && grid[i][j] < 0b100) {
				return 0;
			}
		}
	}
	autMark();
	return 1;
}

int main() {
	initscr();
	keypad(stdscr, TRUE);
	clear();
	noecho();
	initializeGrid();
	printGrid();
	while ((ch = getch()) != 'q' && ch != 'Q') {
		switch(ch)
		{
			case KEY_UP:
				if (x > 0) {
					move(--x, 2*y);
				}
				break;
			case KEY_DOWN:
				if (x < ROW - 1) {
					move(++x, 2*y);
				}
				break;
			case KEY_LEFT:
				if (y > 0) {
					move(x, 2*(--y));
				}
				break;
			case KEY_RIGHT:
				if (y < COL - 1) {
					move(x, 2*(++y));
				}
				break;
			case ' ':
				if (x != -1) {
					if (mined) {
						open();
					}
					else {
						initializeMines();
						open();
						printGrid();
						mined = 1;
					}
				}
				break;
			case 'f': case 'F':
				if (x != -1) {
					mark();
				}
				break;
			case 'r': case 'R':
				clear();
				x = ROW/2 + ROW%2 - 1;
				y = COL/2 + COL%2 - 1;
				initializeGrid();
				printGrid();
				mined = 0;
				gameOver = 0;
				break;
		}
		if (gameOver) {
			attron(A_BOLD);
			move(ROW, 0);
			printw("GAME OVER\n");
			attroff(A_BOLD);
			clrtoeol();
			printw("Press Q to quit or R to restart");
			x = -1;
			y = -1;
		}
		else if (mined && gameWon()) {
			attron(A_BOLD);
			move(ROW, 0);
			printw("YOU WON!\n");
			attroff(A_BOLD);
			clrtoeol();
			printw("Press Q to quit or R to restart");
			x = -1;
			y = -1;
		}
	}
	endwin();
	return 0;
}
