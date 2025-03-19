#include "utils.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

using namespace std;

void initializing()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        cout << "Could not init SDL: " << SDL_GetError() << endl;
        return;
    }
    if (!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG))
    {
        cout << "Could not init SDL_image: " << IMG_GetError() << endl;
        SDL_Quit();
        return;
    }
}

SDL_Texture* load_texture(SDL_Renderer* renderer, const char* image_file)
{
    SDL_Texture* texture = IMG_LoadTexture(renderer, image_file);
    if (texture == NULL)
    {
        cout << "Could not create texture: " << IMG_GetError() << endl;
        return NULL;
    }
    return texture;
}

SDL_Texture* load_texture_with_transparent_background(SDL_Renderer* renderer, const char* image_file)
{
    SDL_Surface* surface = IMG_Load(image_file);
    if (surface == NULL)
    {
        cout << "Could not load surface: " << SDL_GetError() << endl;
        return NULL;
    }
    
    // Make the white background transparent
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 255, 255));

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL)
    {
        cout << "Could not create texture from surface: " << SDL_GetError() << endl;
        SDL_FreeSurface(surface);
        return NULL;
    }
    
    // Free surface as it's no longer needed
    SDL_FreeSurface(surface);
    return texture;
}

void update_character_frame(Uint32 &last_frame_time, int &frame_num, const int CHARACTER_ANIMATION_DELAY)
{
    Uint32 current_time = SDL_GetTicks();
    if (current_time - last_frame_time > CHARACTER_ANIMATION_DELAY)
    {
        frame_num++;
        if (frame_num > 3) frame_num = 1;
        last_frame_time = current_time;
    }
}