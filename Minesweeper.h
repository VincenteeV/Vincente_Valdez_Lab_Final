#pragma once

#include <notcurses/notcurses.h>
#include <stdbool.h>

typedef enum
{
    NORMAL,
    CHECKERBOARD,
    LIAR
} GameMode;

typedef enum
{
    SMALL,
    MEDIUM,
    LARGE,
    CUSTOM
} GameSize;

/**
 * @brief Contains the notcurses plane for each cell aswell as different info to be stored for each individual cell on the board.
 *
 * @var::plane
 * The notcurses plane that shows and represents each cell
 * @var::is_mine
 * A bool representing whether or not the cell is a mine
 * @var::is_revealed
 * A bool representing whether or not the cell has been flagged by the player
 * @var::is_flagged
 * A bool representing whether or not the cell has been flagged by the player
 * @var::neighbor_count
 * An int representing the number of mines adjacent to the cell in all 8 directions, maximum of 8 during gameplay
 * @var::display_count
 * An int representing the number shown on the tile if neighbor_count is greater than 0, seperate from actual neighbor_count to allow for LIAR gamemode
 */
typedef struct
{
    struct ncplane *plane;
    bool is_mine;
    bool is_revealed;
    bool is_flagged;
    int neighbor_count;
    int display_count;
} cell;

/**
 * @brief A struct that stores the current info of the game for each loop and the main not curses plane of the board
 *
 * @var::running
 * Whether or not the game is still being played by the user
 * @var::firstMove
 * Whether or not it is the players first move, updated after the player makes their first move and is used to determine when to place and count bombs
 * @var::userWon
 * Whether the player won or lost the game, true if the player won, false if they lost
 * @var::int cursor_y, cursor_x
 * The y and x coordinated of the players cursor that is controlled with WASD
 * @var::boardHeight, boardWidth
 * The width and height of the playable board chosen by the user, small, medium, large, or a custom chosen size
 * @var::mineCount
 * The number of mines on the board that need to be found by the player, has 3 set values relating to board size or a custom mine count
 * @var::flagCount
 * The number of flags that the user has placed down on the board, can be increased with the player placing flags or removed by the player removing the flag or
 * the tile that the flag is on
 * @var::variant
 * A GameMode enum that represents which version of minesweeper the player wants to play, NORMAL, LIAR, or CHECKERBOARD
 * @var::nc
 * The main standard plane of the playing board that all of the cells and other ui elements are on top of
 */
typedef struct
{
    bool running;
    bool firstMove;
    bool userWon;
    int cursor_y, cursor_x;
    int boardHeight, boardWidth;
    int mineCount;
    int flagCount;
    GameMode variant;
    struct notcurses *nc;
} gameState;

/**
 * @brief Runs a main menu that allows the user to select the gamemode they want, what size board they want, and then saves that data
 *
 * @param[in] nc: Pointer to base notcurses context for rendering the menu and its options
 * @param[out] mode: The gamemode selected by the user in the menu
 * @param[out] size: The size of the board selected by the user in the menu, including custom sized board
 *
 * @return Int representing whether the player quit. -1 means the user quit, 0 means the user continued and started the game
 */
int runMainMenu(struct notcurses *nc, GameMode *mode, GameSize *size);

/**
 * @brief A function that prints a given prompt using notcurses and returns a positive int value that the user entered.
 *
 * @param[in] nc: Pointer to base notcurses context for rendering prompt and taking in user input on one line
 * @param[in] y: The y value where the prompt will be printed
 * @param[in] x: The x value where the prompt will be printed
 * @param[in] prompt: The string prompt that will be printed out using notcurses that will be printed out behind the area where the user enters the input
 */
int getCustomInput(struct notcurses *nc, int y, int x, const char *prompt);

/**
 * @brief Populates the cells of the passed in 3d array with their default parameters and creates the nc planes for each cell based on the users chosen parameters
 *
 * @param[in] nc: Pointer to base notcurses context for rendering everything else, in this case the board and its tiles
 * @param[in] h: The height of the playing board chosen by the user
 * @param[in] w: The width of the playing board chosen by the user
 * @param[out] board: The 3d array of cells that are populated with default parameters and nc planes in the function
 *
 * @return An int value with either -1 or 0 representing if all of the nc planes were succesfully created, -1 is not succesful, 0 is succesful
 */
int createMinesweeperGrid(struct notcurses *nc, int h, int w, cell board[h][w]);

/**
 * @brief Places a certain amount of mines around the board and avoids the players current cursor location.
 *
 * @param[in] state: The gamestate of the board used to check how many mines are to be placed on the board
 * @param[in,out] board: The array of cells where .isMine is updated for each cell a mine is placed on
 */
void placeMines(gameState *state, cell board[state->boardHeight][state->boardWidth]);

/**
 * @brief Used to count the number of neighbors in all 8 directions next to each cell that isn't a mine. Also aligns actual neighbors with displayed neighbors based of game variant.
 *
 * @param[in] state: Gamestate used to check size of board for loop and to apply check the variant of the game to apply accurate neighbor counts.
 * @param[in,out] board: Used to check whether a cell is a mine. And also used to assign display and actual neighbor counts to each cell.
 */
void countNeighbors(gameState *state, cell board[state->boardHeight][state->boardWidth]);

/**
 * @brief Draws the board based on the parameters of the cells in the board array, including whether a cell is revealed, flagged , and its displayed neighbors.
 * Also checks if the user has won by checking by all of the tiles have been revealed and all flags are on mine cells
 *
 * @param[in,out] state: gamestate used to check size parameters of board for loop aswell as cursor position and the variant of the game.
 * Also used to update playerWon if all win conditions are met.
 * @param[in] board: Used to check state of cells and update visuals based on different parameters of each cell.
 */
void drawBoard(gameState *state, cell board[state->boardHeight][state->boardWidth]);

/**
 * @brief Recieves user input and updates the game based on the user input including moving cursor, flagging and clearing tiles, and quitting game.
 *
 * @param[in] key: The key that the user pressed to be translated to the appropriate in game action.
 * @param[in,out] state: Used to update the cursor position, check if it is the game first move and then update that, and also to update the flag count.
 * @param[in,out] board: Used to ensure the cursor stays inside the limits of the board aswell as to update whether a cell is flagged or not.
 */
void getUserControls(uint32_t key, gameState *state, cell board[state->boardHeight][state->boardWidth]);

/**
 * @brief Reveals the tile at the x and y positions given by the parameters and then recursively reveals the tiles neighbors if the tile has a neighbor count of zero.
 * Also end the game if the revealed tile is a bomb.
 *
 * @param[in,out] state: Used to ensure revealing logic stays inside of the board size limits and also updates the flag count if the user reveals a flagged tile.
 * @param[in,out] board: Used to check if a cell is already revealed in which case it returns immideatly, also checks if the cell is a mine in which case it ends the game.
 * Used to check if the cell has been flagged which it then removes the flag on the cell and updates the count. Also updates .isRevealed for the cell that has been revealed.
 * @param[in] x: The x value of the cell to be revealed
 * @param[in] y: The y value of the cell to be revealed
 */
void revealTile(gameState *state, cell board[state->boardHeight][state->boardWidth], int x, int y);

/**
 * @brief Displays the ending graphics with a red YOU LOST or a green YOU WON depending on how the game ended.
 *
 * @param[in] state: Used to make the ending graphics appear in the direct center of the board aswell as to check whether or not the player won.
 */
void gameOver(gameState *state);