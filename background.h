#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

void moving_background(int speed, int& bg1_position_x, int& bg2_position_x, 
                      SDL_Renderer* renderer, SDL_Texture* background1, 
                      SDL_Texture* background2, const int WINDOW_WIDTH, 
                      const int WINDOW_HEIGHT);

#endif // BACKGROUND_H