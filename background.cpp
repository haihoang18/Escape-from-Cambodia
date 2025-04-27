#include "background.h"
#include "utils.h"
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <iostream>

using namespace std;

void moving_background(int speed, int& bg1_position_x, int& bg2_position_x, 
                      SDL_Renderer* renderer, SDL_Texture* background1, 
                      SDL_Texture* background2, const int WINDOW_WIDTH, 
                      const int WINDOW_HEIGHT) {

    // Reset positions when a background moves completely off-screen
    if (bg1_position_x <= -WINDOW_WIDTH) bg1_position_x = bg2_position_x + WINDOW_WIDTH;
    if (bg2_position_x <= -WINDOW_WIDTH) bg2_position_x = bg1_position_x + WINDOW_WIDTH;

    // Create rectangle for rendering
    SDL_Rect background_rect;
    background_rect.y = 0;
    background_rect.w = WINDOW_WIDTH;
    background_rect.h = WINDOW_HEIGHT;

    // Render first background
    background_rect.x = bg1_position_x;
    SDL_RenderCopy(renderer, background1, NULL, &background_rect);

    // Render second background
    background_rect.x = bg2_position_x;
    SDL_RenderCopy(renderer, background2, NULL, &background_rect);
}