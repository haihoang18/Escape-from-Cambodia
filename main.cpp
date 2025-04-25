#define SDL_MAIN_HANDLED

#include "background.h"
#include "utils.h"
#include "stungun.h"
#include "obstacle.h"
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath> // Thêm thư viện cmath cho sqrt nếu cần trong các file khác, mặc dù main không dùng trực tiếp

using namespace std;

// Kích thước cửa sổ
const int WINDOW_WIDTH = 1400;
const int WINDOW_HEIGHT = 750;

// Kích thước nhân vật (có thể điều chỉnh tùy theo hình ảnh thực tế)
const int CHARACTER_WIDTH = 125;
const int CHARACTER_HEIGHT = 125;

// Thông số game
const int GROUND_LEVEL = WINDOW_HEIGHT - 100; // Mặt đất cách đáy 100 pixel
const int JUMP_VELOCITY = -25; // Tốc độ nhảy ban đầu (âm để đi lên)
const int GRAVITY = 1; // Gia tốc trọng trường
const int MOVE_SPEED = 5; // Tốc độ di chuyển nhân vật

// Thông số animation
const int ANIMATION_SPEED = 10; // Số frame để chuyển sang frame animation tiếp theo

// Thông số background
const int BACKGROUND_SCROLL_SPEED = 5; // Tốc độ cuộn background


// Cấu trúc cho vật phẩm Power-up
struct PowerUp {
    SDL_Rect rect;
    bool active; // Cờ để kiểm tra vật phẩm có đang hoạt động không
};

// Biến cho hệ số nhảy ngẫu nhiên
float jump_multiplier = 1.0f;
int jumps_with_multiplier = 0;

// Texture cho vật phẩm power-up
SDL_Texture* power_up_texture = nullptr;
// Danh sách các vật phẩm power-up đang hoạt động
std::vector<PowerUp> power_ups;
// Cờ cho trạng thái "có thể bay"
bool can_fly = false;
// Bộ đếm thời gian bay
int fly_timer = 0;
const int FLY_DURATION_FRAMES = 5 * 60; // Thời gian bay 5 giây ở 60 FPS (giả sử 60 FPS)

// Thông số spawn vật phẩm
int power_up_spawn_timer = 0;
const int POWER_UP_SPAWN_INTERVAL = 300; // Khoảng thời gian tối thiểu giữa các lần thử spawn (ví dụ: 5 giây ở 60 FPS)
const int POWER_UP_SPAWN_CHANCE = 10; // Tỷ lệ 1/20 để spawn khi timer đạt ngưỡng

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

// Hàm load texture (giữ lại vì vẫn dùng texture cho các đối tượng khác)
SDL_Texture* load_texture(SDL_Renderer* renderer, const string& file_path) {
    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(file_path.c_str());
    if (loadedSurface == nullptr) {
        cout << "Không thể tải hình ảnh " << file_path << "! SDL_image Error: " << IMG_GetError() << endl;
    } else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == nullptr) {
            cout << "Không thể tạo texture từ " << file_path << "! SDL Error: " << SDL_GetError() << endl;
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
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

    // Tạo cửa sổ và renderer
    SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
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

    // Tải texture cho vật phẩm power-up
    power_up_texture = load_texture(renderer, "background/bui_tien.png");
    if (!power_up_texture) {
        cout << "Không thể tải texture vật phẩm power-up!" << endl;
        // Tiếp tục game mà không có power-up hoặc thoát tùy ý
    }

    // Tải các texture nhân vật (giữ lại)
    SDL_Texture* horseman_right = load_texture(renderer, "character/horseman_right.png");
    SDL_Texture* horseman_left = load_texture(renderer, "character/horseman_left.png");
     SDL_Texture* horseman_right_1 = load_texture(renderer, "character/horseman_right_1.png");
    SDL_Texture* horseman_right_2 = load_texture(renderer, "character/horseman_right_2.png");
    SDL_Texture* horseman_left_1 = load_texture(renderer, "character/horseman_left_1.png");
    SDL_Texture* horseman_left_2 = load_texture(renderer, "character/horseman_left_2.png");
    SDL_Texture* horseman_jump_right = load_texture(renderer, "character/horseman_jump_right.png");
    SDL_Texture* horseman_jump_left = load_texture(renderer, "character/horseman_jump_left.png");


     if (!horseman_right || !horseman_left || !horseman_right_1 || !horseman_right_2 || !horseman_left_1 || !horseman_left_2 || !horseman_jump_right || !horseman_jump_left) {
         cout << "Lỗi tải texture nhân vật!" << endl;
         // Xử lý lỗi, có thể thoát game
          SDL_DestroyRenderer(renderer);
          SDL_DestroyWindow(window);
          IMG_Quit();
          SDL_Quit();
          return -1;
     }


    // Khởi tạo vị trí nhân vật
    int character_x = WINDOW_WIDTH / 4;
    int character_y = GROUND_LEVEL - CHARACTER_HEIGHT;
    int velocity_y = 0;
    bool is_jumping = false;
    bool facing_right = true;
    bool is_moving = false;
    bool moving_left = false;
    bool moving_right = false;
    bool game_over = false;
    bool hit_by_stungun = false; // Biến để kiểm tra xem nhân vật có bị stungun bắn trúng không
    int stunned_timer = 0; // Bộ đếm thời gian bị stunned

    // Khởi tạo stungun
    StunGun stungun(renderer, &character_x, &character_y);

    // Khởi tạo vật cản
    Obstacle obstacles(renderer, &character_x, &character_y, WINDOW_WIDTH, WINDOW_HEIGHT, GROUND_LEVEL);


    // Khởi tạo background (giữ lại texture)
    SDL_Texture* background1 = load_texture(renderer, "background/background1.png");
    SDL_Texture* background2 = load_texture(renderer, "background/background2.png");
     if (!background1 || !background2) {
        cout << "Không thể tải hình ảnh background!" << endl;
        // Tiếp tục game mà không có background hoặc thoát tùy ý
    }
    int bg1_position_x = 0;
    int bg2_position_x = WINDOW_WIDTH;
    bool game_world_moving = true; // Biến để kiểm soát khi thế giới game di chuyển


    // Biến cho animation nhân vật
    int animation_frame = 0;
    int animation_counter = 0;

    // Vòng lặp game chính
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT: // Di chuyển sang trái
                        if (!hit_by_stungun && !game_over && !can_fly) { // Chỉ di chuyển khi không bị stunned, game over, và không bay
                            moving_left = true;
                            facing_right = false;
                            is_moving = true;
                        } else if (can_fly) { // Khi bay, mũi tên trái để di chuyển trái
                            moving_left = true;
                            facing_right = false;
                            is_moving = true;
                        }
                        break;
                    case SDLK_RIGHT: // Di chuyển sang phải
                        if (!hit_by_stungun && !game_over && !can_fly) { // Chỉ di chuyển khi không bị stunned, game over, và không bay
                            moving_right = true;
                            facing_right = true;
                            is_moving = true;
                        } else if (can_fly) { // Khi bay, mũi tên phải để di chuyển phải
                            moving_right = true;
                            facing_right = true;
                            is_moving = true;
                        }
                        break;
                    case SDLK_SPACE: // Nhảy (nút SPACE)
                        if (!is_jumping && !hit_by_stungun && !game_over && !can_fly) { // Chỉ nhảy khi không đang nhảy, không bị stunned, game over, và không bay
                            velocity_y = JUMP_VELOCITY * jump_multiplier; // Áp dụng hệ số nhảy
                            is_jumping = true;

                            // Cập nhật bộ đếm số lần nhảy với hệ số hiện tại
                            jumps_with_multiplier++;

                            // Nếu đã nhảy 2 lần với hệ số này, tạo hệ số mới và reset bộ đếm
                            if (jumps_with_multiplier >= 2) {
                                jump_multiplier = generateRandomJumpMultiplier();
                                jumps_with_multiplier = 0;
                            }
                        }
                        break;
                    case SDLK_UP: // Di chuyển lên khi bay
                        if (can_fly) {
                            velocity_y = -MOVE_SPEED; // Tốc độ di chuyển lên khi bay
                        }
                        // Logic nhảy cũ ở đây đã được chuyển sang SDLK_SPACE
                        break;
                    case SDLK_DOWN: // Di chuyển xuống khi bay
                         if (can_fly) {
                            velocity_y = MOVE_SPEED; // Tốc độ di chuyển xuống khi bay
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
                            game_world_moving = true; // Bắt đầu cuộn lại background màu
                            animation_frame = 0;
                            animation_counter = 0;


                            // Reset các đối tượng trong game
                            stungun.resetAfterCollision();
                            obstacles.reset();

                             // Reset lại hệ số nhảy và bộ đếm khi game restart
                            jump_multiplier = generateRandomJumpMultiplier();
                            jumps_with_multiplier = 0;
                            // Reset trạng thái bay và xóa tất cả power-up
                            can_fly = false;
                            fly_timer = 0; // Reset bộ đếm thời gian bay
                            power_ups.clear();
                            power_up_spawn_timer = 0;
                        }
                        break;
                }
            } else if (event.type == SDL_KEYUP) {
               switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        if (!hit_by_stungun && !game_over || can_fly) { // Dừng di chuyển trái khi nhả phím (kể cả khi bay)
                            moving_left = false;
                            if (!moving_right && ! (can_fly && moving_right)) is_moving = false;
                        }
                        break;
                    case SDLK_RIGHT:
                         if (!hit_by_stungun && !game_over || can_fly) { // Dừng di chuyển phải khi nhả phím (kể cả khi bay)
                            moving_right = false;
                            if (!moving_left && ! (can_fly && moving_left)) is_moving = false;
                        }
                        break;
                     case SDLK_UP: // Dừng di chuyển lên khi bay
                        if (can_fly) {
                            velocity_y = 0;
                        }
                        break;
                    case SDLK_DOWN: // Dừng di chuyển xuống khi bay
                         if (can_fly) {
                            velocity_y = 0;
                        }
                        break;
                }
            }
        }

        // Cập nhật logic game chỉ khi game chưa kết thúc và không bị stunned
        if (!game_over && !hit_by_stungun) {

            // Áp dụng trọng lực nếu không trong trạng thái bay
            if (!can_fly) {
                 velocity_y += GRAVITY;
            } else {
                 // Nếu đang bay, tăng bộ đếm thời gian bay
                 fly_timer++;
                 // Kiểm tra xem đã hết thời gian bay chưa
                 if (fly_timer >= FLY_DURATION_FRAMES) {
                     can_fly = false; // Tắt trạng thái bay
                     fly_timer = 0; // Reset timer
                     // Khi hết trạng thái bay, có thể đặt lại vận tốc y về 0 hoặc giá trị nhỏ
                     // để tránh rơi quá nhanh ngay lập tức, tùy thuộc vào cảm giác game muốn
                     velocity_y = 0; // Ví dụ: đặt vận tốc y về 0
                     // cout << "Fly duration ended. Can Fly: " << can_fly << endl; // Debug
                 }
            }


            // Cập nhật vị trí nhân vật theo vận tốc
            character_y += velocity_y;

            // Giới hạn nhân vật ở trong màn hình và trên mặt đất khi không bay
            if (!can_fly) {
                if (character_y >= GROUND_LEVEL - CHARACTER_HEIGHT) {
                    character_y = GROUND_LEVEL - CHARACTER_HEIGHT;
                    is_jumping = false;
                    velocity_y = 0;
                }
                 // Giới hạn nhân vật không đi quá mép trên khi không bay (tùy chọn)
                if (character_y < 0) {
                    character_y = 0;
                     if (velocity_y < 0) velocity_y = 0; // Ngăn đi lên tiếp nếu chạm đỉnh
                }
            } else { // Khi bay, giới hạn hoàn toàn trong màn hình
                 if (character_y < 0) {
                    character_y = 0;
                    if (velocity_y < 0) velocity_y = 0;
                }
                if (character_y > WINDOW_HEIGHT - CHARACTER_HEIGHT) {
                    character_y = WINDOW_HEIGHT - CHARACTER_HEIGHT;
                     if (velocity_y > 0) velocity_y = 0;
                }
            }


            // Cập nhật vị trí X của nhân vật dựa trên di chuyển trái/phải
             if (moving_left) {
                 character_x -= MOVE_SPEED;
             }
             if (moving_right) {
                 character_x += MOVE_SPEED;
             }

            // Giới hạn nhân vật trong màn hình theo chiều ngang
            if (character_x < 0) character_x = 0;
            if (character_x > WINDOW_WIDTH - CHARACTER_WIDTH) character_x = WINDOW_WIDTH - CHARACTER_WIDTH;


            // Cập nhật stungun
            stungun.update();

            // Kiểm tra va chạm stungun
            if (stungun.isColliding(CHARACTER_WIDTH, CHARACTER_HEIGHT)) {
                hit_by_stungun = true;
                game_over = true; // Kết thúc game khi bị stungun bắn trúng
                game_world_moving = false; // Dừng thế giới game
                 can_fly = false; // Tắt trạng thái bay khi thua
                 fly_timer = 0; // Reset timer bay
            }

            // Cập nhật vật cản
            // Sửa lỗi: Truyền đúng 2 tham số cho Obstacle::update
            obstacles.update(game_world_moving, game_world_moving ? BACKGROUND_SCROLL_SPEED : 0);


            // Kiểm tra va chạm vật cản
             if (obstacles.isColliding(CHARACTER_WIDTH, CHARACTER_HEIGHT)) {
                game_over = true; // Kết thúc game khi va chạm vật cản
                game_world_moving = false; // Dừng thế giới game
                 can_fly = false; // Tắt trạng thái bay khi thua
                 fly_timer = 0; // Reset timer bay
            }


            // --- Logic Power-up ---

            // Logic spawn power-up
            power_up_spawn_timer++;
            if (power_up_spawn_timer >= POWER_UP_SPAWN_INTERVAL) {
                if (rand() % POWER_UP_SPAWN_CHANCE == 0) { // 1/20 cơ hội spawn
                    PowerUp new_power_up;
                    new_power_up.rect.w = 150; // Kích thước power-up (điều chỉnh nếu cần)
                    new_power_up.rect.h = 150; // Kích thước power-up
                    new_power_up.rect.x = WINDOW_WIDTH; // Spawn ở rìa phải
                    // Spawn ngẫu nhiên theo chiều dọc, tránh quá sát mép trên/dưới
                    new_power_up.rect.y = rand() % (WINDOW_HEIGHT - new_power_up.rect.h - 100) + 50;
                    new_power_up.active = true;
                    power_ups.push_back(new_power_up);
                     // cout << "Spawned Power-up at y: " << new_power_up.rect.y << endl; // Debug
                }
                power_up_spawn_timer = 0; // Reset timer sau mỗi lần thử spawn
            }

            // Cập nhật vị trí và kiểm tra va chạm power-up
            for (auto it = power_ups.begin(); it != power_ups.end(); ) {
                if (it->active) {
                     // Di chuyển sang trái cùng tốc độ cuộn background
                    if (game_world_moving) {
                        it->rect.x -= BACKGROUND_SCROLL_SPEED;
                    }

                    // Kiểm tra va chạm với nhân vật
                     SDL_Rect character_rect_collision = {character_x, character_y, CHARACTER_WIDTH, CHARACTER_HEIGHT};
                     if (SDL_HasIntersection(&character_rect_collision, &it->rect)) {
                         can_fly = true; // Kích hoạt trạng thái bay
                         fly_timer = 0; // Bắt đầu đếm thời gian bay
                         it->active = false; // Đánh dấu để xóa vật phẩm
                         // cout << "Collected Power-up! Can Fly: " << can_fly << endl; // Debug
                     }

                    // Xóa vật phẩm nếu đi ra khỏi màn hình hoặc đã được thu thập
                    if (it->rect.x + it->rect.w < 0 || !it->active) {
                        it = power_ups.erase(it);
                    } else {
                        ++it;
                    }
                } else {
                     it = power_ups.erase(it); // Xóa nếu đã được đánh dấu là không active
                }
            }


            // Cập nhật animation nhân vật khi di chuyển (chỉ khi không bay)
            if (is_moving && !can_fly) {
                animation_counter++;
                if (animation_counter >= ANIMATION_SPEED) {
                    animation_counter = 0;
                    animation_frame = (animation_frame + 1) % 2; // Chuyển giữa frame 1 và 2
                }
            } else if (!can_fly){
                 animation_frame = 0; // Về frame đứng yên khi không di chuyển và không bay
            }

            // Khi đang bay, animation có thể khác hoặc đứng yên tùy thiết kế game
            // Hiện tại giữ animation frame cuối cùng trước khi bay hoặc đứng yên ở frame 0
            if (can_fly) {
                 // Có thể thêm logic animation riêng cho trạng thái bay ở đây
                 // Ví dụ: animation_frame = animation_bay;
                 // is_moving = true; // Có thể coi là luôn di chuyển khi bay
            }


        } else if (hit_by_stungun) {
            // Logic khi bị stungun bắn trúng (ví dụ: hiệu ứng stunned)
            stunned_timer++;
            if (stunned_timer >= 60) { // Ví dụ: bị stunned in 1 second (60 frames)
                hit_by_stungun = false; // End stunned
                stunned_timer = 0;
                // Add other recovery logic here if needed
            }
             // The game is still over immediately after being hit, so this stunned logic might not be needed if the game ends right away
             // If you want the game to end after the stunned effect wears off, you need to adjust the game_over logic
        }


        // Xóa màn hình
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Vẽ background (vẫn dùng texture)
         moving_background(game_world_moving ? BACKGROUND_SCROLL_SPEED : 0, bg1_position_x, bg2_position_x, renderer, background1, background2, WINDOW_WIDTH, WINDOW_HEIGHT);


        // Vẽ stungun (vẫn dùng texture)
        stungun.render();

        // Vẽ vật cản (vẫn dùng texture)
        obstacles.render();

        // Vẽ vật phẩm power-up (vẫn dùng texture)
        if (power_up_texture) {
             for (const auto& pu : power_ups) {
                if (pu.active) {
                    SDL_RenderCopy(renderer, power_up_texture, NULL, &pu.rect);
                }
             }
        }


        // Vẽ nhân vật (vẫn dùng texture)
        SDL_Rect character_rect = {character_x, character_y, CHARACTER_WIDTH, CHARACTER_HEIGHT};
        SDL_Texture* current_character_texture = nullptr;

        // Chọn texture dựa trên hướng nhìn và trạng thái (nhảy hoặc di chuyển hoặc bay)
        if (is_jumping && !can_fly) { // Animation nhảy (chỉ khi không bay)
            current_character_texture = facing_right ? horseman_jump_right : horseman_jump_left;
        } else if (is_moving && !can_fly) { // Animation di chuyển (chỉ khi không bay)
            if (facing_right) {
                current_character_texture = (animation_frame == 0) ? horseman_right_1 : horseman_right_2;
            } else {
                current_character_texture = (animation_frame == 0) ? horseman_left_1 : horseman_left_2;
            }
        } else if (can_fly) { // Animation khi bay (có thể sử dụng animation riêng hoặc animation đứng yên)
             current_character_texture = facing_right ? horseman_right : horseman_left; // Ví dụ: dùng animation đứng yên khi bay
             // Nếu muốn animation bay riêng, thêm texture mới và sử dụng ở đây
        }
         else { // Animation đứng yên
            current_character_texture = facing_right ? horseman_right : horseman_left;
        }

        // Vẽ nhân vật (trừ khi bị stunned - có thể thêm hiệu ứng nhấp nháy)
        // if (!hit_by_stungun || stunned_timer % 10 < 5 ) {  // Tạo hiệu ứng nhấp nháy bằng cách không vẽ nhân vật ở một số frame
             SDL_RenderCopy(renderer, current_character_texture, NULL, &character_rect);
        // }


        // Hiển thị thông báo khi thua cuộc - Chỉ vẽ lớp phủ mờ, bỏ chữ
        if (game_over) {
            // Vẽ lớp phủ mờ
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 192); // Màu đen, mờ đục
            SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
            SDL_RenderFillRect(renderer, &overlay);
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
    if(power_up_texture) SDL_DestroyTexture(power_up_texture); // Giải phóng texture power-up

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}