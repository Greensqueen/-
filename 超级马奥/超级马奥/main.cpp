#include <graphics.h>
#include <conio.h>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float GRAVITY = 0.5f;
const float JUMP_FORCE = -12.0f;

// 音乐管理类（支持MP3格式）
class MusicManager {
private:
    bool isPlaying;
    char musicPath[MAX_PATH];

public:
    MusicManager() : isPlaying(false) {
        // 默认背景音乐文件名（MP3格式）
        strcpy_s(musicPath, MAX_PATH, "background.mp3");
    }

    void setMusicPath(const char* path) {
        if (path) strcpy_s(musicPath, MAX_PATH, path);
    }

    // 播放背景音乐（循环播放MP3）
    void play() {
        if (!isPlaying) {
            mciSendStringA("close bg_music", NULL, 0, NULL); // 关闭可能的残留音乐
            char cmd[256];
            // 关键：添加type mpegvideo以支持MP3格式
            sprintf_s(cmd, "open \"%s\" type mpegvideo alias bg_music", musicPath);
            mciSendStringA(cmd, NULL, 0, NULL);
            mciSendStringA("play bg_music repeat", NULL, 0, NULL); // 循环播放
            isPlaying = true;
        }
    }

    // 停止音乐（游戏结束时调用）
    void stop() {
        if (isPlaying) {
            mciSendStringA("stop bg_music", NULL, 0, NULL);
            mciSendStringA("close bg_music", NULL, 0, NULL);
            isPlaying = false;
        }
    }
};

// 玩家类
class Player {
public:
    float x, y;
    float velocityX, velocityY;
    int width, height;
    int lives;
    int coins;
    int score;
    bool isJumping;
    bool isFacingRight;

    Player() {
        x = 100;
        y = WINDOW_HEIGHT - 150;
        velocityX = velocityY = 0;
        width = 24;
        height = 32;
        lives = 3;
        coins = 0;
        score = 0;
        isJumping = false;
        isFacingRight = true;
    }

    void update() {
        x += velocityX;
        velocityY += GRAVITY;
        y += velocityY;

        // 地面碰撞
        if (y > WINDOW_HEIGHT - 50) {
            y = WINDOW_HEIGHT - 50;
            velocityY = 0;
            isJumping = false;
        }

        // 边界限制
        if (x < 0) x = 0;
        if (x > WINDOW_WIDTH - width) x = WINDOW_WIDTH - width;
    }

    void jump() {
        if (!isJumping) {
            velocityY = JUMP_FORCE;
            isJumping = true;
        }
    }

    void draw() {
        setfillcolor(RED);
        solidroundrect(static_cast<int>(x), static_cast<int>(y),
            static_cast<int>(x + width), static_cast<int>(y + height), 5, 5);

        // 眼睛
        setfillcolor(WHITE);
        if (isFacingRight) {
            solidcircle(static_cast<int>(x + width - 8), static_cast<int>(y + 10), 4);
        }
        else {
            solidcircle(static_cast<int>(x + 8), static_cast<int>(y + 10), 4);
        }
    }
};

// 敌人类
class Enemy {
public:
    float x, y;
    float velocityX;
    int width, height;
    bool isAlive;

    Enemy(float startX, float startY) {
        x = startX;
        y = startY;
        velocityX = 2.0f;
        width = 28;
        height = 28;
        isAlive = true;
    }

    void update() {
        if (!isAlive) return;
        x += velocityX;
        // 巡逻边界
        if (x > WINDOW_WIDTH - 100 || x < 50) {
            velocityX = -velocityX;
        }
    }

    void draw() {
        if (!isAlive) return;
        setfillcolor(GREEN);
        solidroundrect(static_cast<int>(x), static_cast<int>(y),
            static_cast<int>(x + width), static_cast<int>(y + height), 8, 8);

        // 眼睛
        setfillcolor(BLACK);
        if (velocityX > 0) {
            solidcircle(static_cast<int>(x + width - 8), static_cast<int>(y + 10), 3);
        }
        else {
            solidcircle(static_cast<int>(x + 8), static_cast<int>(y + 10), 3);
        }
    }
};

// 金币类
class Coin {
public:
    float x, y;
    int width, height;
    bool isCollected;

    Coin(float posX, float posY) {
        x = posX;
        y = posY;
        width = 16;
        height = 16;
        isCollected = false;
    }

    void draw() {
        if (isCollected) return;
        setfillcolor(YELLOW);
        solidcircle(static_cast<int>(x + width / 2), static_cast<int>(y + height / 2), width / 2);
        setfillcolor(RGB(255, 215, 0));
        solidcircle(static_cast<int>(x + width / 2), static_cast<int>(y + height / 2), width / 3);
    }
};

// 平台类
class Platform {
public:
    float x, y;
    int width, height;
    bool isMoving;
    float moveRange;
    float moveSpeed;
    float startX;

    Platform(float posX, float posY, int w, bool moving = false, float range = 0, float speed = 1) {
        x = posX;
        y = posY;
        width = w;
        height = 12;
        isMoving = moving;
        moveRange = range;
        moveSpeed = speed;
        startX = posX;
    }

    void update() {
        if (isMoving) {
            x = startX + sin(static_cast<float>(clock()) * 0.001f * moveSpeed) * moveRange;
        }
    }

    void draw() {
        setfillcolor(RGB(150, 75, 0));
        solidrectangle(static_cast<int>(x), static_cast<int>(y),
            static_cast<int>(x + width), static_cast<int>(y + height));
    }
};

// 终点旗帜类
class Flag {
public:
    float x, y;
    int width, height;

    Flag(float posX, float posY) {
        x = posX;
        y = posY;
        width = 10;
        height = 60;
    }

    void draw() {
        // 旗杆
        setfillcolor(RGB(128, 128, 128));
        solidrectangle(static_cast<int>(x), static_cast<int>(y),
            static_cast<int>(x + width), static_cast<int>(y + height));

        // 旗帜
        setfillcolor(RED);
        POINT points[] = {
            {static_cast<int>(x + width), static_cast<int>(y + 10)},
            {static_cast<int>(x + width + 30), static_cast<int>(y + 20)},
            {static_cast<int>(x + width), static_cast<int>(y + 30)}
        };
        solidpolygon(points, 3);
    }
};

// 游戏状态
enum GameState {
    START_SCREEN,
    PLAYING,
    LEVEL_COMPLETE,
    GAME_OVER
};

// 生成随机关卡
void generateRandomLevel(std::vector<Platform>& platforms,
    std::vector<Enemy>& enemies,
    std::vector<Coin>& coins,
    Flag& flag) {
    platforms.clear();
    enemies.clear();
    coins.clear();

    // 地面
    platforms.push_back(Platform(0, WINDOW_HEIGHT - 50, WINDOW_WIDTH));

    // 终点旗帜
    float flagY = WINDOW_HEIGHT - 180 - (rand() % 150);
    flag = Flag(WINDOW_WIDTH - 100, flagY);

    // 空中平台
    int numPlatforms = 5 + rand() % 5;
    float lastX = 50;
    float lastY = WINDOW_HEIGHT - 150;

    for (int i = 0; i < numPlatforms; i++) {
        float x = lastX + 70 + rand() % 100;
        float y = lastY - 100 + rand() % 160;
        int width = 60 + rand() % 100;
        bool isMoving = (rand() % 4 == 0);
        float moveRange = isMoving ? (20 + rand() % 60) : 0;
        float moveSpeed = isMoving ? (0.3f + (rand() % 10) * 0.1f) : 0;

        // 边界限制
        if (x < 0) x = 0;
        if (x > WINDOW_WIDTH - 200) x = WINDOW_WIDTH - 200;
        if (y < 150) y = 150;
        if (y > WINDOW_HEIGHT - 100) y = WINDOW_HEIGHT - 100;

        platforms.push_back(Platform(x, y, width, isMoving, moveRange, moveSpeed));

        // 添加金币
        if (rand() % 4 != 0) {
            float coinX = x + 10 + rand() % (width - 30);
            float coinY = y - 30;
            coins.push_back(Coin(coinX, coinY));
        }

        // 添加敌人
        if (rand() % 2 == 0) {
            float enemyX = x + 10 + rand() % (width - 40);
            float enemyY = y - 30;
            enemies.push_back(Enemy(enemyX, enemyY));
        }

        lastX = x;
        lastY = y;
    }

    // 终点平台
    platforms.push_back(Platform(WINDOW_WIDTH - 150, flagY + 70, 100));

    // 额外金币
    int extraCoins = rand() % 5;
    for (int i = 0; i < extraCoins; i++) {
        int platformIndex = 1 + rand() % (platforms.size() - 1);
        float x = platforms[platformIndex].x + 10 + rand() % (platforms[platformIndex].width - 30);
        float y = platforms[platformIndex].y - 30;
        coins.push_back(Coin(x, y));
    }

    // 额外敌人
    int extraEnemies = 1 + rand() % 3;
    for (int i = 0; i < extraEnemies; i++) {
        int platformIndex = 1 + rand() % (platforms.size() - 1);
        float x = platforms[platformIndex].x + 10 + rand() % (platforms[platformIndex].width - 40);
        float y = platforms[platformIndex].y - 30;
        enemies.push_back(Enemy(x, y));
    }
}

// 主函数
int main() {
    // 初始化图形窗口
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT, EW_SHOWCONSOLE | EW_DBLCLKS);
    BeginBatchDraw();
    setbkcolor(RGB(107, 140, 255));
    cleardevice();

    // 初始化随机种子
    srand(static_cast<unsigned int>(time(nullptr)));

    // 初始化音乐管理器（支持MP3）
    MusicManager musicManager;
    

    // 游戏对象
    GameState gameState = START_SCREEN;
    Player player;
    std::vector<Platform> platforms;
    std::vector<Enemy> enemies;
    std::vector<Coin> coins;
    Flag flag(WINDOW_WIDTH - 100, WINDOW_HEIGHT - 110);
    generateRandomLevel(platforms, enemies, coins, flag);

    // 游戏计时
    int gameTime = 120;
    time_t startTime = time(nullptr);
    int elapsed = 0;

    // 播放背景音乐
    musicManager.play();

    // 游戏主循环
    while (true) {
        // 按键处理
        if (_kbhit()) {
            char key = _getch();
            if (gameState == START_SCREEN && key == ' ') {
                gameState = PLAYING;
                startTime = time(nullptr);
            }
            if (gameState == LEVEL_COMPLETE && key == ' ') {
                generateRandomLevel(platforms, enemies, coins, flag);
                player = Player();
                gameState = PLAYING;
                startTime = time(nullptr);
            }
            if (gameState == GAME_OVER && key == ' ') {
                generateRandomLevel(platforms, enemies, coins, flag);
                player = Player();
                gameState = PLAYING;
                startTime = time(nullptr);
            }
        }

        cleardevice();

        // 游戏状态逻辑
        switch (gameState) {
        case START_SCREEN:
            // 游戏标题 - 使用大字体和金色
            settextstyle(100, 0, _T("微软雅黑"));
            settextcolor(RGB(255, 215, 0));  // 金色
            outtextxy(200, 120, _T("!超级马奥!"));

            // 副标题和说明文字
            settextstyle(30, 0, _T("微软雅黑"));
            settextcolor(RGB(255, 255, 200));  // 柔和白色
            outtextxy(300, 230, _T("空格键开始"));

            // 操作说明
            settextstyle(25, 0, _T("微软雅黑"));
            settextcolor(RGB(200, 255, 255));  // 浅蓝色
            outtextxy(200, 320, _T("方向键移动，空格跳跃"));

            // 得分说明 - 使用不同颜色区分
            settextstyle(25, 0, _T("微软雅黑"));
            settextcolor(YELLOW);  // 金币颜色
            outtextxy(200, 360, _T("收集金币(+2分)"));

            settextstyle(25, 0, _T("微软雅黑"));
            settextcolor(GREEN);  // 敌人颜色
            outtextxy(200, 400, _T("消灭敌人(+5分)"));

            settextstyle(22, 0, _T("微软雅黑"));
            settextcolor(RED);  // 旗帜颜色
            outtextxy(200, 440, _T("到达终点旗帜获胜"));
            break;

        case PLAYING:
            // 玩家控制
            if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
                player.velocityX = -5;
                player.isFacingRight = false;
            }
            else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
                player.velocityX = 5;
                player.isFacingRight = true;
            }
            else {
                player.velocityX = 0;
            }
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                player.jump();
            }

            // 更新玩家
            player.update();

            // 平台碰撞
            for (auto& platform : platforms) {
                platform.update();
                if (player.y + player.height > platform.y &&
                    player.y < platform.y + platform.height &&
                    player.x + player.width > platform.x &&
                    player.x < platform.x + platform.width &&
                    player.velocityY > 0) {
                    player.y = platform.y - player.height;
                    player.velocityY = 0;
                    player.isJumping = false;
                }
            }

            // 敌人碰撞
            for (auto& enemy : enemies) {
                enemy.update();
                if (enemy.isAlive &&
                    player.x + player.width > enemy.x &&
                    player.x < enemy.x + enemy.width &&
                    player.y + player.height > enemy.y &&
                    player.y < enemy.y + enemy.height) {
                    if (player.velocityY > 0 && player.y + player.height < enemy.y + enemy.height / 2) {
                        enemy.isAlive = false;
                        player.velocityY = JUMP_FORCE / 2;
                        player.score += 5; // 消灭敌人+5分
                    }
                    else {
                        player.lives--;
                        player.x = 100;
                        player.y = WINDOW_HEIGHT - 150;
                        if (player.lives <= 0) {
                            gameState = GAME_OVER;
                        }
                    }
                }
            }

            // 金币收集
            for (auto& coin : coins) {
                if (!coin.isCollected &&
                    player.x + player.width > coin.x &&
                    player.x < coin.x + coin.width &&
                    player.y + player.height > coin.y &&
                    player.y < coin.y + coin.height) {
                    coin.isCollected = true;
                    player.coins++;
                    player.score += 2; // 收集金币+2分
                }
            }

            // 终点检测
            if (player.x + player.width > flag.x &&
                player.x < flag.x + flag.width &&
                player.y + player.height > flag.y &&
                player.y < flag.y + flag.height) {
                gameState = LEVEL_COMPLETE;
            }

            // 时间检测
            elapsed = static_cast<int>(time(nullptr) - startTime);
            if (elapsed >= gameTime) {
                gameState = GAME_OVER;
            }

            // 绘制游戏元素
            setfillcolor(RGB(107, 140, 255));
            solidrectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
            for (auto& platform : platforms) platform.draw();
            for (auto& enemy : enemies) enemy.draw();
            for (auto& coin : coins) coin.draw();
            flag.draw();
            player.draw();

            // 绘制UI
            settextstyle(20, 0, _T("Arial"));
            settextcolor(WHITE);
            outtextxy(20, 20, _T("生命:"));
            for (int i = 0; i < player.lives; i++) {
                setfillcolor(RED);
                solidcircle(100 + i * 25, 30, 8);
            }
            outtextxy(20, 50, _T("金币:"));
            wchar_t coinText[20];
            swprintf_s(coinText, L"%d", player.coins);
            outtextxy(100, 50, coinText);
            outtextxy(20, 80, _T("积分:"));
            wchar_t scoreText[20];
            swprintf_s(scoreText, L"%d", player.score);
            outtextxy(100, 80, scoreText);
            outtextxy(WINDOW_WIDTH - 150, 20, _T("时间:"));
            wchar_t timeText[20];
            swprintf_s(timeText, L"%d", gameTime - elapsed);
            outtextxy(WINDOW_WIDTH - 70, 20, timeText);
            break;

        case LEVEL_COMPLETE:
            settextstyle(100, 0, _T("Arial"));
            settextcolor(YELLOW);
            outtextxy(250, 100, _T("关卡完成!"));
            settextstyle(25, 0, _T("Arial"));
            settextcolor(WHITE);
            wchar_t summary[50];
            swprintf_s(summary, L"收集金币: %d/%d", player.coins, static_cast<int>(coins.size()));
            outtextxy(280, 280, summary);
            wchar_t levelScore[50];
            swprintf_s(levelScore, L"本次积分: %d", player.score);
            outtextxy(280, 320, levelScore);
            outtextxy(280, 380, _T("按空格键下一关"));
            break;

        case GAME_OVER:
            settextstyle(100, 0, _T("Arial"));
            settextcolor(RED);
            outtextxy(250, 100, _T("游戏结束"));
            settextstyle(25, 0, _T("Arial"));
            settextcolor(WHITE);
            wchar_t finalScore[50];
            swprintf_s(finalScore, L"最终积分: %d", player.score);
            outtextxy(280, 280, finalScore);
            outtextxy(280, 320, _T("按空格键重新开始"));
            break;
        }

        FlushBatchDraw();
        Sleep(10);

        // 按ESC退出
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
    }

    // 停止音乐
    musicManager.stop();

    // 释放资源
    EndBatchDraw();
    closegraph();
    return 0;
}