#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <iostream>

using namespace std;

// Kích thước cửa sổ
const int WINDOW_WIDTH = 1400;
const int WINDOW_HEIGHT = 750;

// Kích thước nhân vật (có thể điều chỉnh tùy theo hình ảnh thực tế)
const int CHARACTER_WIDTH = 100;
const int CHARACTER_HEIGHT = 100;

// Thông số game
const int GROUND_LEVEL = WINDOW_HEIGHT - 100; // Mặt đất cách đáy 100 pixel
const int JUMP_VELOCITY = -20; // Tốc độ nhảy ban đầu (âm để đi lên)
const int GRAVITY = 1; // Gia tốc trọng trường

// Hàm tải texture với nền trong suốt
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* file) {
    SDL_Surface* surface = IMG_Load(file);
    if (!surface) {
        cout << "Không thể tải surface: " << IMG_GetError() << endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        cout << "Không thể tạo texture: " << SDL_GetError() << endl;
    }
    return texture;
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
    SDL_Texture* horseman_right = load_texture(renderer, "horseman_right.png");
    SDL_Texture* horseman_left = load_texture(renderer, "horseman_left.png");
    if (!horseman_right || !horseman_left) {
        cout << "Không thể tải hình ảnh nhân vật!" << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Khởi tạo trạng thái nhân vật
    int character_x = WINDOW_WIDTH / 2 - CHARACTER_WIDTH / 2; // Vị trí ban đầu giữa màn hình
    int character_y = GROUND_LEVEL - CHARACTER_HEIGHT; // Đặt trên mặt đất
    int velocity_y = 0; // Vận tốc theo trục y (dùng cho nhảy)
    bool is_jumping = false; // Trạng thái nhảy
    bool facing_right = true; // Hướng ban đầu: quay phải

    // Vòng lặp chính
    bool running = true;
    SDL_Event event;
    while (running) {
        // Xử lý sự kiện bàn phím
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT: // Di chuyển sang trái
                        character_x -= 10;
                        facing_right = false;
                        break;
                    case SDLK_RIGHT: // Di chuyển sang phải
                        character_x += 10;
                        facing_right = true;
                        break;
                    case SDLK_SPACE: // Nhảy
                        if (!is_jumping) {
                            velocity_y = JUMP_VELOCITY;
                            is_jumping = true;
                        }
                        break;
                }
            }
        }

        // Cập nhật vị trí nhân vật khi nhảy
        if (is_jumping) {
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

        // Xóa màn hình
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu đen
        SDL_RenderClear(renderer);

        // Vẽ nhân vật
        SDL_Rect character_rect = {character_x, character_y, CHARACTER_WIDTH, CHARACTER_HEIGHT};
        if (facing_right) {
            SDL_RenderCopy(renderer, horseman_right, NULL, &character_rect);
        } else {
            SDL_RenderCopy(renderer, horseman_left, NULL, &character_rect);
        }

        // Hiển thị lên màn hình
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Giới hạn khoảng 60 FPS
    }

    // Giải phóng tài nguyên
    SDL_DestroyTexture(horseman_right);
    SDL_DestroyTexture(horseman_left);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}