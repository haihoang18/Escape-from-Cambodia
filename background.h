#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

/**
 * Move and render scrolling backgrounds
 * 
 * @param speed         Scrolling speed in pixels per frame
 * @param bg1_position_x Reference to the first background's x position
 * @param bg2_position_x Reference to the second background's x position
 * @param renderer      SDL renderer to draw with
 * @param background1   First background texture
 * @param background2   Second background texture
 * @param WINDOW_WIDTH  Window width
 * @param WINDOW_HEIGHT Window height
 */
void moving_background(int speed, int& bg1_position_x, int& bg2_position_x, 
                      SDL_Renderer* renderer, SDL_Texture* background1, 
                      SDL_Texture* background2, const int WINDOW_WIDTH, 
                      const int WINDOW_HEIGHT);

#endif // BACKGROUND_H