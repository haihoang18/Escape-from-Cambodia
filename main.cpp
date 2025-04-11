#define SDL_MAIN_HANDLED

#include "background.h"
#include "utils.h"
#include "stungun.h"
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <iostream>

using namespace std;

// Kích thước cửa sổ
const int WINDOW_WIDTH = 1400;
const int WINDOW_HEIGHT = 750;

// Kích thước nhân vật (có thể điều chỉnh tùy theo hình ảnh thực tế)
const int CHARACTER_WIDTH = 125;
const int CHARACTER_HEIGHT = 125;

// Thông số game
const int GROUND_LEVEL = WINDOW_HEIGHT - 100; // Mặt đất cách đáy 100 pixel
const int JUMP_VELOCITY = -20; // Tốc độ nhảy ban đầu (âm để đi lên)
const int GRAVITY = 1; // Gia tốc trọng trường
const int MOVE_SPEED = 5; // Tốc độ di chuyển nhân vật

// Thông số animation
const int ANIMATION_SPEED = 10; // Số frame để chuyển sang frame animation tiếp theo

// Thông số background
const int BACKGROUND_WIDTH = WINDOW_WIDTH; // Chiều rộng background bằng chiều rộng cửa sổ
const float BACKGROUND_SCROLL_SPEED = 2; // Tốc độ cuộn background

int main(int argc, char* argv[]) {
    // Khởi tạo SDL và SDL_image
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "Không thể khởi tạo SDL: " << SDL_GetError() << endl;
        return -1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cout << "Không thể khởi tạo SDL_image: " << IMG_GetError() << endl;
        SDL_Quit();
        return -1;
    }

    // Tạo cửa sổ và renderer
    SDL_Window* window = SDL_CreateWindow("Game Nhan Vat", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Không thể tạo cửa sổ: " << SDL_GetError() << endl;
        IMG_Quit();
        SDL_Quit();
        return -1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cout << "Không thể tạo renderer: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Tải hình ảnh nhân vật
    SDL_Texture* horseman_right = load_texture(renderer, "character/horseman_right.png");
    SDL_Texture* horseman_left = load_texture(renderer, "character/horseman_left.png");
    
    // Tải hình ảnh animation di chuyển
    SDL_Texture* horseman_right_1 = load_texture(renderer, "character/horseman_right_1.png");
    SDL_Texture* horseman_right_2 = load_texture(renderer, "character/horseman_right_2.png");
    SDL_Texture* horseman_left_1 = load_texture(renderer, "character/horseman_left_1.png");
    SDL_Texture* horseman_left_2 = load_texture(renderer, "character/horseman_left_2.png");
    
    // Tải hình ảnh nhảy
    SDL_Texture* horseman_jump_right = load_texture(renderer, "character/horseman_jump_right.png");
    SDL_Texture* horseman_jump_left = load_texture(renderer, "character/horseman_jump_left.png");
    
    // Kiểm tra tất cả textures đã được tải đúng
    if (!horseman_right || !horseman_left || 
        !horseman_right_1 || !horseman_right_2 || 
        !horseman_left_1 || !horseman_left_2 || 
        !horseman_jump_right || !horseman_jump_left) {
        cout << "Không thể tải hình ảnh nhân vật!" << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Tải hình ảnh các background
    SDL_Texture* background1 = load_texture(renderer, "background/background1.png");
    SDL_Texture* background2 = load_texture(renderer, "background/background2.png");
    if (!background1 || !background2) {
        cout << "Không thể tải hình ảnh background!" << endl;
        // Giải phóng tất cả texture đã tải
        SDL_DestroyTexture(horseman_right);
        SDL_DestroyTexture(horseman_left);
        SDL_DestroyTexture(horseman_right_1);
        SDL_DestroyTexture(horseman_right_2);
        SDL_DestroyTexture(horseman_left_1);
        SDL_DestroyTexture(horseman_left_2);
        SDL_DestroyTexture(horseman_jump_right);
        SDL_DestroyTexture(horseman_jump_left);
        if (background1) SDL_DestroyTexture(background1);
        if (background2) SDL_DestroyTexture(background2);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Khởi tạo trạng thái nhân vật
    int character_x = 0; // Vị trí ban đầu giữa màn hình
    int character_y = GROUND_LEVEL - CHARACTER_HEIGHT; // Đặt trên mặt đất
    int velocity_y = 0; // Vận tốc theo trục y (dùng cho nhảy)
    bool is_jumping = false; // Trạng thái nhảy
    bool facing_right = true; // Hướng ban đầu: quay phải
    bool is_moving = false; // Trạng thái di chuyển
    
    // Biến kiểm tra phím nhấn
    bool moving_left = false;
    bool moving_right = false;
    
    // Biến đếm frame cho animation
    int animation_frame = 0;
    int animation_counter = 0;
    
    // Tạo đối tượng stungun đuổi theo nhân vật
    StunGun stungun(renderer, &character_x, &character_y);
    
    // Biến kiểm tra va chạm với stungun
    bool hit_by_stungun = false;
    
    // Đếm thời gian bị stunned
    int stunned_timer = 0;
    const int STUNNED_DURATION = 60; // Khoảng 1 giây ở 60 FPS

    // Biến theo dõi background
    float bg_pos_x = 0; // Vị trí hiện tại của background
    bool showing_background1 = true; // Đang hiển thị background1
    bool transition_complete = false; // Quá trình chuyển đổi đã hoàn thành

    // Vòng lặp chính
    bool running = true;
    SDL_Event event;
    while (running) {
        // Xử lý sự kiện bàn phím
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN && !hit_by_stungun) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT: // Di chuyển sang trái
                        moving_left = true;
                        facing_right = false;
                        is_moving = true;
                        break;
                    case SDLK_RIGHT: // Di chuyển sang phải
                        moving_right = true;
                        facing_right = true;
                        is_moving = true;
                        break;
                    case SDLK_UP: // Nhảy
                        if (!is_jumping) {
                            velocity_y = JUMP_VELOCITY;
                            is_jumping = true;
                        }
                        break;
                }
            } else if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        moving_left = false;
                        if (!moving_right) is_moving = false;
                        break;
                    case SDLK_RIGHT:
                        moving_right = false;
                        if (!moving_left) is_moving = false;
                        break;
                }
            }
        }
        
        // Kiểm tra va chạm với stungun
        if (!hit_by_stungun && stungun.isColliding(CHARACTER_WIDTH, CHARACTER_HEIGHT)) {
            hit_by_stungun = true;
            stungun.resetAfterCollision(); // Đặt lại vị trí stungun sau khi va chạm
        }

        // Xử lý thời gian stunned
        if (hit_by_stungun) {
            stunned_timer++;
            if (stunned_timer >= STUNNED_DURATION) {
                hit_by_stungun = false;
                stunned_timer = 0;
                // Đảm bảo stungun được đặt lại vị trí nếu chưa được làm trước đó
                stungun.resetAfterCollision();
            }
        }
        
        // Xử lý di chuyển (cả khi đứng yên và khi nhảy)
        if (moving_left && !hit_by_stungun) {
            character_x -= MOVE_SPEED;
            
            // Chỉ cuộn background khi nhân vật ở gần rìa trái
            if (character_x < WINDOW_WIDTH / 4 && bg_pos_x < 0) {
                bg_pos_x += BACKGROUND_SCROLL_SPEED;
                character_x += BACKGROUND_SCROLL_SPEED; // Giữ nhân vật ở vị trí tương đối
            }
        }
        if (moving_right && !hit_by_stungun) {
    // Nếu vẫn đang ở background1 và chưa đến ranh giới
    if (showing_background1) {
        character_x += MOVE_SPEED;
        
        // Khi gần đến ranh giới background1 và background2
        if (character_x > WINDOW_WIDTH * 3/4 && bg_pos_x > -BACKGROUND_WIDTH) {
            // Cuộn background thay vì di chuyển nhân vật
            bg_pos_x -= BACKGROUND_SCROLL_SPEED;
            character_x -= BACKGROUND_SCROLL_SPEED; // Giữ nhân vật ở vị trí tương đối
        }
        
        // Khi đến ranh giới background1 và background2
        if (bg_pos_x <= -BACKGROUND_WIDTH + CHARACTER_WIDTH + 50) { // Thêm một chút khoảng cách
            // Chuyển sang background2
            showing_background1 = false;
            bg_pos_x = 0; // Reset vị trí background
            character_x = 50; // Đặt nhân vật ở đầu background2 (thêm một chút khoảng cách)
        }
    } else {
        // Đã ở background2, cho phép di chuyển bình thường
        character_x += MOVE_SPEED;
        
        // Giới hạn không cho di chuyển quá ranh giới bên phải của background2
        if (character_x > WINDOW_WIDTH - CHARACTER_WIDTH - 50) {
            character_x = WINDOW_WIDTH - CHARACTER_WIDTH - 50;
        }
    }
}

        // Cập nhật vị trí nhân vật khi nhảy
        if (is_jumping && !hit_by_stungun) {
            character_y += velocity_y; // Cập nhật vị trí y
            velocity_y += GRAVITY; // Áp dụng trọng lực
            
            if (character_y >= GROUND_LEVEL - CHARACTER_HEIGHT) { // Chạm đất
                character_y = GROUND_LEVEL - CHARACTER_HEIGHT;
                is_jumping = false;
                velocity_y = 0;
            }
        }

        // Giới hạn nhân vật trong màn hình
        if (character_x < 0) character_x = 0;
        if (character_x > WINDOW_WIDTH - CHARACTER_WIDTH) character_x = WINDOW_WIDTH - CHARACTER_WIDTH;

        // Cập nhật frame animation khi di chuyển
        if (is_moving && !is_jumping && !hit_by_stungun) {
            animation_counter++;
            if (animation_counter >= ANIMATION_SPEED) {
                animation_counter = 0;
                animation_frame = (animation_frame + 1) % 3; // Sử dụng 3 frame (0: default, 1, 2)
            }
        } else if (!is_moving) {
            animation_frame = 0; // Reset về frame mặc định khi không di chuyển
            animation_counter = 0;
        }
        
        // Cập nhật vị trí stungun
        stungun.update();

       // Xóa màn hình
SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu đen
SDL_RenderClear(renderer);

// Vẽ background
if (showing_background1) {
    // Vẽ background1 ở vị trí hiện tại
    SDL_Rect bg_rect1 = {(int)bg_pos_x, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderCopy(renderer, background1, NULL, &bg_rect1);
    
    // Vẽ background2 ngay sau background1
    SDL_Rect bg_rect2 = {(int)bg_pos_x + BACKGROUND_WIDTH, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderCopy(renderer, background2, NULL, &bg_rect2);
} else {
    // Chỉ vẽ background2 khi đã chuyển sang
    SDL_Rect bg_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderCopy(renderer, background2, NULL, &bg_rect);
}

        // Vẽ nhân vật với animation thích hợp
        SDL_Rect character_rect = {character_x, character_y, CHARACTER_WIDTH, CHARACTER_HEIGHT};
        
        if (is_jumping) {
            // Sử dụng hình ảnh nhảy dựa vào hướng
            if (facing_right) {
                SDL_RenderCopy(renderer, horseman_jump_right, NULL, &character_rect);
            } else {
                SDL_RenderCopy(renderer, horseman_jump_left, NULL, &character_rect);
            }
        } else if (is_moving) {
            // Sử dụng animation di chuyển
            if (facing_right) {
                switch (animation_frame) {
                    case 0:
                        SDL_RenderCopy(renderer, horseman_right, NULL, &character_rect);
                        break;
                    case 1:
                        SDL_RenderCopy(renderer, horseman_right_1, NULL, &character_rect);
                        break;
                    case 2:
                        SDL_RenderCopy(renderer, horseman_right_2, NULL, &character_rect);
                        break;
                }
            } else {
                switch (animation_frame) {
                    case 0:
                        SDL_RenderCopy(renderer, horseman_left, NULL, &character_rect);
                        break;
                    case 1:
                        SDL_RenderCopy(renderer, horseman_left_1, NULL, &character_rect);
                        break;
                    case 2:
                        SDL_RenderCopy(renderer, horseman_left_2, NULL, &character_rect);
                        break;
                }
            }
        } else {
            // Sử dụng hình ảnh đứng bình thường
            if (facing_right) {
                SDL_RenderCopy(renderer, horseman_right, NULL, &character_rect);
            } else {
                SDL_RenderCopy(renderer, horseman_left, NULL, &character_rect);
            }
        }
        
        // Hiệu ứng bị stunned (nhấp nháy)
        if (hit_by_stungun && (stunned_timer / 5) % 2 == 0) {
            // Tạo hiệu ứng nhấp nháy bằng cách không vẽ nhân vật ở một số frame
        } else {
            // Vẽ nhân vật bình thường
        }
        
        // Vẽ stungun
        stungun.render();

        // Hiển thị lên màn hình
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Giới hạn khoảng 60 FPS
    }

    // Giải phóng tài nguyên
    SDL_DestroyTexture(horseman_right);
    SDL_DestroyTexture(horseman_left);
    SDL_DestroyTexture(horseman_right_1);
    SDL_DestroyTexture(horseman_right_2);
    SDL_DestroyTexture(horseman_left_1);
    SDL_DestroyTexture(horseman_left_2);
    SDL_DestroyTexture(horseman_jump_right);
    SDL_DestroyTexture(horseman_jump_left);
    SDL_DestroyTexture(background1);
    SDL_DestroyTexture(background2);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}