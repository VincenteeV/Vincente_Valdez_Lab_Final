#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <notcurses/notcurses.h>
#include "Minesweeper.h"

int main(void)
{
    setlocale(LC_ALL, "");
    srand(time(NULL)); // Seed random with time to ensure randomized board

    struct notcurses_options nopts = {
        .flags = NCOPTION_SUPPRESS_BANNERS,
    };

    struct notcurses *nc = notcurses_init(&nopts, stdout);
    if (nc == NULL)
    {
        return EXIT_FAILURE;
    }

    GameMode chosenMode;
    GameSize chosenSize;

    // Run main menu function to get chosen size and gamemode from user
    if (runMainMenu(nc, &chosenMode, &chosenSize) == -1)
    {
        notcurses_stop(nc);
        return EXIT_SUCCESS;
    }

    ncplane_erase(notcurses_stdplane(nc));
    ncplane_set_bg_default(notcurses_stdplane(nc));
    ncplane_set_fg_rgb(notcurses_stdplane(nc), 0xAAAAAA);

    int boardWidth = 9, boardHeight = 9, mineCount = 10; // Set parameters to smallest sized board by default, then update if different choice is chosen

    if (chosenSize == MEDIUM)
    {
        boardWidth = 16;
        boardHeight = 16;
        mineCount = 40;
    }
    else if (chosenSize == LARGE)
    {
        boardWidth = 30;
        boardHeight = 16;
        mineCount = 99;
    }
    else if (chosenSize == CUSTOM) // Update board height and width to align with selected size
    {
        boardHeight = getCustomInput(nc, 2, 5, "Please enter the number of rows: ");
        while (boardHeight > 30 || boardHeight == 0)
        {
            boardHeight = getCustomInput(nc, 2, 5, "Please enter the number of rows (1-30): ");
        }

        boardWidth = getCustomInput(nc, 4, 5, "Please enter the number of columns: ");
        while (boardWidth > 50 || boardWidth == 0)
        {
            boardWidth = getCustomInput(nc, 4, 5, "Please enter the number of columns (1-50): ");
        }
        mineCount = getCustomInput(nc, 6, 5, "Please enter the number of mines: ");
        while (mineCount > boardHeight * boardWidth || mineCount == 0) // Reprompt user for mine count if number of mines exceeds number of spaces on board
        {
            int size = boardHeight * boardWidth;

            char prompt[64];
            snprintf(prompt, sizeof(prompt), "Please enter the number of mines (1-%d): ", size);

            mineCount = getCustomInput(nc, 6, 5, prompt);
        }
        ncplane_erase(notcurses_stdplane(nc));
    }

    cell board[boardHeight][boardWidth]; // Create a 3d array of cell structs to represent each tile on the board
    if (createMinesweeperGrid(nc, boardHeight, boardWidth, board) != 0)
    {
        notcurses_stop(nc);
        return -1;
    }

    struct ncplane_options hopts = {
        .y = 1,
        .x = 5,
        .rows = 1,
        .cols = 40,
    };

    struct ncplane *hud_plane = ncplane_create(notcurses_stdplane(nc), &hopts); // Create plane for HUD

    gameState state = {// Initialize gameState with default options
                       .firstMove = true,
                       .running = true,
                       .userWon = false,
                       .cursor_x = 0,
                       .cursor_y = 0,
                       .boardHeight = boardHeight,
                       .boardWidth = boardWidth,
                       .mineCount = mineCount,
                       .flagCount = 0,
                       .variant = chosenMode,
                       .nc = nc};

    struct ncinput ni;

    while (state.running)
    {
        // Redraw Hud every iteration
        ncplane_erase(hud_plane);
        ncplane_set_fg_rgb(hud_plane, 0x00FF00);
        ncplane_printf_yx(hud_plane, 0, 0, "Total Mines: %d | Remaining Mines: %d",
                          state.mineCount,
                          state.mineCount - state.flagCount);

        // Redraw board with updated tiles every iteration
        drawBoard(&state, board);

        // Get input from user
        uint32_t key = notcurses_get_blocking(nc, &ni);
        getUserControls(key, &state, board);
    }

    // Delete board after game stops running
    notcurses_stop(nc);
    return EXIT_SUCCESS;
}

int runMainMenu(struct notcurses *nc, GameMode *Mode, GameSize *Size)
{
    struct ncplane *stdn = notcurses_stdplane(nc);
    struct ncinput ni;

    int cursor_pos = 0;
    GameMode selectedMode = NORMAL;
    GameSize selectedSize = SMALL;
    bool running = true;

    while (running)
    {
        // Clear the screen for the new frame
        ncplane_erase(stdn);

        // Draw the Header
        ncplane_set_fg_rgb(stdn, 0x00FF00);
        ncplane_putstr_yx(stdn, 1, 5, "=== MINESWEEPER SETUP ===");

        // Draw the Options
        const char *options[] = {
            " Normal", " Checkerboard", " Liar",
            " Small (9x9) ", " Medium (16x16)", " Large (30x16)", " Custom",
            " [ START GAME ] "};

        int row_offset = 3;
        // 0-2: Modes, 3-6: Sizes, 7: Start
        for (int i = 0; i < 8; i++)
        {
            ncplane_set_bg_rgb(stdn, 0x000000);
            ncplane_set_fg_rgb(stdn, 0xFFFFFF);

            if (i == 0)
            {
                ncplane_set_fg_rgb(stdn, 0x888888);
                ncplane_putstr_yx(stdn, row_offset++, 5, "SELECT MODE:");
            }
            if (i == 3)
            {
                row_offset++;
                ncplane_set_fg_rgb(stdn, 0x888888);
                ncplane_putstr_yx(stdn, row_offset++, 5, "SELECT SIZE:");
            }
            if (i == 7)
            {
                row_offset++;
            } // Gap before start button

            // Determine if this item is currently selected by the user
            bool isActiveMode = (i >= 0 && i <= 2 && (int)selectedMode == i);
            bool isActiveSize = (i >= 3 && i <= 6 && (int)selectedSize == (i - 3));

            // Set colors based on cursor hover vs selection
            if (cursor_pos == i)
            {
                ncplane_set_bg_rgb(stdn, 0xFFFFFF);
                ncplane_set_fg_rgb(stdn, 0x000000);
            }
            else if (isActiveMode || isActiveSize)
            {
                ncplane_set_bg_rgb(stdn, 0x000000);
                ncplane_set_fg_rgb(stdn, 0x00FFFF);
            }
            else
            {
                ncplane_set_bg_rgb(stdn, 0x000000);
                ncplane_set_fg_rgb(stdn, 0xFFFFFF);
            }

            // Draw the checkbox and text
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "[%c] %s",
                     (isActiveMode || isActiveSize) ? 'X' : ' ',
                     options[i]);

            ncplane_putstr_yx(stdn, row_offset++, 5, buffer);
        }
        notcurses_render(nc);

        uint32_t key = notcurses_get_blocking(nc, &ni);

        // Process user input
        if (key == 'w' || key == 'W')
        {
            cursor_pos = (cursor_pos > 0) ? cursor_pos - 1 : 7;
        }
        else if (key == 's' || key == 'S')
        {
            cursor_pos = (cursor_pos < 7) ? cursor_pos + 1 : 0;
        }
        else if (key == NCKEY_ENTER)
        {
            if (cursor_pos >= 0 && cursor_pos <= 2)
            {
                selectedMode = (GameMode)cursor_pos;
            }
            else if (cursor_pos >= 3 && cursor_pos <= 6)
            {
                selectedSize = (GameSize)(cursor_pos - 3);
            }
            else if (cursor_pos == 7)
            {
                running = false; // Start Game
            }
        }
        else if (key == 'q' || key == 'Q')
        {
            return -1; // Quit Game
        }
    }

    // Save choices to the pointers so main() can use them
    *Mode = selectedMode;
    *Size = selectedSize;
    return 0;
}

int getCustomInput(struct notcurses *nc, int y, int x, const char *prompt)
{
    struct ncplane *stdn = notcurses_stdplane(nc);
    char buf[16] = {0};
    int pos = 0;
    struct ncinput ni;

    while (true)
    {
        // Clear line to be printed on
        ncplane_printf_yx(stdn, y, x, "%-60s", "");

        // Print out prompt
        ncplane_set_fg_rgb(stdn, 0xFFFFFF);
        ncplane_printf_yx(stdn, y, x, "%s %s_", prompt, buf);
        notcurses_render(nc);

        uint32_t key = notcurses_get_blocking(nc, &ni);

        if (key == NCKEY_ENTER || key == '\n' || key == '\r')
        {
            if (pos == 0)
                continue; // Don't allow empty input
            break;
        }
        else if (key == NCKEY_BACKSPACE && pos > 0)
        {
            buf[--pos] = '\0';
        }
        else if (key >= '0' && key <= '9' && pos < 5)
        {
            buf[pos++] = (char)key;
            buf[pos] = '\0';
        }
        else if (key == 'q' || key == 'Q')
        {
            return -1;
        }
    }

    return atoi(buf);
}

int createMinesweeperGrid(struct notcurses *nc, int h, int w, cell board[h][w])
{
    struct ncplane *stdn = notcurses_stdplane(nc);

    // Plane options blueprint, 2 columns wide to make cells square
    struct ncplane_options cell_opts = {
        .rows = 1,
        .cols = 2,
    };

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            // Calculate screen position padding
            cell_opts.y = y + 2;
            cell_opts.x = x * 2 + 5;

            // Create the individual cell plane attached to standard plane
            board[y][x].plane = ncplane_create(stdn, &cell_opts);
            if (!board[y][x].plane)
                return -1;

            // Create logic state for each cell
            board[y][x].is_mine = 0;
            board[y][x].is_revealed = 0;
            board[y][x].is_flagged = 0;
            board[y][x].neighbor_count = 0;
            board[y][x].display_count = 0;
        }
    }

    // No errors with plane creation if function returns 0
    return 0;
}

void placeMines(gameState *state, cell board[state->boardHeight][state->boardWidth])
{
    int placed = 0;
    while (placed < state->mineCount)
    {
        // Generate random coordinates within board bounds
        int ry = rand() % state->boardHeight;
        int rx = rand() % state->boardWidth;

        // Only place a mine if there isn't one there already
        if (board[ry][rx].is_mine == 0 && ry != state->cursor_y && rx != state->cursor_x)
        {
            board[ry][rx].is_mine = 1;
            placed++;
        }
    }
}

void countNeighbors(gameState *state, cell board[state->boardHeight][state->boardWidth])
{
    for (int y = 0; y < state->boardHeight; y++)
    {
        for (int x = 0; x < state->boardWidth; x++)
        {
            // Skip mine cells
            if (board[y][x].is_mine)
                continue;

            int count = 0;

            // Check the 3x3 area around the current cell
            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    int ny = y + dy; // neighbor y
                    int nx = x + dx; // neighbor x

                    // Ensure neighbor is inside the grid
                    if (ny >= 0 && ny < state->boardHeight && nx >= 0 && nx < state->boardWidth)
                    {
                        // Increment count if it's a mine
                        if (board[ny][nx].is_mine)
                        {
                            count++;
                            if (state->variant == CHECKERBOARD && (ny + nx) % 2 == 0)
                            {
                                count++;
                            }
                        }
                    }
                }
            }
            // Align displayed count with actual count
            board[y][x].neighbor_count = count;
            board[y][x].display_count = count;

            // If user is playing the Liar variant, update the display count to be +-1 from the actual count
            if (state->variant == LIAR && count > 0)
            {
                board[y][x].display_count = count + (rand() % 3) - 1;
            }
        }
    }
}
void drawBoard(gameState *state, cell board[state->boardHeight][state->boardWidth])
{
    bool allTilesRevealed = true;
    for (int y = 0; y < state->boardHeight; y++)
    {
        for (int x = 0; x < state->boardWidth; x++)
        {
            struct ncplane *p = board[y][x].plane;

            // Determine Cursor Highlight
            if (y == state->cursor_y && x == state->cursor_x)
            {
                ncplane_set_bg_rgb(p, 0x00FF00);
            } // If user is playing checkerboard variant, every other tile should be different color than default
            else if (state->variant == CHECKERBOARD && (y + x) % 2 == 0 && board[y][x].is_revealed)
            {
                ncplane_set_bg_rgb(p, 0x3B444B);
            }
            else
            {
                ncplane_set_bg_default(p);
            }

            // Determine Revealed Tile Visuals
            if (board[y][x].is_revealed)
            {

                if (board[y][x].is_mine)
                {
                    ncplane_putstr_yx(p, 0, 0, "💣");
                }
                else
                {
                    switch (board[y][x].display_count)
                    {
                    // Different colors for every number and 0's are blank
                    case 0:
                        ncplane_putstr_yx(p, 0, 0, "  ");
                        break;
                    case 1:
                        ncplane_set_fg_rgb(p, 0x0000FF);
                        ncplane_putstr_yx(p, 0, 0, "1");
                        break;
                    case 2:
                        ncplane_set_fg_rgb(p, 0x008000);
                        ncplane_putstr_yx(p, 0, 0, "2");
                        break;
                    case 3:
                        ncplane_set_fg_rgb(p, 0xFF0000);
                        ncplane_putstr_yx(p, 0, 0, "3");
                        break;
                    case 4:
                        ncplane_set_fg_rgb(p, 0x9400D3);
                        ncplane_putstr_yx(p, 0, 0, "4");
                        break;
                    case 5:
                        ncplane_set_fg_rgb(p, 0xFF1493);
                        ncplane_putstr_yx(p, 0, 0, "5");
                        break;
                    case 6:
                        ncplane_set_fg_rgb(p, 0x00FFFF);
                        ncplane_putstr_yx(p, 0, 0, "6");
                        break;
                    case 7:
                        ncplane_set_fg_rgb(p, 0xFFFFFF);
                        ncplane_putstr_yx(p, 0, 0, "7");
                        break;
                    case 8:
                        ncplane_set_fg_rgb(p, 0x2F4F4F);
                        ncplane_putstr_yx(p, 0, 0, "8");
                        break;
                    }
                }
            }
            else if (board[y][x].is_flagged)
            {
                if (state->variant == CHECKERBOARD && (y + x) % 2 == 0)
                {
                    // Set background to match checkerboard
                    ncplane_set_bg_rgb(p, 0x3B444B);
                }

                ncplane_set_fg_rgb(p, 0xFFFF00);
                ncplane_putstr_yx(p, 0, 0, "⚑");
                if (!board[y][x].is_mine)
                {
                    allTilesRevealed = false;
                }
            }
            else
            {
                ncplane_set_fg_rgb(p, 0x888888);
                ncplane_putstr_yx(p, 0, 0, "回");
                allTilesRevealed = false;
                // If there are any hidden tiles, the user hasn't finished the game
            }
        }
    }
    if (allTilesRevealed)
    {
        // User wins game if there are no more hidden tiles and all flagged tiles are mines
        state->userWon = true;
        gameOver(state);
    }

    notcurses_render(state->nc); // Render everything once after all cells are updated
}

void getUserControls(uint32_t key, gameState *state, cell board[state->boardHeight][state->boardWidth])
{

    switch (key)
    {
    case 'w':
    case 'W':
        if ((state->cursor_y) > 0)
            (state->cursor_y)--;
        break;
    case 's':
    case 'S':
        if ((state->cursor_y) < state->boardHeight - 1)
            (state->cursor_y)++;
        break;
    case 'a':
    case 'A':
        if ((state->cursor_x) > 0)
            (state->cursor_x)--;
        break;
    case 'd':
    case 'D':
        if ((state->cursor_x) < state->boardWidth - 1)
            (state->cursor_x)++;
        break;
    case 'c':
    case 'C':
        if (state->firstMove)
        {
            // If it's the players first move, place and count the mines on the board and update firstMove
            placeMines(state, board);
            countNeighbors(state, board);
            state->firstMove = 0;
        }
        // Then reveal the tile the cursor is on
        revealTile(state, board, state->cursor_x, state->cursor_y);
        break;
    case 'f':
    case 'F':
        if (board[state->cursor_y][state->cursor_x].is_flagged)
        {
            // If tile is flagged remove flag
            board[state->cursor_y][state->cursor_x].is_flagged = 0;
            state->flagCount--;
        }
        else if (state->flagCount < state->mineCount && !board[state->cursor_y][state->cursor_x].is_revealed)
        {
            // If tile is hidden and not flagged remove flag
            board[state->cursor_y][state->cursor_x].is_flagged = 1;
            state->flagCount++;
        }

        break;
    // Q and Esc quit game
    case 'q':
    case 'Q':
        state->running = false;
        break;
    case NCKEY_ESC:
        state->running = false;
        break;
    }
}

void revealTile(gameState *state, cell board[state->boardHeight][state->boardWidth], int x, int y)
{
    // If tile is already revealed escape early
    if (board[y][x].is_revealed)
    {
        return;
    }
    // If tile is a mine then reveal the rest of the mines on the board and perform gameover sequence
    if (board[y][x].is_mine)
    {
        for (int i = 0; i < state->boardHeight; i++)
        {
            for (int j = 0; j < state->boardWidth; j++)
            {
                if (board[i][j].is_mine)
                {
                    board[i][j].is_revealed = 1;
                }
            }
        }
        gameOver(state);
    }
    else
        board[y][x].is_revealed = 1;

    if (board[y][x].is_flagged)
    {
        // If there is a flag on the revealed tile then remove it and update the flag count
        board[y][x].is_flagged = false;
        state->flagCount--;
    }

    // Perform recursive reveal of all 8 neighbors if the tile has zero neighbors
    if (board[y][x].neighbor_count == 0 && !board[y][x].is_mine)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            for (int dx = -1; dx <= 1; dx++)
            {
                int ny = y + dy; // neighbor y
                int nx = x + dx; // neighbor x

                // Ensure neighbor is inside the grid
                if (ny >= 0 && ny < state->boardHeight && nx >= 0 && nx < state->boardWidth)
                {
                    revealTile(state, board, nx, ny);
                }
            }
        }
    }
}

void gameOver(gameState *state)
{
    // Default colored background overlay around text to make it look good
    struct ncplane_options bg_opts = {
        .y = (state->boardHeight / 2),
        .x = state->boardWidth - 2,
        .rows = 5,
        .cols = 15,
    };

    struct ncplane_options opts = {
        .y = 1 + (state->boardHeight / 2),
        .x = (state->boardWidth),
        .rows = 1,
        .cols = 11,
    };

    struct ncplane *std = notcurses_stdplane(state->nc);
    struct ncplane *bg_p = ncplane_create(std, &bg_opts);
    struct ncplane *gameOver_p = ncplane_create(std, &opts);

    ncplane_set_bg_rgb(bg_p, 0x000000);
    ncplane_set_base(bg_p, 0, 0, ' ');

    // Render text
    if (state->userWon)
    {
        ncplane_set_bg_rgb(gameOver_p, 0x008000);
        ncplane_putstr_yx(gameOver_p, 0, 0, " YOU WON! ");
    }
    else
    {
        ncplane_set_bg_rgb(gameOver_p, 0xFF0000);
        ncplane_putstr_yx(gameOver_p, 0, 0, " YOU LOST! ");
    }

    ncplane_set_fg_rgb(bg_p, 0xAAAAAA);
    ncplane_putstr_yx(bg_p, 2, 0, "Press Q to Quit");
}