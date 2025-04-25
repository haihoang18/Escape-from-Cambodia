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
    
    // Thêm biến quản lý tốc độ và thời gian tăng tốc
    float obstacle_speed_multiplier;
    int speed_increase_timer;
    const int SPEED_INCREASE_INTERVAL = 600; // Tăng tốc mỗi 10 giây (giả sử 60 FPS)
    const float SPEED_INCREASE_FACTOR = 0.1f; // Tăng 10% tốc độ mỗi lần
    const float MAX_SPEED_MULTIPLIER = 3.0f; // Giới hạn tốc độ tối đa

public:
    Obstacle(SDL_Renderer* renderer, int* char_x, int* char_y, int window_width, int window_height, int ground_level);
    ~Obstacle();
    void moveAllObstacles(int offsetX);
    void update(bool world_moving, int scroll_speed);
    void render();
    bool isColliding(int character_width, int character_height);
    void reset();
    // Modified spawnObstacle to take a parameter for side (top/bottom)
    void spawnObstacle(bool is_top);
    
    // Thêm phương thức để lấy tốc độ hiện tại
    float getCurrentSpeedMultiplier() const { return obstacle_speed_multiplier; }
};
#endif // OBSTACLE_H