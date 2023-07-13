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
SDL_Event event;

/*------------------------------------------------------------------------------*/

int main(int argc, char* argv[]) {
    // initialize the game
    init_SDL();

    int shutdown_flag = 0;
    int game_status = 0;


    while (shutdown_flag != 1) 
    {
        prepare_scene();

        shutdown_flag = check_input(shutdown_flag);

        render_texture(ball.texture, ball.position_x, ball.position_y);


        present_scene();

        SDL_Delay(16);
        
    }

    SDL_DestroyTexture(ball.texture);
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
    return 0;
}


/*------------------------------------------------------------------------------*/

/**
 * initializes bat and player, as well SDL
 */
void init(void)
{
    // initialize the game
    init_SDL();

    // implementation of ball
    ball.position_x = WIDTH  / 2;
    ball.position_y = HEIGHT / 2;

    ball.height = 1;
    ball.width = 1;

    ball.vector_x = 1;
    ball.vector_y = 1;

    ball.texture = load_texture("graphics/ball.bmp");

    // initialize bat
    bat[0].position_x = WIDTH - 20;
    bat[0].position_y = HEIGHT / 2;
    bat[0].height = 50;
    bat[0].width= 10;

}


/**
 * initialized SDL player. Window and renderer in app are defined.
*/
void init_SDL(void) 
{
    int status_init = SDL_Init(SDL_INIT_VIDEO);
    if (status_init < 0 ) 
    {
        fprintf(stderr, "Error occurred @ init SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    int window_flags = 0;
    int renderer_flags = 0;

    app.window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, window_flags);

    if (app.window == NULL) 
    {
        fprintf(stderr, "Error @ createWindow!\n");
        exit(EXIT_FAILURE);
    }

    app.renderer = SDL_CreateRenderer(app.window, -1, renderer_flags);

    if (app.renderer == NULL) 
    {
        fprintf(stderr, "Error @ rendererCreation\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * moves th bat ether up or down
 * up = 1 -> move bat up
 * up = 0 -> move bat down
*/
void move_bat(int up) 
{
    if (up) 
    {
        printf("Received w\n"); // Handle moving the bat up
    }

    if (up == 0) 
    {
        printf("Received s\n"); // Handle moving the bat down
    }   
}


/**
 * check keyboard for keys needed to control the game
 * - w   -> move the bat up
 * - s   -> move the bat down
 * - esc -> exit the game
*/
int check_input(int shutdown_flag) 
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
        return shutdown_flag;
}


/**
 * clear current render and prepare with color
 * color can be set using RGB (0-255)
*/
void prepare_scene(void)
{
	SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, 1);
	SDL_RenderClear(app.renderer);
}

/**
 * update screen with present render
*/
void present_scene(void)
{
	SDL_RenderPresent(app.renderer);
}


/**
 *  load the texture for the ball and the bat
 *
 */
SDL_Texture* load_texture(char* path) 
{
    SDL_Texture* texture = NULL;

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", path);

    SDL_Surface* surface = SDL_LoadBMP(path);
    if (surface == NULL) 
    {
        fprintf(stderr, "Unable to load image %s: %s\n", path, SDL_GetError());
        return NULL;
    }

    texture = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_FreeSurface(surface);

    if (texture == NULL) 
    {
        fprintf(stderr, "Unable to create texture from surface: %s\n", SDL_GetError());
    }

    return texture;
}


/**
 *
 */
void render_texture(SDL_Texture* texture, int x, int y)
{
    SDL_Rect dest_rect;

    dest_rect.x = x;
    dest_rect.y = y;

    SDL_QueryTexture(texture, NULL, NULL, &dest_rect.w, &dest_rect.h);

    SDL_RenderCopy(app.renderer, texture, NULL, &dest_rect);
}