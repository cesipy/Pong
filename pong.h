#ifndef PONG_H
#define PONG_H

#include <SDL.h>
#include <stdio.h>


#define WIDTH  1100
#define HEIGHT 850

enum state{ START, DRAW, PLAYER_WINS, AI_WINS };


// definitions of functions
void init_SDL();
void init();
void ball_bat_reset(int);

void prepare_scene(void);
void present_scene(void);
SDL_Texture* load_texture(char* path);
void render_texture(SDL_Texture* texture, int x, int y, int w, int h);
int check_input(int);

void move_bat(int up_or_down);
void move_bat_opponent();
void move_ball();
void collision(int);

typedef struct {
    SDL_Renderer* renderer;
    SDL_Window* window;
    int game_state;
} App;

typedef struct {
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

typedef struct {
    int position_x;
    int position_y;
    int height;
    int width;

    // texture
    SDL_Texture* texture;
} Bat;


#endif