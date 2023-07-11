/**
 * instant todo:
 *  - improve error handling
*/

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "pong.h"

// global vars, so all functions can read
App app;
Ball ball;
Bat bat[2];          // two bats for player and ai opponent

// definitions of functions
void init_SDL();
void draw_game();
void move_bat(int up_or_down);
void move_bat_opponent();


int main(int argc, char* argv[]) {
    // initialize the game
    init_SDL();

    int shutdown_flag = 0;
    int game_status = 0;

    SDL_Event event;
    while (shutdown_flag != 1) 
    {

        //check for esc for quiting
        while ( SDL_PollEvent(&event) != 0)
        {
            // check if wanted to quit
            if (event.type == SDL_QUIT)
            {
                shutdown_flag = 1;
            }

            //check the input
            else if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_w:
                    // w is pressed
                    move_bat(1);
                    break;
                
                case SDLK_s:
                    // s is pressed
                    move_bat(0);
                    break;

                case SDLK_ESCAPE:
                    // esc is pressed
                    shutdown_flag = 1;
                    break;
                
                default:
                    break;
                }

            }
        }
    }

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
    return 0;
}


/*------------------------------------------------------------------------------*/

/**
 * initialized SDL player. Window and renderer in app are defined.
*/
void init_SDL(void) 
{
    int status_init = SDL_Init(SDL_INIT_VIDEO);
    if (status_init < 0 ) 

    {
        fprintf(stderr, "Error occurred @ init SDL :%s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    int window_flags = 0;
    int renderer_flags = 0;

    app.window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, window_flags);

    if (app.window == NULL) 
    {
        fprintf(stderr, "Error @ createWindow!\n");
        exit(1);
    }


    app.renderer = SDL_CreateRenderer(app.window, -1, renderer_flags);

    if (app.renderer == NULL) 
    {
        fprintf(stderr, "Error @ rendererCreation\n");
        exit(1);
    }
}

/**
 * moves th bat eather up or down
 * up = 1 -> move bat up
 * up = 0 -> move bat down
*/
void move_bat(int up) 
{

    if (up) 
    // handle move bat up
    {
        // testing functionality so far:
        printf("received w\n");
    }

    if (up == 0) 
    // handle move bat down
    {
        // testing 
        printf("received s\n");
    }
    
}