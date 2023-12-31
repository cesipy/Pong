# Pong in C

This is an attempt to implement the classic game 'Pong'. Here, the SDL2 Library is used for the graphics.
The game allows a players to control his bat and compete against an AI, or you can watch an AI vs AI match.

![screenshot](graphics/showcase.png)

### Requirements

To run the program, you need to have SDL2, SDL2_ttf and SDL2_mixer libraries installed on your system.
SDL2 is for the graphical renderings, SDL2_ttf is for loading fonts. SDL2_mixer is used to play sounds.

### Installing SDL2, SDL_ttf and SDL_mixer (MacOS)

In order to run the program, you have to install SDL2 and SDL_ttf (for fonts). 
On MacOs I recommend installing SDL2 and SDL_ttf using `brew`.

Commands to install needed libs (on MacOs):

    brew install sdl2
    brew install sdl2_ttf
    brew install sdl2_mixer

Using the provided makefile it should compile on MacOs.

### Compiling and Running
To compile the program (on macOS), you can use the provided `Makefile` usning `make`.

To run the game in single player vs AI mode, simpy run:
    
    ./pong

To run the game in AI vs AI mode, simply type:

    ./pong  -ai

If you want to play the game with a friend, run_

    ./pong -1v1

### Controls
The game is controlled, using the 'w' and 's' keys. 'w' moves the bat up, while 's' move is down.

### Settings
In the source code, you can adjust the macros to modify the gameplay experience.
All the macros are explained in the code.

For example:
`INITIAL_VELOCITY` sets the initial speed of the ball (suggested range: 3 to 6)
