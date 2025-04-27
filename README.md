    MỘT VÀI NHẬN XÉT CỦA NHỮNG NGƯỜI TRẢI NGHIỆM 🐧:
- Ubisoft: Đây đúng là một kiệt tác, chúng tôi muốn mua game của bạn, 1 tỉ đô đủ không ?
- Microsoft: Hãy về làm cho chúng tôi, we need you.
- Hidetaka Miyazaki (cha đẻ Elden Ring): Sau khi chơi game này, tôi cảm thấy hổ thẹn về tác phẩm của mình, nếu game này ra sớm vài năm, Elden Ring sẽ mất giải Game of the year.
- Rolnado: Người sáng tạo ra game này mới thực sự là GOAT, i love you, you're my idol. 💋💋💋
- Jack J97: Game này còn cháy hơn cả lửa chùa. 🔥🔥🔥
    
    Được đánh giá: Messi / 10 bởi người hâm mộ
    
    GIỚI THIỆU GAME

1. Nội dung
- Chạy để không bị chích điện ᯓ🏃‍➡️
- Nhảy để tránh địch

2. Các đối tượng trong game
- Nhân vật chính 
![image alt](https://github.com/haihoang18/Escape-from-Cambodia/blob/7ff995f73a7dc3fd5f63cdab8a90ff35c44ac391/character/horseman_jump_right.png)
- Địch
![image alt](https://github.com/haihoang18/Escape-from-Cambodia/blob/7ff995f73a7dc3fd5f63cdab8a90ff35c44ac391/background/tungtungtungsahur.png)
- Bụi tiên
![image alt](https://github.com/haihoang18/Escape-from-Cambodia/blob/7ff995f73a7dc3fd5f63cdab8a90ff35c44ac391/background/bui_tien.png)
- Súng điện
![image alt](https://github.com/haihoang18/Escape-from-Cambodia/blob/7ff995f73a7dc3fd5f63cdab8a90ff35c44ac391/stungun/stungun_2.png)

3. Hướng dẫn chơi
- Trạng thái bình thường: dùng space để nhảy, các nút mũi tên phải, trái để di chuyển.
- Trạng thái hít bụi tiên: dùng tất cả các nút mũi tên để di chuyển tự do trong background, không chịu tác dụng của trọng lực

4. Cơ chế
- Nhân vật chính chạm vào địch, súng điện là thua game 👎👎👎
- Địch sẽ xuất hiện cả bên dưới và bên trên màn hình 
- Sau mỗi 2 lần nhảy sẽ random lực nhảy, có 3 trường hợp:
    * 0,5 lực gốc
    *   1 lực gốc
    *   2 lực gốc
- Nhặt bụi tiên sẽ bay được 💨
- Khi di chuyển quá chậm sẽ bị súng điện đuổi kịp 🔌⚡💥😵

5. Về đồ họa của game
- Nhân vật chính: Death road to Canada, chỉnh sửa lại bằng piskel
- Địch: Tung tung tung sahur
- Background: Sora
- Animation: sử dụng bằng piskel
- Súng điện, bụi tiên: tạo bằng PixelMe

6. Code
- main.cpp:  xử lý đầu vào từ người dùng (di chuyển, nhảy, khởi động lại game), cập nhật vị trí và trạng thái của nhân vật, kiểm tra va chạm với chướng ngại vật và "stungun", quản lý vật phẩm tăng sức mạnh (power-up) cho phép bay, cập nhật trạng thái game (thua cuộc, bị choáng) và điều khiển quá trình render (vẽ) các đối tượng lên màn hình.
- background.cpp và background.h: xử lý việc cuộn nền (background) của trò chơi, tạo cảm giác chuyển động liên tục. Hàm chính trong đây là moving_background, có nhiệm vụ cập nhật vị trí của hai lớp nền để chúng di chuyển và lặp lại.
- obstacle.cpp và obstacle.h: Các tệp này định nghĩa lớp Obstacle, quản lý các chướng ngại vật trong game. Lớp này có các phương thức để tạo (spawn) chướng ngại vật ở cả phía trên và dưới màn hình, cập nhật vị trí của chúng khi màn hình cuộn, vẽ chúng và kiểm tra va chạm với nhân vật. Lớp Obstacle cũng bao gồm logic để tăng tốc độ di chuyển của chướng ngại vật theo thời gian.
- stungun.cpp và stungun.h: Các tệp này định nghĩa lớp StunGun, biểu diễn một đối tượng "stungun" có khả năng theo dõi nhân vật. Lớp này bao gồm logic để kích hoạt "stungun" khi nhân vật đứng yên trong một khoảng thời gian, di chuyển nó về phía nhân vật, xử lý animation (hoạt ảnh), vẽ nó lên màn hình và kiểm tra va chạm với nhân vật. Nó cũng có chức năng đặt lại vị trí sau khi va chạm.
- utils.cpp và utils.h: Các tệp này chứa các hàm tiện ích chung được sử dụng trong dự án. Các hàm này bao gồm khởi tạo thư viện SDL và SDL_image, tải texture (hình ảnh) từ tệp (bao gồm cả tải với nền trong suốt) và có thể có các hàm khác liên quan đến cập nhật animation cho nhân vật.
Google Drive: https://drive.google.com/file/d/1QfriavwrbZT8mvmzOxZjqquefHW2JwJp/view?usp=share_link

![image alt] (https://github.com/haihoang18/Escape-from-Cambodia/blob/78fe809e68653bd736666de89678feb1cee31789/background/background1.png)