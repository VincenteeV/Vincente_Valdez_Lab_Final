The minesweeper program is controlled using keyboard inputs:
W - Move cursor up
A - Move cursor left
S - Move cursor down
D - Move cursor right

C - Clear Tile
F - Flag Tile

ENTER - Make Menu Selection
Q and ESC - Quit game

The main menu will show at first where the user can select the gamemode, the size of the game, and then start the game. The user can select one size and one gamemode using enter. If the user selects a custom sized game then a new screen will show which allows the user to enter a height (Max of 30), a width (Max of 50), and a bomb count (Max based on chosen width and height). 

After these selections are made the game board will show and a HUD containing the matching bomb and flag count will appear at the top of the screen. once the user clears their first tile the mines will be placed around the board and will avoid the first cleared tile. The neighbors will also be counted and then the game board is set to fully begin play. The user can navigate using WASD. Placing a flag removes one from the flag count at the top of the screen, and using the flag key or revealing a tile that already has a flag on it will increase the flag count displayed.

The game will be won once the player has flagged all the mines placed on the board and all other tiles have been revealed. If the player attempts to reveal a tile with a bomb on it then the game will be lost. In both of these cases a visual will appear on the screen over the game board with a corresponding losing or winning screen and the key to quit the game.

Variants:
Checkerboard - The tiles are colored in a checkerboard pattern and bombs on the lighter colored tiles are worth 2 when considering the neighbor count.
Liar - The neighbor counts are randomly larger or smaller than the true tile count by 1.