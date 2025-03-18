#define SDL_MAIN_HANDLED

#include "utils.h"
#include "background.h"
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <iostream>

using namespace std;

// Function to handle background scrolling
void moving_background(int speed, int& bg1_position_x, int& bg2_position_x, 
                      SDL_Renderer* renderer, SDL_Texture* background1, 
                      SDL_Texture* background2, const int WINDOW_WIDTH, 
                      const int WINDOW_HEIGHT) {
    // Update background positions
    bg1_position_x -= speed;
    bg2_position_x -= speed;

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

// Optional: A standalone program to test the background scrolling
#ifdef TEST_BACKGROUND

const int WINDOW_WIDTH = 1400;
const int WINDOW_HEIGHT = 750;

int main(int argc, char* argv[])
{
    // Initialize SDL
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
    SDL_Window* window = SDL_CreateWindow("Background Test", SDL_WINDOWPOS_CENTERED, 
                            SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 
                            SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == NULL) {
        cout << "Could not create window: " << SDL_GetError() << endl;
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        cout << "Could not create renderer: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Load background textures
    SDL_Texture* background1 = load_texture(renderer, "background/background1.png");
    SDL_Texture* background2 = load_texture(renderer, "background/background2.png");

    if (!background1 || !background2) {
        cout << "Failed to load background textures!" << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    int bg1_position_x = 0;
    int bg2_position_x = WINDOW_WIDTH;

    bool run = true;
    SDL_Event e;
    while (run)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                run = false;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Move and render backgrounds
        moving_background(2, bg1_position_x, bg2_position_x, renderer, 
                         background1, background2, WINDOW_WIDTH, WINDOW_HEIGHT);

        // Present renderer
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
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
#endif