#define SDL_MAIN_HANDLED

#include "background.h"
#include "utils.h"
#include "stungun.h"
#include "obstacle.h" // Thêm include mới
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <iostream>
#include <cstdlib> // Thêm thư viện này cho rand() và srand()
#include <ctime>   // Thêm thư viện này cho time()

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
const int BACKGROUND_SCROLL_SPEED = 5; // Tốc độ cuộn background

// Biến cho hệ số nhảy ngẫu nhiên
float jump_multiplier = 1.0f; // Hệ số nhảy ban đầu
int jumps_with_multiplier = 0; // Số lần nhảy với hệ số hiện tại

// Hàm để tạo hệ số nhảy ngẫu nhiên mới
float generateRandomJumpMultiplier() {
    int random_value = rand() % 3; // Sinh số ngẫu nhiên 0, 1, hoặc 2
    if (random_value == 0) {
        return 0.5f;
    } else if (random_value == 1) {
        return 1.0f;
    } else {
        return 2.0f;
    }
}

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

    // Khởi tạo bộ tạo số ngẫu nhiên
    srand(static_cast<unsigned int>(time(nullptr)));

    // Tạo hệ số nhảy ngẫu nhiên ban đầu
    jump_multiplier = generateRandomJumpMultiplier();

    // ... (các phần code khởi tạo khác giữ nguyên)

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
    int character_x = WINDOW_WIDTH / 4; // Vị trí ban đầu 1/4 màn hình
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
    
    // Tạo đối tượng chướng ngại vật
    Obstacle obstacles(renderer, &character_x, &character_y, WINDOW_WIDTH, WINDOW_HEIGHT, GROUND_LEVEL);
    
    // Biến kiểm tra va chạm
    bool hit_by_stungun = false;
    bool game_over = false; // Biến mới cho trạng thái thua
    
    // Đếm thời gian bị stunned
    int stunned_timer = 0;
    const int STUNNED_DURATION = 60; // Khoảng 1 giây ở 60 FPS

    // Khởi tạo vị trí background
    int bg1_position_x = 0;
    int bg2_position_x = WINDOW_WIDTH; // Background 2 bắt đầu nối tiếp background 1

    // Biến kiểm soát game
    bool game_world_moving = false; // Khi true, thế giới di chuyển thay vì nhân vật

    // Vòng lặp chính
    bool running = true;
    SDL_Event event;
    while (running) {
        // Xử lý sự kiện bàn phím
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) { // Chỉ kiểm tra SDL_KEYDOWN
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT: // Di chuyển sang trái
                        if (!hit_by_stungun && !game_over) { // Chỉ di chuyển khi không bị stunned hoặc game over
                            moving_left = true;
                            facing_right = false;
                            is_moving = true;
                        }
                        break;
                    case SDLK_RIGHT: // Di chuyển sang phải
                        if (!hit_by_stungun && !game_over) { // Chỉ di chuyển khi không bị stunned hoặc game over
                            moving_right = true;
                            facing_right = true;
                            is_moving = true;
                        }
                        break;
                    case SDLK_UP: // Nhảy
                        if (!is_jumping && !hit_by_stungun && !game_over) { // Chỉ nhảy khi không đang nhảy, không bị stunned hoặc game over
                            velocity_y = JUMP_VELOCITY; // Sử dụng JUMP_VELOCITY gốc ở đây, hệ số nhảy được xử lý sau trong logic game
                            is_jumping = true;

                            // Logic cho hệ số nhảy ngẫu nhiên cần được di chuyển xuống sau khi tính toán velocity_y
                            // (hoặc áp dụng hệ số ngay tại đây)
                            // Áp dụng hệ số nhảy ngay khi nhấn phím UP
                            velocity_y = JUMP_VELOCITY * jump_multiplier;

                            // Cập nhật bộ đếm số lần nhảy với hệ số hiện tại
                            jumps_with_multiplier++;

                            // Nếu đã nhảy 2 lần với hệ số này, tạo hệ số mới và reset bộ đếm
                            if (jumps_with_multiplier >= 2) {
                                jump_multiplier = generateRandomJumpMultiplier();
                                jumps_with_multiplier = 0;
                            }
                        }
                        break;
                    case SDLK_r: // Phím tắt để khởi động lại game khi thua
                        if (game_over) {
                            // Reset trạng thái game
                            game_over = false;
                            character_x = WINDOW_WIDTH / 4;
                            character_y = GROUND_LEVEL - CHARACTER_HEIGHT;
                            velocity_y = 0;
                            is_jumping = false;
                            facing_right = true;
                            is_moving = false;
                            moving_left = false;
                            moving_right = false;
                            hit_by_stungun = false;
                            stunned_timer = 0;
                            bg1_position_x = 0;
                            bg2_position_x = WINDOW_WIDTH;
                            game_world_moving = false;
                            animation_frame = 0;
                            animation_counter = 0;

                            // Reset các đối tượng trong game
                            stungun.resetAfterCollision();
                            obstacles.reset();

                             // Reset lại hệ số nhảy và bộ đếm khi game restart
                            jump_multiplier = generateRandomJumpMultiplier();
                            jumps_with_multiplier = 0;
                        }
                        break;
                }
            } else if (event.type == SDL_KEYUP) {
               // ... xử lý KEYUP (đảm bảo các điều kiện !hit_by_stungun && !game_over cũng được thêm vào đây nếu cần ngăn dừng di chuyển khi game over)
               switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        if (!hit_by_stungun && !game_over) {
                            moving_left = false;
                            if (!moving_right) is_moving = false;
                        }
                        break;
                    case SDLK_RIGHT:
                         if (!hit_by_stungun && !game_over) {
                            moving_right = false;
                            if (!moving_left) is_moving = false;
                        }
                        break;
                }
            }
        }
        
        if (!game_over) {
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
            
            // Kiểm tra va chạm với chướng ngại vật
            if (obstacles.isColliding(CHARACTER_WIDTH, CHARACTER_HEIGHT)) {
                game_over = true; // Khi va chạm với chướng ngại vật, game kết thúc
            }
            
            // Xử lý di chuyển nhân vật
            if (moving_left && !hit_by_stungun) {
                if (character_x > 0) {
                    // Nếu nhân vật chưa chạm biên trái màn hình, cho phép di chuyển
                    character_x -= MOVE_SPEED;
                    
                    // Nếu đang ở chế độ thế giới di chuyển và nhân vật đi lùi về bên trái
                    // đủ xa (ví dụ như dưới 1/4 màn hình), tắt chế độ thế giới di chuyển
                    if (game_world_moving && character_x < WINDOW_WIDTH / 4) {
                        game_world_moving = false;
                    }
                }
            }

            if (moving_right && !hit_by_stungun) {
                // Nếu nhân vật chưa đến giữa màn hình hoặc đang không ở chế độ thế giới di chuyển
                if (character_x < WINDOW_WIDTH / 2 && !game_world_moving) {
                    character_x += MOVE_SPEED;
                    
                    // Nếu nhân vật đến giữa màn hình, bật chế độ thế giới di chuyển
                    if (character_x >= WINDOW_WIDTH / 2) {
                        game_world_moving = true;
                    }
                } 
                // Nếu đang ở chế độ thế giới di chuyển, cuộn background thay vì di chuyển nhân vật
                else if (game_world_moving) {
                    moving_background(MOVE_SPEED, bg1_position_x, bg2_position_x, 
                                    renderer, background1, background2, WINDOW_WIDTH, WINDOW_HEIGHT);
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
            
            // Cập nhật chướng ngại vật
            obstacles.update(game_world_moving, MOVE_SPEED);
        }

        // Xóa màn hình
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu đen
        SDL_RenderClear(renderer);
        
        // Vẽ background
        SDL_Rect bg_rect1 = {bg1_position_x, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, background1, NULL, &bg_rect1);
        
        SDL_Rect bg_rect2 = {bg2_position_x, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, background2, NULL, &bg_rect2);
        
        // Vẽ chướng ngại vật
        obstacles.render();

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
        
        // Hiển thị thông báo khi thua cuộc
        if (game_over) {
            // Có thể tạo một texture chữ "GAME OVER" ở đây
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 192); // Màu đen, mờ đục
            SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
            SDL_RenderFillRect(renderer, &overlay);
            
            // Hiển thị thông báo bằng cách vẽ text (cần thêm SDL_ttf cho phần này)
            // Hoặc bạn có thể tạo và tải trước một texture "GAME OVER"
            
            // Thông báo để khởi động lại
            // Bạn có thể thêm text "Nhấn R để chơi lại"
        }

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