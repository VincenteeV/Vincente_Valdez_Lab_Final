#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <notcurses/notcurses.h>
#include "Minesweeper.h"

int main(void)
{
    // 1. Setup locale for UTF-8 (essential for the ▒ character)
    setlocale(LC_ALL, "");
    srand(time(NULL));

    // 2. Initialize Notcurses
    struct notcurses_options nopts = {
        .flags = NCOPTION_SUPPRESS_BANNERS,
    };

    struct notcurses *nc = notcurses_init(&nopts, stdout);
    if (nc == NULL)
    {
        return EXIT_FAILURE;
    }

    GameMode chosen_mode;
    GameSize chosen_size;

    if (runMainMenu(nc, &chosen_mode, &chosen_size) == -1)
    {
        notcurses_stop(nc);
        return EXIT_SUCCESS;
    }

    ncplane_erase(notcurses_stdplane(nc));
    ncplane_set_bg_default(notcurses_stdplane(nc));
    ncplane_set_fg_rgb(notcurses_stdplane(nc), 0xAAAAAA);

    int boardWidth = 9, boardHeight = 9, mineCount = 10;

    if (chosen_size == SIZE_MEDIUM)
    {
        boardWidth = 16;
        boardHeight = 16;
        mineCount = 40;
    }
    else if (chosen_size == SIZE_LARGE)
    {
        boardWidth = 30;
        boardHeight = 16;
        mineCount = 99;
    }
    else if (chosen_size == SIZE_CUSTOM)
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
        while (mineCount > 1000 || mineCount == 0)
        {
            mineCount = getCustomInput(nc, 6, 5, "Please enter the number of mines (1-1000): ");
        }
        ncplane_erase(notcurses_stdplane(nc));
    }
    // 3. Create the board
    cell_t board[boardHeight][boardWidth];
    if (createMinesweeperGrid(nc, boardHeight, boardWidth, board) != 0)
    {
        notcurses_stop(nc);
        return -1;
    }

    // 4. Initialize Game State
    struct ncinput ni;

    gameState state = {
        .cursor_x = 0,
        .cursor_y = 0,
        .boardHeight = boardHeight,
        .boardWidth = boardWidth,
        .mineCount = mineCount,
        .firstMove = true,
        .running = true};

    while (state.running)
    {

        // Update the visual state of all cells (highlights, numbers, etc.)
        drawBoard(nc, &state, board);

        uint32_t key = notcurses_get_blocking(nc, &ni);
        getUserControls(key, &state, board);

    }

    // 5. Cleanup
    notcurses_stop(nc);
    return EXIT_SUCCESS;
}

int createMinesweeperGrid(struct notcurses *nc, int h, int w, cell_t board[h][w])
{
    struct ncplane *stdn = notcurses_stdplane(nc);

    // Plane options blueprint
    struct ncplane_options cell_opts = {
        .rows = 1,
        .cols = 2,
    };

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            // Calculate screen position with some padding
            cell_opts.y = y + 2;
            cell_opts.x = x * 2 + 5;

            // Create the individual cell plane attached to standard plane
            board[y][x].plane = ncplane_create(stdn, &cell_opts);
            if (!board[y][x].plane)
                return -1;

            // Logic state
            board[y][x].is_mine = 0;
            board[y][x].is_revealed = 0;
            board[y][x].is_flagged = 0;
            board[y][x].neighbor_count = 0;

            // Visual state: Set color and string
            ncplane_set_fg_rgb(board[y][x].plane, 0xFFFFFF); // Medium Grey
            ncplane_putstr_yx(board[y][x].plane, 0, 0, "回");
        }
    }

    return 0;
}

int runMainMenu(struct notcurses *nc, GameMode *out_mode, GameSize *out_size)
{
    struct ncplane *stdn = notcurses_stdplane(nc);
    struct ncinput ni;

    int cursor_pos = 0; // 0-2: Modes, 3-6: Sizes, 7: Start
    GameMode selected_mode = MODE_BEGINNER;
    GameSize selected_size = SIZE_SMALL;
    bool running = true;

    while (running)
    {
        // 1. Clear the screen for the new frame
        ncplane_erase(stdn);

        // 2. Draw the Header
        ncplane_set_fg_rgb(stdn, 0x00FF00); // Terminal Green
        ncplane_putstr_yx(stdn, 1, 5, "=== MINESWEEPER SETUP ===");

        // 3. Draw the Options
        const char *options[] = {
            " Normal", " Checkerboard", " Liar",
            " Small (9x9) ", " Medium (16x16)", " Large (30x16)", " Custom",
            " [ START GAME ] "};

        int row_offset = 3;
        for (int i = 0; i < 8; i++)
        {
            ncplane_set_bg_rgb(stdn, 0x000000);
            ncplane_set_fg_rgb(stdn, 0xFFFFFF);
            // Add spacing between sections
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
            bool is_active_mode = (i >= 0 && i <= 2 && (int)selected_mode == i);
            bool is_active_size = (i >= 3 && i <= 6 && (int)selected_size == (i - 3));

            // Set colors based on cursor hover vs selection
            if (cursor_pos == i)
            {
                ncplane_set_bg_rgb(stdn, 0xFFFFFF); // White background (Hover)
                ncplane_set_fg_rgb(stdn, 0x000000); // Black text
            }
            else if (is_active_mode || is_active_size)
            {
                ncplane_set_bg_rgb(stdn, 0x000000);
                ncplane_set_fg_rgb(stdn, 0x00FFFF); // Cyan text for active choice
            }
            else
            {
                ncplane_set_bg_rgb(stdn, 0x000000);
                ncplane_set_fg_rgb(stdn, 0xFFFFFF); // Standard white text
            }

            // Draw the checkbox and text
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "[%c] %s",
                     (is_active_mode || is_active_size) ? 'X' : ' ',
                     options[i]);

            ncplane_putstr_yx(stdn, row_offset++, 5, buffer);
        }

        // 4. Render to the terminal
        notcurses_render(nc);

        // 5. Wait for user input
        uint32_t key = notcurses_get_blocking(nc, &ni);

        // 6. Handle Input State
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
                selected_mode = (GameMode)cursor_pos;
            }
            else if (cursor_pos >= 3 && cursor_pos <= 6)
            {
                selected_size = (GameSize)(cursor_pos - 3);
            }
            else if (cursor_pos == 7)
            {
                running = false; // Start the game!
            }
        }
        else if (key == 'q' || key == 'Q')
        {
            return -1; // Emergency quit
        }
    }

    // Save choices to the pointers so main() can use them
    *out_mode = selected_mode;
    *out_size = selected_size;
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

        ncplane_printf_yx(stdn, y, x, "%-60s", "");

        // Draw the current state
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
            return -1; // Abort
        }
    }

    return atoi(buf);
}

void placeMines(gameState *state, cell_t board[state->boardHeight][state->boardWidth])
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

void countNeighbors(gameState *state, cell_t board[state->boardHeight][state->boardWidth])
{
    for (int y = 0; y < state->boardHeight; y++)
    {
        for (int x = 0; x < state->boardWidth; x++)
        {
            // Skip mine cells; they don't need a neighbor count
            if (board[y][x].is_mine)
                continue;

            int count = 0;

            // Check the 3x3 area around the current cell (y, x)
            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    int ny = y + dy; // neighbor y
                    int nx = x + dx; // neighbor x

                    // 1. BOUNDS CHECK: Ensure neighbor is inside the grid
                    if (ny >= 0 && ny < state->boardHeight && nx >= 0 && nx < state->boardWidth)
                    {
                        // 2. MINE CHECK: Increment count if it's a mine
                        if (board[ny][nx].is_mine)
                        {
                            count++;
                        }
                    }
                }
            }
            board[y][x].neighbor_count = count;
        }
    }
}
void drawBoard(struct notcurses *nc, gameState *state, cell_t board[state->boardHeight][state->boardWidth])
{
    for (int y = 0; y < state->boardHeight; y++)
    {
        for (int x = 0; x < state->boardWidth; x++)
        {
            struct ncplane *p = board[y][x].plane;

            // 1. Determine Highlight
            if (y == state->cursor_y && x == state->cursor_x)
            {
                ncplane_set_bg_rgb(p, 0x444444); // Highlighted cursor background
            }
            else
            {
                ncplane_set_bg_rgb(p, 0x000000); // Standard background
            }

            // 2. Determine Tile Visuals
            if (board[y][x].is_revealed)
            {
                if (board[y][x].is_mine)
                {
                    ncplane_set_fg_rgb(p, 0xFF0000); // Red Mine
                    ncplane_putstr_yx(p, 0, 0, "💣");
                }
                else
                {
                    // Draw the number (neighbor count)
                    ncplane_set_fg_rgb(p, 0x00FF00); // Green numbers
                    char buf[2] = {board[y][x].neighbor_count + '0', '\0'};
                    ncplane_putstr_yx(p, 0, 0, buf);
                }
            }
            else if (board[y][x].is_flagged)
            {
                ncplane_set_fg_rgb(p, 0xFFFF00); // Yellow flag
                ncplane_putstr_yx(p, 0, 0, "⚑");
            }
            else
            {
                ncplane_set_fg_rgb(p, 0x888888); // Hidden tile
                ncplane_putstr_yx(p, 0, 0, "回");
            }
        }
    }
    notcurses_render(nc); // Render everything once after all planes are updated
}

void getUserControls(uint32_t key, gameState *state, cell_t board[state->boardHeight][state->boardWidth])
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
            placeMines(state, board);
            countNeighbors(state, board);
            state->firstMove = 0;
        }
        revealTile(state, board, state->cursor_x, state->cursor_y);

        break;
    case 'f':
    case 'F':
        // handle_flag logic here
        break;
    case 'q':
    case 'Q':
        state->running = false;
        break;
    case NCKEY_ESC:
        state->running = false;
        break;
    }
}

void revealTile(gameState *state, cell_t board[state->boardHeight][state->boardWidth], int x, int y){
    if (board[y][x].is_revealed)
    {
        return;
    }
    else board[y][x].is_revealed = 1;

    if (board[y][x].neighbor_count == 0)
    {
        for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    int ny = y + dy; // neighbor y
                    int nx = x + dx; // neighbor x

                    // 1. BOUNDS CHECK: Ensure neighbor is inside the grid
                    if (ny >= 0 && ny < state->boardHeight && nx >= 0 && nx < state->boardWidth)
                    {
                        revealTile(state, board, nx, ny);
                    }
                }
            }
        
    }
    
}