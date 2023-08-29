#ifndef PONG_H
#define PONG_H

#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL_mixer.h>


#define SENSITIVITY 35          // sensitivity of bat
#define INITIAL_VELOCITY 8      // velocity of ball (optimal 3 - 6)
#define AI_STRENGTH  2          // edit capabilities of ai. lower -> AI is easier to beat
#define NUMBER_MIDDLE_LINES 9
#define ADJUST_RANDOMNESS  1    // higher -> less randomness
/**
 * recommended values:
 * - sensitivity:      15 (slower and more difficult) to 45 (faster and easier), optimal 30
 * - initial_velocity: 3  (slower) to 6 (faster), optimal 4
 * - ai_strength:      3  (possible to beat) to 7 (impossible to beat), depends on velocity, opitmal 5
*/

#define WIDTH  1100
#define HEIGHT 850

/*------------------------------------------------------------------------------*/

enum state{ NORMAL, DRAW, PLAYER_WINS, AI_WINS, AI_VS_AI, PLAYER_VS_PLAYER };


typedef struct 
{
    SDL_Renderer* renderer;
    SDL_Window* window;
    int game_state;
} App;

typedef struct 
{
    int position_x;
    int position_y;
    int height;
    int width;
    
    // moving vector
    int vector_x;
    int vector_y;

    // texture
    SDL_Texture* texture;
} Ball;

typedef struct 
{
    int position_x;
    int position_y;
    int height;
    int width;

    // texture
    SDL_Texture* texture;
} Bat;


typedef struct 
{
int position_x;
int position_y;
int height;
int width;

// texture
SDL_Texture* texture;
} Middle_Line;

/*------------------------------------------------------------------------------*/

// definitions of functions

/**
 * initialized SDL player. Window and renderer in app are defined.
*/
void init_SDL();

/**
 * initializes bats and player, as well SDL
 */
void init();
void ball_bat_reset(int);

void prepare_scene(void);
void present_scene(void);
SDL_Texture* load_texture(char* path);

/**
 * render the structure of ball / bat.
 *
 * takes all the coordinates and draws the object for
 * the given frame based on its attributes
 * @param SDL_Texture* texture  - loaded texture
 * @param int x                 - x position of object
 * @param int y                 - y position of object
 * @param int width             - width of object
 * @param height                - height of object
 */
void render_texture(SDL_Texture* texture, int x, int y, int w, int h);
int check_input(int);
void draw_score(TTF_Font* font);
void draw_middle_line(void);

void move_bat(int up_or_down);
void move_bat_opponent();
void move_bat_second_player(int);

/**
 * move the ball in the direction of the vector
 *
 * sensitivity of new direction after collision is too high.
 * this feature has to be further improved
*/
void move_ball();
void collision(int);

void move_bat_aivsai(void);

/**
 * function to handle errors.
 * It will print the error message along with the system error message using perror.
 * It will also exit the program with a failure status.
 * @param int message - error message to be print
 */
void err(const char*);

/**
 * generates a random number in the range from 'min' to 'max'.
 * @param int min
 * @param int max
 * @return int - random number
 */
int random_num(int, int);

/**
 * load sound media for the game.
 *
 * this function loads sound files into memory using the SDL_mixer library. It
 * loads various sound effects that are used during gameplay, such as collision
 * sounds between the ball and bats or walls.
 *
 */
void load_media(void);

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
void play_sound(int);

#endif