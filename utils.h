#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

void initializing();

SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path);

SDL_Texture* load_texture_with_transparent_background(SDL_Renderer* renderer, const char* path); 

void update_character_frame(Uint32 &last_frame_time, int &frame_num, const int CHARACTER_ANIMATION_DELAY);
#endif
