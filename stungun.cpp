#include "stungun.h"
#include "utils.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Constructor
StunGun::StunGun(SDL_Renderer* renderer, int* target_x, int* target_y) {
    this->renderer = renderer;
    this->target_x = target_x;
    this->target_y = target_y;
    
    // Khởi tạo vị trí ban đầu từ bên trái màn hình
    resetPosition();
    
    // Kích thước stungun lớn hơn (70% chiều cao màn hình)
    const int WINDOW_HEIGHT = 750;
    this->height = WINDOW_HEIGHT * 0.7;  // 70% chiều cao màn hình
    this->width = this->height;          // Giữ tỷ lệ khung hình vuông
    
    // Tốc độ di chuyển (chậm hơn)
    this->speed = 2;
    
    // Thiết lập animation
    this->current_frame = 0;
    this->animation_counter = 0;
    this->animation_speed = 10; // Tăng tốc độ animation
    
    // Khởi tạo random seed
    std::srand(std::time(nullptr));
    
    // Load textures
    loadTextures();
}

// Destructor
StunGun::~StunGun() {
    // Giải phóng textures
    for (SDL_Texture* texture : textures) {
        SDL_DestroyTexture(texture);
    }
    textures.clear();
}

// Load textures
bool StunGun::loadTextures() {
    // Load các texture animation của stungun
    SDL_Texture* stungun_1 = load_texture(renderer, "stungun/stungun_1.png");
    SDL_Texture* stungun_2 = load_texture(renderer, "stungun/stungun_2.png");
    SDL_Texture* stungun_3 = load_texture(renderer, "stungun/stungun_3.png");
    
    if (!stungun_1 || !stungun_2 || !stungun_3) {
        std::cout << "Không thể tải hình ảnh stungun!" << std::endl;
        return false;
    }
    
    // Thêm vào vector
    textures.push_back(stungun_1);
    textures.push_back(stungun_2);
    textures.push_back(stungun_3);
    
    return true;
}

// Đặt lại vị trí luôn từ bên trái màn hình
void StunGun::resetPosition() {
    // Kích thước màn hình
    const int WINDOW_WIDTH = 1400;
    const int WINDOW_HEIGHT = 750;
    
    // Đặt vị trí bên ngoài màn hình (bên trái)
    x = -width;
    
    // Vị trí y ngẫu nhiên trong khoảng từ 0 đến chiều cao màn hình trừ chiều cao stungun
    y = std::rand() % (WINDOW_HEIGHT - height);
}

// Cập nhật vị trí
void StunGun::update() {
    // Tính hướng di chuyển đến mục tiêu
    int target_center_x = *target_x + width/4; // Điều chỉnh để stungun tiếp cận nhưng không nhắm chính xác vào center
    int target_center_y = *target_y + height/4;
    int stungun_center_x = x + width/2;
    int stungun_center_y = y + height/2;
    
    // Tính vector hướng
    float dx = target_center_x - stungun_center_x;
    float dy = target_center_y - stungun_center_y;
    
    // Tính độ dài vector
    float length = sqrt(dx*dx + dy*dy);
    
    // Nếu đủ xa thì di chuyển theo hướng đó
    if (length > 10) {
        // Chuẩn hóa vector
        dx /= length;
        dy /= length;
        
        // Di chuyển theo hướng đã tính, với trọng số nhiều hơn cho chuyển động ngang
        x += static_cast<int>(dx * speed * 1.5); // Tăng tốc độ di chuyển ngang
        y += static_cast<int>(dy * speed * 0.7); // Giảm tốc độ di chuyển dọc
    }
    
    // Cập nhật animation
    animation_counter++;
    if (animation_counter >= animation_speed) {
        animation_counter = 0;
        current_frame = (current_frame + 1) % textures.size();
    }
}

// Vẽ stungun
void StunGun::render() {
    SDL_Rect stungun_rect = {x, y, width, height};
    SDL_RenderCopy(renderer, textures[current_frame], NULL, &stungun_rect);
}

// Kiểm tra va chạm với mục tiêu - điều chỉnh vùng va chạm
bool StunGun::isColliding(int target_width, int target_height) {
    // Tạo vùng va chạm nhỏ hơn một chút so với kích thước thực để game dễ chơi hơn
    int collision_margin = width / 5;
    
    return (x + collision_margin < *target_x + target_width &&
            x + width - collision_margin > *target_x &&
            y + collision_margin < *target_y + target_height &&
            y + height - collision_margin > *target_y);
}

// Reset vị trí khi bị stunned
void StunGun::resetAfterCollision() {
    resetPosition();
}