#include "stungun.h"
#include "utils.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

StunGun::StunGun(SDL_Renderer* renderer, int* target_x, int* target_y)
: IDLE_THRESHOLD(120), // 2 seconds at 60 FPS
MOVEMENT_THRESHOLD(30) // 30 pixels of movement considered significant
{
this->renderer = renderer;
this->target_x = target_x;
this->target_y = target_y;
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
// First, check if character has moved significantly
int dx = abs(*target_x - last_target_x);
int dy = abs(*target_y - last_target_y);
int distance_moved = sqrt(dx*dx + dy*dy);
if (distance_moved > MOVEMENT_THRESHOLD) {
// Character moved significantly, reset timer
idle_timer = 0;
last_target_x = *target_x;
last_target_y = *target_y;
// If stungun was active, deactivate it
if (is_active) {
is_active = false;
resetPosition(); // Move stungun off-screen
}
} else {
// Character hasn't moved significantly, increment timer
idle_timer++;
// If idle for long enough, activate stungun
if (idle_timer >= IDLE_THRESHOLD && !is_active) {
is_active = true;
// Position stungun to enter from left side
const int WINDOW_HEIGHT = 750;
x = -width;
y = (WINDOW_HEIGHT - height) / 2;
}
}
// Only update stungun position if it's active
if (is_active) {
// Store current position for animation
int prev_x = x;
int prev_y = y;
// Calculate direction to target
int target_center_x = *target_x + width/4;
int target_center_y = *target_y + height/4;
int stungun_center_x = x + width/2;
int stungun_center_y = y + height/2;
// Calculate direction vector
float dir_x = target_center_x - stungun_center_x;
float dir_y = target_center_y - stungun_center_y;
// Normalize direction
float length = sqrt(dir_x*dir_x + dir_y*dir_y);
// Only move if not already at target
if (length > 10) {
// Normalize vector
dir_x /= length;
dir_y /= length;
// Move at fixed speed
x += static_cast<int>(dir_x * speed);
y += static_cast<int>(dir_y * speed);
}
// Update animation if position changed
if (prev_x != x || prev_y != y) {
animation_counter++;
if (animation_counter >= animation_speed) {
animation_counter = 0;
current_frame = (current_frame + 1) % textures.size();
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