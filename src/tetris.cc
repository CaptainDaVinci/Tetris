#include <iostream>
#include <array>
#include <random>
#include <algorithm>
#include <curses.h>
#include <unistd.h>

auto constexpr REFRESH = 160000;

using Block = std::array<std::array<int, 4>, 4>;

/* 
	Function to print the tetramino block with suitable tetramino 
	number based on block id provided as input. 
*/
void printBlock(int n) {
    switch (n) {
        case 1:
	        attron(COLOR_PAIR(1));
	        printw("%d ", n);
	        break;

        case 2:
	        attron(COLOR_PAIR(2));
	        printw("%d ", n);
	        break;

        case 3:
	        attron(COLOR_PAIR(3));
	        printw("%d ", n);
	        break;

        case 4:
	        attron(COLOR_PAIR(4));
	        printw("%d ", n);
	        break;

        case 5:
	        attron(COLOR_PAIR(5));
	        printw("%d ", n);
	        break;

        case 6:
	        attron(COLOR_PAIR(6));
	        printw("%d ", n);
	        break;

        case 7:
	        attron(COLOR_PAIR(7));
	        printw("%d ", n);
	        break;

        default:
	        attron(COLOR_PAIR(7));
	        printw("  ");
	        break;
    }
}

/* 
	Class definition for the Tetramino Block. 
*/
class Tetromino {
    Block original, temp;

public:
    Block block;
    
    /* Constructor initialising all the blocks value to be 0 */ 
    Tetromino() {
        for (unsigned i = 0; i < block.size(); ++i) {
        	for (unsigned j = 0; j < block[i].size(); ++j) {
        		block[i][j] = 0;
			}
    	}
	}

	/* Assign the type of the block given the block number */ 
    void setBlock(int block) {
        switch (block) {
            case 1:
	            setIBlock();
	            break;

            case 2:
	            setJBlock();
	            break;

            case 3:
	            setLBlock();
	            break;

            case 4:
	            setOBlock();
	            break;

            case 5:
	            setSBlock();
	            break;

            case 6:
            	setTBlock();
            	break;

		    case 7:
            	setZBlock();
            	break;
        }
    }

    void rotate() {
        original = block;

        for (unsigned i = 0; i < block.size(); ++i) {
            for (unsigned j = 0; j < block[i].size(); ++j) {
                temp[j][block[i].size() - 1 - i] = block[i][j];
            }
        }

        block = temp;
    }

    void rotateBack() {
        block = original;
    }

    void drawNextBlock(int blockID) {
        temp = block;
        setBlock(blockID);

        for (unsigned i = 0; i < block.size(); ++i) {
            for (unsigned j = 0; j < block[i].size(); ++j){
                printBlock(block[i][j]);
            }
            printw("\n");
        }

        block = temp;
    }

private:
    void setIBlock() {
        block = {{{0, 1, 0, 0},
  		          {0, 1, 0, 0},
		          {0, 1, 0, 0},
		          {0, 1, 0, 0}}};
    }

    void setJBlock() {
        block = {{{0, 0, 0, 0},
  		          {0, 2, 2, 2},
		          {0, 0, 0, 2},
		          {0, 0, 0, 0}}};
    }

    void setLBlock() {
        block = {{{0, 0, 0, 0},
		          {0, 3, 3, 3},
		          {0, 3, 0, 0},
		          {0, 0, 0, 0}}};
    }

    void setOBlock() {
        block = {{{0, 0, 0, 0},
		          {0, 4, 4, 0},
		          {0, 4, 4, 0},
		          {0, 0, 0, 0}}};
    }

    void setSBlock() {
        block = {{{0, 0, 0, 0},
			       {0, 0, 5, 5},
			       {0, 5, 5, 0},
			       {0, 0, 0, 0}}};
    }

    void setTBlock() {
        block = {{{0, 0, 0, 0},
		          {0, 6, 6, 6},
		          {0, 0, 6, 0},
		          {0, 0, 0, 0}}};
    }

    void setZBlock() {
        block = {{{0, 0, 0, 0},
		          {0, 7, 7, 0},
		          {0, 0, 7, 7},
		          {0, 0, 0, 0}}};
    }

    friend void printBlock(int n);
};

/* Main Tetris class, which uses a Tetromino object */ 
class Tetris  {
    const int points = 5;
    int score, blockID, nextBlockID;
    std::array<std::array<int, 10>, 22> grid;
    Tetromino tetromino;
    std::mt19937 engine;

public:
    Tetris() : engine(std::random_device()()) {

        for (unsigned i = 0; i < grid.size(); ++i) {
        	for (unsigned j = 0; j < grid[i].size(); ++j) {
        		grid[i][j] = 0;
			}
		}

        score = blockID = 0;
    }

    void start() {
        pickBlock(blockID);

        while (!gameOver()) {
            tetromino.setBlock(blockID);
            pickBlock(nextBlockID);
            render();
            clearLines();
            blockID = nextBlockID;
        }

        printw("Good Game!");
        refresh();

        while (true){
            int ch = getch();

            if (ch != ERR && ch == 'e') {
            	break;
			}
        }
    }

private:
    void render() {
        int prevRow = 0, prevCol = 0;
        for (unsigned row = 0, col = 3; row < grid.size(); ++row) {
            if (kbhit()) {
                switch (getch()) {
                    case KEY_LEFT:
	                    col--;
	                    break;

                    case KEY_RIGHT:
                    	col++;
                    	break;

                    case ' ':
                    	tetromino.rotate();

                    	if (isColliding(row, col)) {
                    		tetromino.rotateBack();
						}

                    	break;
                }

                if (isColliding(row, col)) {
                	col = prevCol;
				}
            }

            if (!isColliding(row, col)) {
            	moveBlock(row, col);
            } else{
                moveBlock(prevRow, prevCol);
                break;
            }

            printGrid();
            removeBlock(row, col);
            prevRow = row, prevCol = col;
        }
    }

    void pickBlock(int &block) {
        std::uniform_int_distribution<int> generate(1, 7);
        int selectedBlock;
        do {
            selectedBlock = generate(engine);
        } while (selectedBlock == block);

        block = selectedBlock;
    }

    bool kbhit() {
        int ch = getch();
        if (ch != ERR) {
            ungetch(ch);
            return true;
        }

        return false;
    }

    void moveBlock(const int row, const int col) {
        for (unsigned i = 0; i < tetromino.block.size(); ++i) {
            for (unsigned j = 0; j < tetromino.block[i].size(); ++j) {
                if (tetromino.block[i][j] != 0) {
                    grid[row + i][col + j] = tetromino.block[i][j];
                }
            }
        }
    }

    bool isColliding(const int row, const int col) {
        for (unsigned int i = 0; i < tetromino.block.size(); ++i) {
            for (unsigned int j = 0; j < tetromino.block[i].size(); ++j) {

				if (tetromino.block[i][j] != 0) {

                    if (col + (int)j < 0 || col + j >= grid[0].size()) {
                    	return true;
					}

                    if (grid[row + i][col + j] != 0) {
                    	return true;
					}

                    if (row + i >= grid.size()) {
                    	return true;
					}
                }
            }
        }

        return false;
    }

    void removeBlock(const int row, const int col) {
        for (unsigned i = 0; i < tetromino.block.size(); ++i) {
            for (unsigned j = 0; j < tetromino.block[i].size(); ++j) {
                if (tetromino.block[i][j] != 0) {
                    grid[row + i][col + j] = 0;
                }
            }
        }
    }

    void clearLines() {
        std::vector<int> lineNumber;
        for (unsigned row = 1; row < grid.size(); ++row) {
            if (std::all_of(grid[row].begin(), grid[row].end(),
							[](int i) { return i != 0; })) {
            	lineNumber.push_back(row);
			}
        }

        score += points * lineNumber.size();
        for (int i = lineNumber.size() - 1, k = 0; i >= 0; --i, ++k) {
            for (int row = lineNumber[i] + k; row >= 1; --row) {
                grid[row] = grid[row - 1];
            }
        }
    }

    bool gameOver() {
        if (std::any_of(grid[1].begin(), grid[1].end(),
						[](int i) { return i != 0; })) {
        	return true;
		}

        return false;
    }

    void printGrid() {
        usleep(REFRESH);
        clear();

        attron(COLOR_PAIR(7));

		for (unsigned i = 0; i < grid[0].size(); ++i) {
        	printw("--");
		}

        printw("--\n");

        for (unsigned i = 2; i < grid.size(); ++i) {
            attron(COLOR_PAIR(7));
            printw("|");

            for (unsigned j = 0; j < grid[i].size(); ++j) {
                printBlock(grid[i][j]);
                refresh();
            }

            attron(COLOR_PAIR(7));
            printw("|\n");
        }

        attron(COLOR_PAIR(7));

        for (unsigned i = 0; i < grid[0].size(); ++i) {
        	printw("--");
		}

        printw("--\n\n");

        printw("SCORE: %d\n\n", score);
        printw("Next Block\n");
        tetromino.drawNextBlock(nextBlockID);

        refresh();
    }

    friend void printBlock(int n);
};

/* Driver program to run the Tetris game */ 
int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);

    start_color();
    attron(A_BOLD);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);

    Tetris tetris;
    tetris.start();

    endwin();
}
