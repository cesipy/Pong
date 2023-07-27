/**
 * instant todo:
 *  - improve error handling
*/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "pong.h"
#include <SDL2_ttf/SDL_ttf.h>


#define SENSITIVITY 30          // sensitivity of bat
#define INITIAL_VELOCITY 5      // velocity of ball (optimal 3 - 6)
#define AI_STRENGTH 10          // edit capabilities of ai. lower -> AI is easier to beat 
#define NUMBER_MIDDLE_LINES 8
/**
 * recommended values:
 * - sensitivity:      15 (slower and more difficult) to 45 (faster and easier), optimal 30
 * - initial_velocity: 3  (slower) to 6 (faster), optimal 4
 * - ai_strength:      3  (possible to beat) to 7 (impossible to beat), depends on velocity, opitmal 5
*/


// global vars, so all functions can read
App app;
Ball ball;
Bat bat[2];          // two bats for player and ai opponent
Middle_Line middle_line[NUMBER_MIDDLE_LINES];
SDL_Event event;

int score[2];        // scores for each player. score[0] = player, score[1] = ai


/*------------------------------------------------------------------------------*/


int main(int argc, char* argv[]) {

    if (argc == 2 && strcmp(argv[1], "-ai") == 0) 
    {
        printf("AI vs AI mode!\n");
        app.game_state = AI_VS_AI;
    } 
    else if (argc == 1) 
    {
        app.game_state = NORMAL;    // set to normal game state
    } 
    else 
    {
        fprintf(stderr, "Incorrect usage! To play AI vs AI, use: %s -ai\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // initialize the game
    init();

    int shutdown_flag = 0;

    // init sld_ttf for the fonts and score drawing
    TTF_Init();

    // open font for score drawing
    TTF_Font* sans = TTF_OpenFont("graphics/fonts/sans.ttf", 128);
    TTF_Font* fraunces = TTF_OpenFont("graphics/fonts/fraunces.ttf", 256);

    // Define target frame rate
    const int TARGET_FPS = 60;

    // Calculate the time per frame based on the target frame rate
    const Uint32 targetFrameTime = 1000 / TARGET_FPS;  // in milliseconds

    while (shutdown_flag != 1) 
    {
        Uint32 frameStartTime = SDL_GetTicks();

        prepare_scene();

        shutdown_flag = check_input(shutdown_flag); // checks input and processes controls for my bat

        if (app.game_state == AI_VS_AI) { move_bat_aivsai(); }  // ai vs ai

        // move AI's bat
        move_bat_opponent();


        // draw the middle line
        draw_middle_line();

        // draw ball
        render_texture(ball.texture, ball.position_x, ball.position_y, ball.width, ball.height);

        // draw my bat
        render_texture(bat[0].texture, bat[0].position_x, bat[0].position_y, bat[0].width, bat[0].height);

        // draw opponent bat
        render_texture(bat[1].texture, bat[1].position_x, bat[1].position_y, bat[1].width, bat[1].height);

        //draw_score(sans);
        draw_score(fraunces);
        
        // draw for this iteration
        present_scene();


        // update ball by its vector
        move_ball();

        Uint32 frameEndTime = SDL_GetTicks();
        Uint32 frameTime = frameEndTime - frameStartTime;

        // If the frame was rendered faster than the target frame time, introduce a delay
        if (frameTime < targetFrameTime) {
            Uint32 delayTime = targetFrameTime - frameTime;
            SDL_Delay(delayTime);
        }
    }
    printf("Score: player - AI\n\t    %d - %d\n", score[0], score[1]);

    // clean up
    //TTF_CloseFont(sans);
    TTF_CloseFont(fraunces);
    TTF_Quit();
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

    // initialize ball's and bat' position to default.
    // initialize ball's direction vector
    ball_bat_reset(1);

    ball.height = 25;
    ball.width = 25;

    ball.texture = load_texture("graphics/ball.bmp");


    // initialize bat
    bat[0].height = 100;
    bat[0].width= 15;

    bat->texture = load_texture("graphics/bat.bmp");


    // initialize opponent bat
    bat[1].height = 100;
    bat[1].width = 15;

    bat[1].texture = load_texture("graphics/bat.bmp");


    // initialize middle line
    int incr = (HEIGHT / NUMBER_MIDDLE_LINES);
    int middle = WIDTH / 2 - 5;                     // -5 because width of rectangle is 10
    int counter = 0;

    for (int i = 25; i < HEIGHT; i += incr)
    {
        middle_line[counter].position_x = middle;
        middle_line[counter].position_y = i;
        middle_line[counter].height = 120;
        middle_line[counter].width = 120;

        middle_line[counter].texture = load_texture("graphics/rect.bmp");

        counter++;
    }

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
 * 
 * sensitivity of new direction after collision is too high. 
 * this feature has to be further improved
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
        score[0]++;
        ball_bat_reset(0);

    }
    
    if (ball.position_x > WIDTH - ball.width) 
    { 
        score[1]++;
        ball_bat_reset(0);
    }
}



/**
 * moves th bat ether up or down
 * up = 1 -> move bat up
 * up = 0 -> move bat down
*/
void move_bat(int up) 
{
    int current_bat_position = bat[0].position_y + bat[0].height / 2;
    if (up== 0 && current_bat_position < HEIGHT) 
   {

        // Handle moving the bat up
        bat[0].position_y += SENSITIVITY;
    }

    if (up && current_bat_position > 0) 
    {

        // Handle moving the bat down
        bat[0].position_y -= SENSITIVITY;
    }   
}


/**
 * ai moves the opponent's bat.
 * strength of AI can be adjusted by setting macro AI_STRENGTH
 *  AI_STRENGTH --- (continue!)
 */
void move_bat_opponent(void)
{
    int middle = HEIGHT / 2 - 50;

    // if ball moves towards player (not ai)
    if (ball.vector_x > 0)
    {

        if (bat[1].position_y < middle)
        {

            bat[1].position_y += AI_STRENGTH;
        }

        else if (bat[1].position_y > middle)
        {

            bat[1].position_y -= AI_STRENGTH;
        }
    }
    // ball moves towards ai's bat
    else if (ball.vector_x < 0)
    {

        // calculate the predicted intersection point of the ball and the AI's bat
        int predicted_intersection = ball.position_y + 
                (bat[1].position_x - ball.position_x) * ball.vector_y / ball.vector_x;

        int current_bat_position   = bat[1].position_y + bat[1].height / 2;

        if (predicted_intersection > current_bat_position 
            && predicted_intersection < current_bat_position + 30)
        { }
        
        else if (predicted_intersection < current_bat_position 
            && current_bat_position > 0)
        {

            bat[1].position_y -= AI_STRENGTH;
        }

        else if (predicted_intersection > current_bat_position && current_bat_position < HEIGHT)
        {

            bat[1].position_y += AI_STRENGTH;
        }
    }
}


/**
 * resets ball and bat to default position.
 * if flag is set, the ball's direction vector is set.
 * if flag is not set, the ball's direction vector is reversed.
*/
void ball_bat_reset(int first_init_flag)
{
    if (first_init_flag) { ball.vector_x = INITIAL_VELOCITY; }
    else                 { ball.vector_x = -ball.vector_x;  }   
    
    ball.vector_y = 0;

    ball.position_x = WIDTH  / 2;
    ball.position_y = HEIGHT / 2;

    bat[0].position_x = WIDTH - 30;
    bat[0].position_y = HEIGHT / 2 - 50;

    bat[1].position_x = 5;
    bat[1].position_y = HEIGHT / 2 - 50;
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


        int bat_center_y = bat[player].position_y + bat[player].height / 2;

        int ball_center_y = ball.position_y + ball.height / 2;

        // compute y value
        ball.vector_y = (ball_center_y - bat_center_y);

        /* 
        adjust_velocity is used to control the velocity after a collision occurred.
        smaller number means 
        */
        int adjusting_velocity = 8;
        ball.vector_y = ball.vector_y / adjusting_velocity;
    }
}



/**
 * check keyboard for keys needed to control the game.
 * if control keys (w / s) are typed, call move_bat function
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


void move_bat_aivsai() 
{
    int middle = HEIGHT / 2 - 50;

    if (ball.vector_x < 0)
    {

        if (bat[0].position_y < middle) 
        {

            bat[0].position_y += AI_STRENGTH;
        }

        else if (bat[0].position_y > middle)
        {

            bat[0].position_y -= AI_STRENGTH;
        }
    }
    // ball moves toward right ai
    else if (ball.vector_x > 0)
    {

        int predicted_intersection = ball.position_y +
            (bat[0].position_x - ball.position_x) * ball.vector_y / ball.vector_x;

        int current_bat_position   = bat[0].position_y + bat[0].height / 2;

        if (predicted_intersection > current_bat_position 
            && predicted_intersection < current_bat_position + 30)
        { }
        
        else if (predicted_intersection < current_bat_position 
            && current_bat_position > 0)
        {

            bat[0].position_y -= AI_STRENGTH;
        }

        else if (predicted_intersection > current_bat_position && current_bat_position < HEIGHT)
        {

            bat[0].position_y += AI_STRENGTH;
        }
    }
}


//SDL_Texture* 
void draw_score(TTF_Font* font)
{
    SDL_Color white = {10, 10, 10};

    // create score message
    char score_message[50];
    snprintf(score_message, sizeof(score_message), "%d - %d", score[1], score[0]);

    // render to surface
    SDL_Surface* surfaceMessage =
        TTF_RenderText_Solid(font, score_message, white); 

    SDL_Texture* message = SDL_CreateTextureFromSurface(app.renderer, surfaceMessage);

    SDL_Rect rect;
    rect.x = WIDTH  / 2 - 30;
    rect.y = 50;
    rect.h = 60;
    rect.w = 60;

    SDL_RenderCopy(app.renderer, message, NULL, &rect);
    SDL_DestroyTexture(message);
}


void draw_middle_line() 
{
    for(int i=0; i < NUMBER_MIDDLE_LINES; i++)
    {
        render_texture(middle_line[i].texture, middle_line[i].position_x, middle_line[i].position_y, 
            middle_line[i].width, middle_line[i].height);
    }
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
 * load the texture for the ball and the bat.
 * 
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
    //dest_rect.w = width;
    //dest_rect.h = height;

    // use this to fetch size of .bmps
    SDL_QueryTexture(texture, NULL, NULL, &dest_rect.w, &dest_rect.h);

    SDL_RenderCopy(app.renderer, texture, NULL, &dest_rect);
}