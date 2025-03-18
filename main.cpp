#define SDL_MAIN_HANDLED

#include "utils.h"
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <iostream>

using namespace std;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* background1;
SDL_Texture* background2;

const int WINDOW_WIDTH = 1400;
const int WINDOW_HEIGHT = 750;

int main(int argc, char* argv[])
{
    // Initialize SDL first
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return -1;
    }
    
    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cout << "Could not initialize SDL_image! SDL_Error: " << IMG_GetError() << endl;
        SDL_Quit();
        return -1;
    }

    // Create window
    window = SDL_CreateWindow("SDL_Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                             WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == NULL) {
        cout << "Could not create window: " << SDL_GetError() << endl;
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        cout << "Could not create renderer: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Load textures
    background1 = load_texture(renderer, "background/background1.png");
    background2 = load_texture(renderer, "background/background2.png");
    
    if (!background1 || !background2) {
        cout << "Failed to load background textures!" << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }
    
    // Query texture dimensions for debugging
    int img_width, img_height;
    SDL_QueryTexture(background1, NULL, NULL, &img_width, &img_height);
    cout << "Background1 size: " << img_width << "x" << img_height << endl;

    SDL_QueryTexture(background2, NULL, NULL, &img_width, &img_height);
    cout << "Background2 size: " << img_width << "x" << img_height << endl;

    int bg1_position_x = 0;
    int bg2_position_x = WINDOW_WIDTH;

    SDL_Rect background_rect;
    background_rect.y = 0;
    background_rect.w = WINDOW_WIDTH;
    background_rect.h = WINDOW_HEIGHT;

    bool run = true;
    SDL_Event e;
    while (run) {
        // Handle events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                run = false;
            }
        }

        // Update background positions
        bg1_position_x -= 2;
        bg2_position_x -= 2;

        if (bg1_position_x <= -WINDOW_WIDTH) bg1_position_x = bg2_position_x + WINDOW_WIDTH;
        if (bg2_position_x <= -WINDOW_WIDTH) bg2_position_x = bg1_position_x + WINDOW_WIDTH;

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render background 1
        background_rect.x = bg1_position_x;
        SDL_RenderCopy(renderer, background1, NULL, &background_rect);
        
        // Render background 2
        background_rect.x = bg2_position_x;
        SDL_RenderCopy(renderer, background2, NULL, &background_rect);

        // Present renderer
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    // Clean up resources
    SDL_DestroyTexture(background1);
    SDL_DestroyTexture(background2);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}