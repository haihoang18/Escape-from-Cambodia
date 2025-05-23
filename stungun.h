#ifndef STUNGUN_H
#define STUNGUN_H

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <vector>

class StunGun {
private:
    // Textures cho animation
    std::vector<SDL_Texture*> textures;
    
    // Vị trí và kích thước
    int x;
    int y;
    int width;
    int height;
    
    // Tốc độ di chuyển
    int speed;
    
    // Biến cho animation
    int current_frame;
    int animation_counter;
    int animation_speed;
    
    // Mục tiêu (nhân vật) để đuổi theo
    int* target_x;
    int* target_y;
    // Add to stungun.h in the private section:
    int last_target_x;          // Last known character x position
    int last_target_y;          // Last known character y position
    int idle_timer;             // Timer to track how long character has been idle
    const int IDLE_THRESHOLD;   // Time threshold before stungun activates
    const int MOVEMENT_THRESHOLD; // Distance threshold to consider character as "moved"
    bool is_active;             // Flag to track if stungun is actively chasing
    bool* is_jumping_ptr; // Con trỏ tới trạng thái nhảy của nhân vật
    // Renderer
    SDL_Renderer* renderer;
    
public:
    // Constructor
    StunGun(SDL_Renderer* renderer, int* target_x, int* target_y, bool* is_jumping_ptr);
    
    // Destructor
    ~StunGun();
    
    // Load textures
    bool loadTextures();
    
    // Đặt lại vị trí từ một góc màn hình
    void resetPosition();
    
    // Cập nhật vị trí
    void update();
    
    // Vẽ stungun
    void render();
    
    // Kiểm tra va chạm với mục tiêu
    bool isColliding(int target_width, int target_height);
    
    // Reset vị trí khi bị stunned
    void resetAfterCollision();
};

#endif // STUNGUN_H