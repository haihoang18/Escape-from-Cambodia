#include "obstacle.h"
#include "utils.h"
#include <iostream>

Obstacle::Obstacle(SDL_Renderer* renderer, int* char_x, int* char_y, int window_width, int window_height, int ground_level) 
    : renderer(renderer), character_x_ptr(char_x), character_y_ptr(char_y),
      spawn_timer(0), WINDOW_WIDTH(window_width), WINDOW_HEIGHT(window_height),
      GROUND_LEVEL(ground_level) {
    
    // Tải texture cho chướng ngại vật
    texture = load_texture(renderer, "background/tungtungtungsahur.png");
    if (!texture) {
        std::cout << "Không thể tải hình ảnh chướng ngại vật!" << std::endl;
    }
    
    // Khởi tạo random number generator
    rng.seed(static_cast<unsigned int>(time(nullptr)));
    
    // Thiết lập khoảng thời gian giữa các lần spawn (2-5 giây ở 60 FPS)
    spawn_interval = 120 + (rng() % 180);
    
    // Phân phối chiều cao của chướng ngại vật (nếu cần thiết)
    height_dist = std::uniform_int_distribution<int>(0, 0); // Hiện tại chỉ đặt trên mặt đất
}

Obstacle::~Obstacle() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Obstacle::update(bool world_moving, int scroll_speed) {
    // Tăng bộ đếm thời gian spawn
    spawn_timer++;
    
    // Nếu đến thời gian spawn, tạo một chướng ngại vật mới
    if (spawn_timer >= spawn_interval) {
        spawnObstacle();
        spawn_timer = 0;
        // Ngẫu nhiên khoảng thời gian cho lần spawn tiếp theo
        spawn_interval = 120 + (rng() % 180);
    }
    
    // Cập nhật vị trí của tất cả chướng ngại vật
    for (auto it = obstacles.begin(); it != obstacles.end();) {
        // Di chuyển chướng ngại vật sang trái
        if (world_moving) {
            it->x -= scroll_speed;
        }
        
        // Xóa chướng ngại vật nếu đã đi ra khỏi màn hình
        if (it->x + OBSTACLE_WIDTH < 0) {
            it = obstacles.erase(it);
        } else {
            ++it;
        }
    }
}

void Obstacle::render() {
    // Vẽ tất cả chướng ngại vật
    for (const auto& obstacle : obstacles) {
        SDL_RenderCopy(renderer, texture, NULL, &obstacle);
    }
}

bool Obstacle::isColliding(int character_width, int character_height) {
    int char_x = *character_x_ptr;
    int char_y = *character_y_ptr;
    
    for (const auto& obstacle : obstacles) {
        // Kiểm tra va chạm hình chữ nhật đơn giản
        // Định nghĩa một lề va chạm (bạn có thể điều chỉnh giá trị này)
        int collision_margin = 20; // Ví dụ: giảm 20 pixel từ mỗi cạnh

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
    // Xóa tất cả chướng ngại vật
    obstacles.clear();
    spawn_timer = 0;
}

void Obstacle::spawnObstacle() {
    SDL_Rect new_obstacle;
    new_obstacle.w = OBSTACLE_WIDTH;
    new_obstacle.h = OBSTACLE_HEIGHT;
    new_obstacle.x = WINDOW_WIDTH; // Spawn ở phía bên phải màn hình
    new_obstacle.y = GROUND_LEVEL - OBSTACLE_HEIGHT; // Đặt trên mặt đất
    
    obstacles.push_back(new_obstacle);
}