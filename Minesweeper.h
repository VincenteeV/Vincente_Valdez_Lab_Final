#define MINESWEEPER_H

#include <notcurses/notcurses.h>
#include <stdbool.h>

typedef struct {
    struct ncplane* plane;
    bool is_mine;
    bool is_revealed;
    bool is_flagged;
    int neighbor_count;
} cell_t;

typedef struct {
    int cursor_y, cursor_x;
    int boardHeight, boardWidth;
    int mineCount;
    bool running;
    bool firstMove;
} gameState;

typedef enum {
    MODE_BEGINNER, MODE_INTERMEDIATE, MODE_EXPERT
} GameMode;

typedef enum {
    SIZE_SMALL, SIZE_MEDIUM, SIZE_LARGE, SIZE_CUSTOM
} GameSize;
// Function to set up the board

int createMinesweeperGrid(struct notcurses* nc, int h, int w, cell_t board[h][w]);

int runMainMenu(struct notcurses* nc, GameMode* out_mode, GameSize* out_size);

int getCustomInput(struct notcurses* nc, int y, int x, const char* prompt);

void placeMines(gameState *state, cell_t board[state->boardHeight][state->boardWidth]);

void drawBoard(struct notcurses *nc, gameState *state, cell_t board[state->boardHeight][state->boardWidth]);

void countNeighbors(gameState *state, cell_t board[state->boardHeight][state->boardWidth]);

void getUserControls(uint32_t key, gameState *state, cell_t board[state->boardHeight][state->boardWidth]);

void revealTile(gameState *state, cell_t board[state->boardHeight][state->boardWidth], int x, int y);