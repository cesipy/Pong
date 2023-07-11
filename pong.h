#ifndef PONG_H
#define PONG_H

#include <SDL.h>

#define WIDTH 640
#define HEIGHT 480


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
} Ball;

typedef struct {
    int position_x;
    int position_y;
    int height;
    int width;
} Bat;


#endif