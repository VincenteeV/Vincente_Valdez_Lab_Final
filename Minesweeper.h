#define MINESWEEPER_H

#include <notcurses/notcurses.h>
#include <stdbool.h>

typedef struct {
    struct ncplane* plane;
    int is_mine;
    int is_revealed;
    int neighbor_count;
} cell_t;

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