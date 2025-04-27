#include "obstacle.h"
#include "utils.h"
#include <iostream>
#include <random>
#include <ctime>
#include <SDL2_image/SDL_image.h>
#include <algorithm> 

Obstacle::Obstacle(SDL_Renderer* renderer, int* char_x, int* char_y, int window_width, int window_height, int ground_level)
    : renderer(renderer), character_x_ptr(char_x), character_y_ptr(char_y),
      WINDOW_WIDTH(window_width), WINDOW_HEIGHT(window_height), GROUND_LEVEL(ground_level) {
    
    // Khởi tạo texture vật cản 
    texture = IMG_LoadTexture(renderer, "background/tungtungtungsahur.png");
    if (!texture) {
        std::cout << "Không thể tải texture vật cản! SDL_image Error: " << IMG_GetError() << std::endl;
    }
    
    // Khởi tạo các timer và interval cho vật cản
    spawn_timer_top = 0;
    spawn_interval_top = 180; // 3 giây ở 60 FPS
    spawn_timer_bottom = 0;
    spawn_interval_bottom = 300; // 5 giây ở 60 FPS
    
    // Cài đặt generator số ngẫu nhiên
    rng.seed(static_cast<unsigned int>(std::time(nullptr)));
    height_dist = std::uniform_int_distribution<int>(100, 300); // Phạm vi chiều cao ngẫu nhiên
    
    // Thêm khởi tạo biến tốc độ và timer
    obstacle_speed_multiplier = 1.0f;
    speed_increase_timer = 0;
}

Obstacle::~Obstacle() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Obstacle::update(bool world_moving, int scroll_speed) {
    // Tăng timer tốc độ
    speed_increase_timer++;
    
    // Kiểm tra nếu đến thời điểm tăng tốc
    if (speed_increase_timer >= SPEED_INCREASE_INTERVAL) {
        speed_increase_timer = 0;
        if (obstacle_speed_multiplier < MAX_SPEED_MULTIPLIER) {
            obstacle_speed_multiplier += SPEED_INCREASE_FACTOR;
            // Giới hạn tốc độ tối đa
            if (obstacle_speed_multiplier > MAX_SPEED_MULTIPLIER) {
                obstacle_speed_multiplier = MAX_SPEED_MULTIPLIER; // Tốc độ tối đa : 3 lần lúc đầu
            }
        }
    }
    
    // Cập nhật vị trí các vật cản hiện có 
    auto it = obstacles.begin();
    while (it != obstacles.end()) {
        if (world_moving) {
            // Áp dụng hệ số tốc độ vào tốc độ di chuyển
            it->x -= static_cast<int>(scroll_speed * obstacle_speed_multiplier);
        }
        
        // Xóa vật cản nếu đi ra khỏi màn hình
        if (it->x + it->w < 0) {
            it = obstacles.erase(it);
        } else {
            ++it;
        }
    }
    
    // Cập nhật timer và spawn vật cản 
    spawn_timer_top++;
    if (spawn_timer_top >= spawn_interval_top) {
        spawn_timer_top = 0;
        // Áp dụng hệ số tốc độ để giảm khoảng thời gian spawn
        int adjusted_interval = static_cast<int>(180 / obstacle_speed_multiplier);
        // Đảm bảo khoảng thời gian spawn không quá ngắn
        spawn_interval_top = (adjusted_interval < 60) ? 60 : adjusted_interval;
        spawnObstacle(true);
    }
    
    spawn_timer_bottom++;
    if (spawn_timer_bottom >= spawn_interval_bottom) {
        spawn_timer_bottom = 0;
        // Áp dụng hệ số tốc độ để giảm khoảng thời gian spawn
        int adjusted_interval = static_cast<int>(300 / obstacle_speed_multiplier);
        // Đảm bảo khoảng thời gian spawn không quá ngắn
        spawn_interval_bottom = (adjusted_interval < 90) ? 90 : adjusted_interval;
        spawnObstacle(false);
    }
}

void Obstacle::render() {
    // Vẽ tất cả chướng ngại vật
    for (const auto& obstacle : obstacles) {
        // Determine flip based on y position (top or bottom)
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        // Assuming top obstacles are near the top (y=0) and bottom obstacles are near the ground level
        if (obstacle.y < WINDOW_HEIGHT / 2) { 
            flip = SDL_FLIP_VERTICAL;
        }

        SDL_RenderCopyEx(renderer, texture, NULL, &obstacle, 0, NULL, flip);
    }
}

bool Obstacle::isColliding(int character_width, int character_height) {
    int char_x = *character_x_ptr;
    int char_y = *character_y_ptr;

    for (const auto& obstacle : obstacles) {
        // Kiểm tra va chạm hình chữ nhật đơn giản
        // Định nghĩa một lề va chạm (bạn có thể điều chỉnh giá trị này)
        int collision_margin = 20;

        // Kiểm tra va chạm hình chữ nhật với lề
        if (char_x + collision_margin < obstacle.x + obstacle.w - collision_margin &&
            char_x + character_width - collision_margin > obstacle.x + collision_margin &&
            char_y + collision_margin < obstacle.y + obstacle.h - collision_margin &&
            char_y + character_height - collision_margin > obstacle.y + collision_margin) {
            return true;
        }
    }

    return false;
}

void Obstacle::reset() {
    // Xóa tất cả vật cản hiện có
    obstacles.clear();
    
    // Reset các timer và interval
    spawn_timer_top = 0;
    spawn_interval_top = 180; // 3 giây ở 60 FPS
    spawn_timer_bottom = 0;
    spawn_interval_bottom = 300; // 5 giây ở 60 FPS
    
    // Reset tốc độ và timer tốc độ
    obstacle_speed_multiplier = 1.0f;
    speed_increase_timer = 0;
}


void Obstacle::spawnObstacle(bool is_top) {
    SDL_Rect new_obstacle;
    new_obstacle.w = OBSTACLE_WIDTH;
    
    // Generate random height for more variety
    new_obstacle.h = height_dist(rng); 
    new_obstacle.x = WINDOW_WIDTH; // Start off-screen on the right
    
    if (is_top) { // Spawn at the top
        new_obstacle.y = 0; // Top of screen
    } else { // Spawn at the bottom
        new_obstacle.y = GROUND_LEVEL - new_obstacle.h; // Place on ground level
    }
    
    // Add obstacle to the list
    obstacles.push_back(new_obstacle);
    
}

void Obstacle::moveAllObstacles(int offsetX) {
    // Di chuyển tất cả vật cản hiện có theo offsetX
    for (auto& obstacle : obstacles) {
        obstacle.x += offsetX;
    }
}