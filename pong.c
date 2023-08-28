#include "pong.h"

App app;
Ball ball;
Bat bat[2];          // two bats for player and ai opponent
Middle_Line middle_line[NUMBER_MIDDLE_LINES];
SDL_Event event;

Mix_Chunk* sound_left1 = NULL;
Mix_Chunk* sound_left2 = NULL;
Mix_Chunk* sound_right1 = NULL;
Mix_Chunk* sound_right2 = NULL;
Mix_Chunk* sound_middle_wall1 = NULL;
Mix_Chunk* sound_middle_wall2 = NULL;
Mix_Chunk* sound_score_increase = NULL;

int score[2];        // scores for each player. score[0] = player, score[1] = ai


/*------------------------------------------------------------------------------*/


int main(int argc, char* argv[]) {

    if (argc == 2 && strcmp(argv[1], "-ai") == 0) 
    {
        printf("AI vs AI mode!\n");
        app.game_state = AI_VS_AI;
    } 
    
    else if (argc == 2 && strcmp(argv[1], "-1v1") == 0)
    {
        printf("Player vs Player mode! (1-vs-1)\n");
        app.game_state = PLAYER_VS_PLAYER;
    }

    else if (argc == 1) 
    {
        app.game_state = NORMAL;    
    } 

    else 
    {
        fprintf(stderr, "Incorrect usage! To play AI vs AI, use: %s -ai\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // initialize the SDL, position of objects
    init();

    int shutdown_flag = 0;

    // init sld_ttf for the fonts and score drawing
    if (TTF_Init() != 0) 
    {
        err("TTF_Init");
        SDL_DestroyRenderer(app.renderer);
        SDL_DestroyWindow(app.window);
        SDL_Quit();
    }

    // open font for score drawing
    TTF_Font* sans = TTF_OpenFont("graphics/fonts/sans.ttf", 128);
    TTF_Font* fraunces = TTF_OpenFont("graphics/fonts/fraunces.ttf", 256);
    if (fraunces == NULL) 
    {
        err("TTF_OpenFont fraunces");
        SDL_DestroyRenderer(app.renderer);
        SDL_DestroyWindow(app.window);
        TTF_Quit();
        SDL_Quit();
    }

    Uint32 next_game_tick = SDL_GetTicks();

    while (shutdown_flag != 1) 
    {
        Uint32 frameStartTime = SDL_GetTicks();

        prepare_scene();

        shutdown_flag = check_input(shutdown_flag); // checks input and processes controls for my bat

        if (app.game_state == AI_VS_AI) { move_bat_aivsai(); }  // ai vs ai

        if (app.game_state == PLAYER_VS_PLAYER) 
        { }

        else 
        {
            // move AI's bat
            move_bat_opponent();
        }


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

        next_game_tick += (1000 / 60);
        int sleep = next_game_tick - SDL_GetTicks();

        if (sleep >= 0)
        {

            SDL_Delay(sleep);
        }
    }

    // clean up
    Mix_CloseAudio();
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

        // load sound effect
        load_media();
    }
}


/**
 * initialized SDL player. Window and renderer in app are defined.
*/
void init_SDL(void) 
{
    int status_init = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
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

    //Initialize SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
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
        play_sound(3);
    }

    // reverse ball direction if it hits the edges; only temporary; should reset and adjust score
    if (ball.position_x < 0) 
    { 
        score[0]++;
        ball_bat_reset(0);
        Mix_PlayChannel(-1, sound_score_increase, 0);
    }
    
    if (ball.position_x > WIDTH - ball.width) 
    { 
        score[1]++;
        ball_bat_reset(0);
        Mix_PlayChannel(-1, sound_score_increase, 0);
    }
}


/**
 * moves th bat ether up or down
 * up = 1 -> move bat up
 * up = 0 -> move bat down
 *
 * @param int up - indicated direction.
 * 1 -> move up, 2-> move down
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

    // if ball moves towards player (not AI), move bat to middle
    if (ball.vector_x > 0)
    {
        if (bat[1].position_y > middle - 20 && bat[1].position_y < middle + 20)
        { }

        else if (bat[1].position_y < middle)
        {
            bat[1].position_y += AI_STRENGTH;
        }

        else if (bat[1].position_y > middle)
        {
            bat[1].position_y -= AI_STRENGTH;
        }
    }

    // ball moves towards AI's bat
    else if (ball.vector_x < 0)
    {
        // calculate the predicted intersection point of the ball and the AI's bat
        int predicted_intersection = ball.position_y + 
                (bat[1].position_x - ball.position_x) * ball.vector_y / ball.vector_x;

        int current_bat_position   = bat[1].position_y + bat[1].height / 2;

        // generate random number
        int random_number = random_num(0, AI_STRENGTH / ADJUST_RANDOMNESS); 

        int random_number2 = random_num(1, 10);
        
        //calculate the increment 
        int incr_decr = random_number2 % 2 == 0 ? random_number : -random_number;

        if (predicted_intersection > current_bat_position - 40 
            && predicted_intersection < current_bat_position + 40)
        { }
        
        else if (predicted_intersection < current_bat_position + 40  
            && current_bat_position > 0)
        {
            int decr = AI_STRENGTH + incr_decr;
            bat[1].position_y -= decr;
        }

        else if (predicted_intersection > current_bat_position 
            && current_bat_position < HEIGHT)
        {
            int incr = AI_STRENGTH + incr_decr;
            bat[1].position_y += AI_STRENGTH;
        }
    }
}


/**
 * move the second player's bat up or down based on the provided input.
 *
 * This function updates the position of the second player's bat based on the
 * provided input flag. The bat can be moved upwards or downwards by a certain
 * sensitivity value.
 *
 * @param int up - Indicates the direction of movement.
 *                 - When set to 0, the bat will move downwards.
 *                 - When set to non-zero, the bat will move upwards.
 *
 * @note The function is used if you run the game using the flag '-1v1'
 *
 * @see SENSITIVITY - The sensitivity value that determines the rate of bat movement.
 * @see bat - The array of bat structures representing the players' bats.
 * @see HEIGHT - The height of the game window.
 */
void move_bat_second_player(int up)
{
    int current_bat_position = bat[1].position_y + bat[1].height / 2;
    if (up== 0 && current_bat_position < HEIGHT) 
    {

        // Handle moving the bat up
        bat[1].position_y += SENSITIVITY;
    }

    if (up && current_bat_position > 0) 
    {

        // Handle moving the bat down
        bat[1].position_y -= SENSITIVITY;
    }   
}

/**
 * resets ball and bat to default position.
 * if flag is set, the ball's direction vector is set.
 * if flag is not set, the ball's direction vector is reversed.
 *
 * @param int first_init_flag - is this the first iteration?
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
 *
 * @param int player - indicates if player (0) or AI (1)
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

        // update ball's y direction

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

        play_sound(player);
    }
}


/**
 * check keyboard for keys needed to control the game.
 * if control keys (w / s) are typed, call move_bat function
 * - w   -> move the bat up
 * - s   -> move the bat down
 * - esc -> exit the game
 *
 * @param int shutdown_flag updated given shutdown_flag
 * @return shutdown_flag
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

                case SDLK_o:
                    if (app.game_state == PLAYER_VS_PLAYER)
                    {
                        move_bat_second_player(1); 
                    }
                    // move other bat up
                    break;

                case SDLK_l:
                    if (app.game_state == PLAYER_VS_PLAYER)
                    {
                        move_bat_second_player(0);
                    }
                    // move other bat down
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
 * in the AI vs AI mode the player's bat has to be controlled using AI. 
 * this function handles this case.
*/
void move_bat_aivsai() 
{
    int middle = HEIGHT / 2 - 50;

    if (ball.vector_x < 0)
    {
        if (bat[0].position_y > middle - 20 && bat[0].position_y < middle + 20)
        { }

        else if (bat[0].position_y < middle) 
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

        // generate random number
        int random_number = random_num(0, AI_STRENGTH / ADJUST_RANDOMNESS); 
        
        //calculate the increment 
        int incr_decr = random_number % 2 == 0 ? random_number : -random_number;

        if (predicted_intersection > current_bat_position - 40 
            && predicted_intersection < current_bat_position + 40)
        { }
        
        
        else if (predicted_intersection < current_bat_position 
            && current_bat_position > 0)
        {
            int decr = AI_STRENGTH + incr_decr;
            bat[0].position_y -= decr;
        }

        else if (predicted_intersection > current_bat_position && current_bat_position < HEIGHT)
        {
            int incr = AI_STRENGTH + incr_decr;
            bat[0].position_y += AI_STRENGTH;
        }
    }
}


/**
 * draws the score of the game on the board.
 * takes a TTF_Font as input - specifies font used for the score.
*/
void draw_score(TTF_Font* font)
{
    SDL_Color white = {240, 240, 240};

    // create score message
    char score_message[50];
    snprintf(score_message, sizeof(score_message), "%d - %d", score[1], score[0]);

    TTF_SetFontHinting(font, TTF_HINTING_NORMAL);

    // render to surface
    SDL_Surface* surfaceMessage =
        TTF_RenderText_Blended(font, score_message, white);  // Use TTF_RenderText_Blended


    SDL_Texture* message = SDL_CreateTextureFromSurface(app.renderer, surfaceMessage);

    SDL_Rect rect;
    rect.h = 140;
    rect.w = 140;
    rect.x = WIDTH  / 2 - rect.w /2;
    rect.y = 15;

    SDL_RenderCopy(app.renderer, message, NULL, &rect);
    SDL_DestroyTexture(message);
    SDL_FreeSurface(surfaceMessage);
}


/**
 * draws the middle line on the screen.
*/
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
	if (SDL_SetRenderDrawColor(app.renderer, 100, 0, 255, 255) != 0) // set color
    {
        err("SDL_SetRenderDrawColor");
    }
	if (SDL_RenderClear(app.renderer) != 0) 
    {
        err("SDL_RenderClear");
    }
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
 * @param char* path - path of .bmp to load
 * @return loaded texture
 */
SDL_Texture* load_texture(char* path) 
{
    SDL_Texture* texture = NULL;

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
 * * render the structure of ball / bat.
 * takes all the coordinates and draws the object for
 * the given frame based on its attributes
 * @param SDL_Texture* texture  - loaded texture
 * @param int x                 - x position of object
 * @param int y                 - y position of object
 * @param int width             - width of object
 * @param height                - height of object
 */
void render_texture(SDL_Texture* texture, int x, int y, int width, int height)
{
    SDL_Rect dest_rect;

    dest_rect.x = x;
    dest_rect.y = y;
    //dest_rect.w = width;
    //dest_rect.h = height;

    // use this to fetch size of .bmp
    if (SDL_QueryTexture(texture, NULL, NULL, &dest_rect.w, &dest_rect.h) != 0) 
    {
        err("SDL_QueryTexture");
    }

    if (SDL_RenderCopy(app.renderer, texture, NULL, &dest_rect) != 0)
    {
        err("SDL_RenderCopy");
    }
}


/**
 * load sound media for the game.
 *
 * this function loads sound files into memory using the SDL_mixer library. It
 * loads various sound effects that are used during gameplay, such as collision
 * sounds between the ball and bats or walls.
 *.
 */
void load_media(void) 
{
    sound_left1 = Mix_LoadWAV( "graphics/sounds/pong_left_1.wav");
    if (sound_left1 == NULL)
    {
        err("load_media");
    }

    sound_left2 = Mix_LoadWAV( "graphics/sounds/pong_left_2.wav");
    if (sound_left1 == NULL)
    {
        err("load_media");
    }

    sound_right1 = Mix_LoadWAV( "graphics/sounds/pong_right_1.wav");
    if (sound_left1 == NULL)
    {
        err("load_media");
    }

    sound_right2 = Mix_LoadWAV( "graphics/sounds/pong_right_2.wav");
    if (sound_left1 == NULL)
    {
        err("load_media");
    }

    sound_middle_wall1 = Mix_LoadWAV( "graphics/sounds/pong_middle_wall_1.wav");
    if (sound_middle_wall1 == NULL)
    {
        err("load_media");
    }

    sound_middle_wall2 = Mix_LoadWAV( "graphics/sounds/pong_middle_wall_2.wav");
    if (sound_middle_wall2 == NULL)
    {
        err("load_media");
    }

    sound_score_increase = Mix_LoadWAV("graphics/sounds/score_increase.wav");
    if ( sound_score_increase == NULL)
    {
        err("load_media");
    }
}


/**
 * play sound effects based on the game events.
 *
 * this function plays different sound effects based on the context provided by
 * the 'side' parameter. It uses the SDL_mixer library to play audio files for
 * collision events between the ball and bats or walls.
 *
 * @param int side - Specifies the context of the sound event:
 *                   - 0: Ball collided with player's bat.
 *                   - 1: Ball collided with opponent's bat.
 *                   - Other values: Ball collided with top/bottom wall.
 */
void play_sound(int side) 
{
    int random_number_collision_sound = random_num(0, 1);
    if (side == 0)  // player
    {
        if (random_number_collision_sound == 0)
        {
            Mix_PlayChannel(-1, sound_right1, 0);
        }
        else
        {
            Mix_PlayChannel(-1, sound_right2, 0);
        }
    }
    
    else if (side == 1)
    {
        if (random_number_collision_sound == 0)
        {
            Mix_PlayChannel(-1, sound_left1, 0);
        }
        else 
        {
            Mix_PlayChannel(-1, sound_left2, 0);
        }
    }
    // ball hits the bottom/top wall
    else 
    {
        if (random_number_collision_sound == 0)
        {
            Mix_PlayChannel(-1, sound_middle_wall1, 0);
        }
        else 
        {
            Mix_PlayChannel(-1, sound_middle_wall2, 0);
        }
    }
}


// generate error messages
void err(const char* message) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "Error @ %s ", message);
    perror(buffer);
    exit(EXIT_FAILURE);
}

// generate random number
int random_num(int min, int max) {
    return min + rand() % (max - min + 1);
}
