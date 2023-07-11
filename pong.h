#ifndef PONG_H
#define PONG_H

#include <SDL.h>

#define WIDTH 1000
#define HEIGHT 1000


typedef struct {
    SDL_Renderer* renderer;
    SDL_Window* window;
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
} Bat;


#endif