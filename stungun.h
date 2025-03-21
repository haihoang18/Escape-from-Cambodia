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
    
    // Renderer
    SDL_Renderer* renderer;
    
public:
    // Constructor
    StunGun(SDL_Renderer* renderer, int* target_x, int* target_y);
    
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