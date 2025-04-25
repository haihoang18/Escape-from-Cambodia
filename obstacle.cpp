#include "obstacle.h"
#include "utils.h"
#include <iostream>
#include <random>
#include <ctime>
#include <SDL2_image/SDL_image.h>
#include <algorithm> // Required for std::remove_if

Obstacle::Obstacle(SDL_Renderer* renderer, int* char_x, int* char_y, int window_width, int window_height, int ground_level)
    : renderer(renderer), character_x_ptr(char_x), character_y_ptr(char_y),
      spawn_timer_top(0), spawn_interval_top(180), // Khoảng thời gian sinh sản trên cùng (ví dụ: 3 giây ở 60 FPS)
      spawn_timer_bottom(0), spawn_interval_bottom(120), // Khoảng thời gian sinh sản dưới cùng (ví dụ: 2 giây ở 60 FPS)
      WINDOW_WIDTH(window_width), WINDOW_HEIGHT(window_height),
      GROUND_LEVEL(ground_level) {

    // Tải texture cho chướng ngại vật
    texture = load_texture(renderer, "background/tungtungtungsahur.png");
    if (!texture) {
        std::cout << "Không thể tải hình ảnh chướng ngại vật!" << std::endl;
    }

    // Khởi tạo random number generator
    rng.seed(static_cast<unsigned int>(time(nullptr)));

    // Bạn có thể thêm logic để ngẫu nhiên hóa spawn_interval_top và spawn_interval_bottom tại đây
    // Ví dụ:
    // spawn_interval_top = 120 + (rng() % 180);
    // spawn_interval_bottom = 120 + (rng() % 180);


    // Phân phối chiều cao của chướng ngại vật (nếu cần thiết) - Not used for now as height is fixed
    // height_dist = std::uniform_int_distribution<int>(0, 0);
}

Obstacle::~Obstacle() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Obstacle::update(bool world_moving, int scroll_speed) {
    // Tăng bộ đếm thời gian spawn cho cả trên và dưới
    spawn_timer_top++;
    spawn_timer_bottom++;

    // Nếu thế giới đang di chuyển và đã đến lúc spawn chướng ngại vật trên cùng mới
    if (world_moving && spawn_timer_top >= spawn_interval_top) {
        spawnObstacle(true); // Sinh sản chướng ngại vật trên cùng
        // Đặt lại bộ đếm thời gian spawn trên cùng và khoảng thời gian ngẫu nhiên cho lần spawn tiếp theo
        spawn_timer_top = 0;
        spawn_interval_top = 120 + (rng() % 180); // Ngẫu nhiên hóa khoảng thời gian cho lần tiếp theo
    }

    // Nếu thế giới đang di chuyển và đã đến lúc spawn chướng ngại vật dưới cùng mới
    if (world_moving && spawn_timer_bottom >= spawn_interval_bottom) {
        spawnObstacle(false); // Sinh sản chướng ngại vật dưới cùng
        // Đặt lại bộ đếm thời gian spawn dưới cùng và khoảng thời gian ngẫu nhiên cho lần spawn tiếp theo
        spawn_timer_bottom = 0;
        spawn_interval_bottom = 120 + (rng() % 180); // Ngẫu nhiên hóa khoảng thời gian cho lần tiếp theo
    }


    // Cập nhật vị trí của các chướng ngại vật hiện có nếu thế giới đang di chuyển
    if (world_moving) {
        for (auto& obstacle : obstacles) {
            obstacle.x -= scroll_speed;
        }
    }

    // Xóa chướng ngại vật đã ra khỏi màn hình
    // Sử dụng erase-remove idiom để xóa hiệu quả
    obstacles.erase(
        std::remove_if(obstacles.begin(), obstacles.end(), [&](const SDL_Rect& obstacle) {
            return obstacle.x + obstacle.w < 0;
        }),
        obstacles.end()
    );
}

void Obstacle::render() {
    // Vẽ tất cả chướng ngại vật
    for (const auto& obstacle : obstacles) {
        // Determine flip based on y position (top or bottom)
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        // Assuming top obstacles are near the top (y=0) and bottom obstacles are near the ground level
        if (obstacle.y < WINDOW_HEIGHT / 2) { // Simple check: if obstacle is in the upper half of the screen
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
    spawn_timer_top = 0; // Reset cả hai bộ hẹn giờ
    spawn_timer_bottom = 0;
}

void Obstacle::spawnObstacle(bool is_top) {
    SDL_Rect new_obstacle;
    new_obstacle.w = OBSTACLE_WIDTH;
    new_obstacle.h = OBSTACLE_HEIGHT;
    new_obstacle.x = WINDOW_WIDTH; // Bắt đầu ngoài màn hình bên phải

    if (is_top) { // Spawn at the top
        new_obstacle.y = 0; // Place at the very top
    } else { // Spawn at the bottom
        new_obstacle.y = GROUND_LEVEL - OBSTACLE_HEIGHT;
    }

    obstacles.push_back(new_obstacle);
}