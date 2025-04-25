#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <vector>
#include <random>
#include <ctime>

class Obstacle {
private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    std::vector<SDL_Rect> obstacles;
    int* character_x_ptr;
    int* character_y_ptr;

    // Timers and intervals for separate top and bottom obstacle spawning
    int spawn_timer_top;
    int spawn_interval_top;
    int spawn_timer_bottom;
    int spawn_interval_bottom;


    std::mt19937 rng;
    std::uniform_int_distribution<int> height_dist;

    const int OBSTACLE_WIDTH = 150;
    const int OBSTACLE_HEIGHT = 150;
    const int WINDOW_WIDTH;
    const int WINDOW_HEIGHT;
    const int GROUND_LEVEL;

public:
    Obstacle(SDL_Renderer* renderer, int* char_x, int* char_y, int window_width, int window_height, int ground_level);
    ~Obstacle();

    void update(bool world_moving, int scroll_speed);
    void render();
    bool isColliding(int character_width, int character_height);
    void reset();
    // Modified spawnObstacle to take a parameter for side (top/bottom)
    void spawnObstacle(bool is_top);
};

#endif // OBSTACLE_H