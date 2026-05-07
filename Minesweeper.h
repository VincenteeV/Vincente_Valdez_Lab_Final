#pragma once

#include <notcurses/notcurses.h>
#include <stdbool.h>

/**
 * @brief
 */
typedef enum {
    NORMAL, CHECKERBOARD, LIAR
} GameMode;

typedef enum {
    SIZE_SMALL, SIZE_MEDIUM, SIZE_LARGE, SIZE_CUSTOM
} GameSize;

typedef struct {
    struct ncplane* plane;
    bool is_mine;
    bool is_revealed;
    bool is_flagged;
    int neighbor_count;
    int display_count;
} cell_t;

typedef struct {
    bool running;
    bool firstMove;
    bool userWon;
    int cursor_y, cursor_x;
    int boardHeight, boardWidth;
    int mineCount;
    int flagCount;
    struct notcurses *nc;
    GameMode variant;
} gameState;

int createMinesweeperGrid(struct notcurses* nc, int h, int w, cell_t board[h][w]);

int runMainMenu(struct notcurses* nc, GameMode* out_mode, GameSize* out_size);

int getCustomInput(struct notcurses* nc, int y, int x, const char* prompt);

void placeMines(gameState *state, cell_t board[state->boardHeight][state->boardWidth]);

void drawBoard(gameState *state, cell_t board[state->boardHeight][state->boardWidth]);

void countNeighbors(gameState *state, cell_t board[state->boardHeight][state->boardWidth]);

void getUserControls(uint32_t key, gameState *state, cell_t board[state->boardHeight][state->boardWidth]);

void revealTile(gameState *state, cell_t board[state->boardHeight][state->boardWidth], int x, int y);

void gameOver(gameState *state);