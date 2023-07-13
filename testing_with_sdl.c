/**
 * instant todo:
 *  - improve error handling
 *  - 0
*/

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "pong.h"

#define SENSITIVITY 30  // sensitivity of bat


// global vars, so all functions can read
App app;
Ball ball;
Bat bat[2];          // two bats for player and ai opponent
SDL_Event event;
int score[2];


/*------------------------------------------------------------------------------*/


int main(int argc, char* argv[]) {
    
    // initialize the game
    init();

    int shutdown_flag = 0;
    int game_status = 0;


    while (shutdown_flag != 1) 
    {
        prepare_scene();

        shutdown_flag = check_input(shutdown_flag);

        // draw ball
        render_texture(ball.texture, ball.position_x, ball.position_y, ball.width, ball.height);


        // draw my bat
        render_texture(bat[0].texture, bat[0].position_x, bat[0].position_y, bat[0].width, bat[0].height);

        // draw opponent bat
        render_texture(bat[1].texture, bat[1].position_x, bat[1].position_y, bat[1].width, bat[1].height);
        
        // draw for this iteration
        present_scene();



        // update ball by its vector
        move_ball();

        SDL_Delay(16);
        
    }
    printf("Score: %d : %d\n", score[0], score[1]);

    // clean up
    SDL_DestroyTexture(ball.texture);
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
    return 0;
}


/*------------------------------------------------------------------------------*/


/**
 * initializes bats and player, as well SDL
 */
void init(void)
{
    // initialize the game
    init_SDL();

    // implementation of ball
    ball.position_x = WIDTH  / 2;
    ball.position_y = HEIGHT / 2;

    ball.height = 25;
    ball.width = 25;

    ball.vector_x = 2;
    ball.vector_y = 0;

    ball.texture = load_texture("graphics/ball.bmp");


    // initialize bat
    bat[0].position_x = WIDTH - 20;
    bat[0].position_y = HEIGHT / 2 -50;
    bat[0].height = 100;
    bat[0].width= 15;

    bat->texture = load_texture("graphics/bat.bmp");


    // initialize opponent bat
    bat[1].position_x = 5;
    bat[1].position_y = HEIGHT / 2 - 50;
    bat[1].height = 100;
    bat[1].width = 15;

    bat[1].texture = load_texture("graphics/bat.bmp");

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
 * move the ball in the direction of the vector
*/
void move_ball(void) 
{
    ball.position_x += ball.vector_x;
    ball.position_y += ball.vector_y;

    collision(0);
    collision(1);

    // reverse ball direction if hits the top or bottom 
    if (ball.position_y <= 0 || ball.position_y + ball.height >= HEIGHT) 
    { 
        ball.vector_y = -ball.vector_y; 
    }


    // reverse ball direction if it hits the edges; only temporary; should reset and adjust score
    if (ball.position_x < 0) 
    { 
        printf("<0\n");
        score[0]++;
        ball.vector_x = -ball.vector_x; 
        ball.vector_y = -ball.vector_y; 
    }
    
    if (ball.position_x > WIDTH - ball.width) 
    { 
        score[1]++;
        ball.vector_x = -ball.vector_x; 
        ball.vector_y = -ball.vector_y; 
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
        bat[0].position_y -= SENSITIVITY;
    }

    if (up == 0) 
    {
        printf("Received s\n"); // Handle moving the bat down
        bat[0].position_y += SENSITIVITY;
    }   
}


/**
 * checks if ball collides with bat.
 * parameter player determines which bat to check.
 */
void collision(int player) 
{
    int ball_condition1 = ball.position_x + ball.width >= bat[player].position_x;
    int ball_condition2 = ball.position_x <= bat[player].position_x + bat[player].width;
    int ball_condition3 = ball.position_y + ball.height >= bat[player].position_y;
    int ball_condition4 = ball.position_y <= bat[player].position_y + bat[player].height;


    if (ball_condition1 && ball_condition2 && ball_condition3 && ball_condition4)
    {
        // collision occurred, reverse the ball's x direction
        ball.vector_x = -ball.vector_x;

        // update ball's y dirction

        // get bat center
        int bat_center_y = bat[player].position_y + bat[player].height / 2;

        // get ball center
        int ball_center_y = ball.position_y + ball.height / 2;

        // compute y value
        ball.vector_y = (ball_center_y - bat_center_y) / (SENSITIVITY * 2); // can be adjusted, controles velocity
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
	SDL_SetRenderDrawColor(app.renderer, 100, 0, 255, 255);
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
 * render the structure of ball / bat. 
 * takes all the coordinates and draws the object for the given frame based on its attributes
 */
void render_texture(SDL_Texture* texture, int x, int y, int width, int height)
{
    SDL_Rect dest_rect;

    dest_rect.x = x;
    dest_rect.y = y;
    dest_rect.w = width;
    dest_rect.h = height;

    //SDL_QueryTexture(texture, NULL, NULL, &dest_rect.w, &dest_rect.h);

    SDL_RenderCopy(app.renderer, texture, NULL, &dest_rect);
}