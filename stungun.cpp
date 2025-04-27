#include "stungun.h"
#include "utils.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

StunGun::StunGun(SDL_Renderer* renderer, int* target_x, int* target_y, bool* is_jumping_ptr)
: IDLE_THRESHOLD(120), // 2 seconds at 60 FPS
MOVEMENT_THRESHOLD(30) // 30 pixels of movement considered significant
{
    this->renderer = renderer;
    this->target_x = target_x;
    this->target_y = target_y;
    this->is_jumping_ptr = is_jumping_ptr; // Gán con trỏ trạng thái nhảy

    // Initialize tracking variables
    this->last_target_x = *target_x;
    this->last_target_y = *target_y;
    this->idle_timer = 0;
    this->is_active = false;
    // Kích thước màn hình
    const int WINDOW_WIDTH = 1400;
    const int WINDOW_HEIGHT = 750;
    // Kích thước stungun lớn hơn (90% chiều cao màn hình)
    this->height = WINDOW_HEIGHT * 0.9;
    this->width = this->height;
    // Đặt vị trí ban đầu ở giữa bên trái màn hình, nhưng chưa xuất hiện
    x = -width * 2; // Đặt xa hơn một chút để ban đầu không xuất hiện
    y = (WINDOW_HEIGHT - height) / 2; // Ở giữa chiều cao màn hình
    // Tốc độ di chuyển cố định
    this->speed = 3; // Tốc độ cố định, điều chỉnh theo nhu cầu
    // Thiết lập animation
    this->current_frame = 0;
    this->animation_counter = 0;
    this->animation_speed = 10;
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
if (!stungun_1 || !stungun_2 ) {
std::cout << "Không thể tải hình ảnh stungun!" << std::endl;
return false;
}
// Thêm vào vector
textures.push_back(stungun_1);
textures.push_back(stungun_2);
return true;
}

// Đặt lại vị trí luôn từ bên trái màn hình
void StunGun::resetPosition() {
// Reset position off-screen
const int WINDOW_HEIGHT = 750;
x = -width * 2;
y = (WINDOW_HEIGHT - height) / 2;
// Also reset idle tracking
idle_timer = 0;
is_active = false;
last_target_x = *target_x;
last_target_y = *target_y;
}

// Cập nhật vị trí
void StunGun::update() {
    // Kiểm tra nếu nhân vật đang nhảy, vô hiệu hóa stungun ngay lập tức
    if (*is_jumping_ptr) {
        if (is_active) {
            is_active = false;
            resetPosition(); // Di chuyển stungun ra khỏi màn hình
        }
        // Không thực hiện các logic cập nhật khác khi nhân vật đang nhảy
        idle_timer = 0; // Reset idle timer khi nhảy
        last_target_x = *target_x; // Cập nhật vị trí cuối cùng
        last_target_y = *target_y;
        return; // Kết thúc hàm update sớm
    }

    // Logic để kích hoạt stungun nếu nhân vật đứng yên (không bị ảnh hưởng bởi di chuyển)
    // Stungun sẽ kích hoạt sau khi nhân vật đứng yên đủ thời gian (IDLE_THRESHOLD).
    idle_timer++;
    if (idle_timer >= IDLE_THRESHOLD && !is_active) {
        is_active = true;
        const int WINDOW_HEIGHT = 750; // Lấy lại kích thước cửa sổ
        x = -width; // Đặt stungun ở rìa trái màn hình để bắt đầu di chuyển vào
        y = (WINDOW_HEIGHT - height) / 2; // Ở giữa chiều cao màn hình
    }

    // Cập nhật vị trí cuối cùng của mục tiêu để sử dụng cho idle_timer trong lần cập nhật tiếp theo.
    // Việc di chuyển bình thường của nhân vật sẽ không vô hiệu hóa stungun nữa.
    last_target_x = *target_x;
    last_target_y = *target_y;


    // Chỉ cập nhật vị trí và animation stungun nếu nó đang hoạt động
    if (is_active) {
        // Lưu vị trí hiện tại cho animation
        int prev_x = x;
        int prev_y = y;

        // Tính toán hướng di chuyển đến mục tiêu (tâm nhân vật)
        int target_center_x = *target_x + width/4; // Ước lượng tâm nhân vật
        int target_center_y = *target_y + height/4; // Ước lượng tâm nhân vật
        int stungun_center_x = x + width/2; // Tâm stungun
        int stungun_center_y = y + height/2; // Tâm stungun

        // Tính toán vector hướng
        float dir_x = target_center_x - stungun_center_x;
        float dir_y = target_center_y - stungun_center_y;

        // Chuẩn hóa vector hướng và di chuyển theo tốc độ cố định
        float length = sqrt(dir_x*dir_x + dir_y*dir_y);

        // Chỉ di chuyển nếu chưa đến gần mục tiêu
        if (length > 10) { // Ngưỡng khoảng cách nhỏ để tránh rung lắc khi ở rất gần mục tiêu
            // Chuẩn hóa vector
            dir_x /= length;
            dir_y /= length;
            // Di chuyển với tốc độ cố định
            x += static_cast<int>(dir_x * speed);
            y += static_cast<int>(dir_y * speed);
        }

        // Cập nhật animation nếu vị trí thay đổi
        if (prev_x != x || prev_y != y) {
            animation_counter++;
            if (animation_counter >= animation_speed) {
                animation_counter = 0;
                current_frame = (current_frame + 1) % textures.size(); // Chuyển frame animation
            }
        }
    }
}

// Vẽ stungun
void StunGun::render() {
// Only render if stungun is active or partially on screen
if (is_active || x > -width) {
SDL_Rect stungun_rect = {x, y, width, height};
SDL_RenderCopy(renderer, textures[current_frame], NULL, &stungun_rect);
}
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
// Add optional cooldown period before stungun can activate again
idle_timer = -120; // Negative value creates a cooldown (2 seconds at 60 FPS)
}