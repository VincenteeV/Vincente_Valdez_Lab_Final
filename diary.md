4/27: Started writing code after watching some videos and reading documentation on notcurses. Started board configuration code and will test if it works before moving on to user input and dynamic sizing. Also planned out how the minesweeper game mechanics will work on paper.

4/29: Got main board looking good and will start to create user input and a start menu for the user to choose game type and size, still getting used to notcurses and haven't implemented any gameplay yet.(70ish Minutes)

4/30: Created a main menu screen using notcurses so that the user can select 1 of 3 game style choices (not implemented yet). and 4 game sizes. The user selects with the arrow keys and enter and then the minesweeper board is made.(2 Hours)

5/2: Created custom game board logic into a seperate screen when user selects custom size. Player enters number of rows, columns, and mines that they want. Also fixed a wierd highligting issue in the main menu that caused the size label to be highlighted when on the lowest gamemode selection. Beginning to start actual gameplay. Created a placeMines function that will activate after the player picks their first tile so that they never start on a bomb. Also planning to commit for like the second time because I've been forgetting to due to just working on my pc because my laptop keyboard is still broken and I figure assignment will look better with more commits.(2.5 Hours)

5/4: Created logic for the movement of the "cursor" around the board, which I put into a function. But there were like 7 or 8 different parameters so I made a gameState struct that contained info about the board and the cursor which ended up being very useful and helped clean up some other functions. Also created a countNeighbors function that numbers out all of the tiles that aren't bombs. Then I created the clearTile function that reveals a block of tiles at a time. Implemented all of this along with the placeMines function so that the game is pretty much playable except there are no consequences for landing on bombs, thats planned soon but good progress. Also realized I haven't been adding the time I've spent on these logs so I estimated the times for previous entries.(4 Hours Early Today)

Created mine and flag count and disallowed user to place more flags after they hit the number of mines on the map. Also started cleaning up the non bomb tiles by making any with no nearby bombs blank instead of 0's. And the flags can no longer be placed onto revealed tiles. Finished making different colors for all of the numbered cells. Added YOU LOST screen, still need to get win condition and winning screen and remove game board beforehand.(4 Hours Late Today)

5/5: Made a screen for YOU WON and created a black border around the ending game message so that it looks cleaner. Also finished variants today with both complete and fully working. Had to change cursor color to accomodate the checkerboard color scheme but it all looks good now and works. Had to change my mine counting logic and add a display_count in addition to my already existing neighbor_count which helped with the liar logic. Last thing to do is finishing touches on the end game screen, may add replay option, may not because I'm lazy but I'll think about it tommorow.(Worked throughout day, ~4 Hours total on actual project)

5/6:Fixed some edge cases involving controls and flags and added non-doxygen comments to main c file. (1.5 Hours)

5/8: Started doxygen comments, got about half way through. Also completely wrote the reflections markdown file. (2 Hours)

5/10: Finished doxygen commenting and wrote the readme file. Also did final git commit.

https://github.com/VincenteeV/Vincente_Valdez_Lab_Final