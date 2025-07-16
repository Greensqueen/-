// 音乐管理类
class MusicManager {
private:
    bool isPlaying;
    char musicPath[MAX_PATH];

public:
    MusicManager() : isPlaying(false) {
        // 默认音乐路径
        strcpy_s(musicPath, MAX_PATH, "background.wav");
    }

    // 设置音乐文件路径
    void setMusicPath(const char* path) {
        strcpy_s(musicPath, MAX_PATH, path);
    }

    // 播放音乐
    void play() {
        if (!isPlaying) {
            // 打开并播放音乐，重复播放
            mciSendString("close background_music", NULL, 0, NULL);
            char command[256];
            sprintf_s(command, "open \"%s\" alias background_music", musicPath);
            mciSendString(command, NULL, 0, NULL);
            mciSendString("play background_music repeat", NULL, 0, NULL);
            isPlaying = true;
        }
    }

    // 暂停音乐
    void pause() {
        if (isPlaying) {
            mciSendString("stop background_music", NULL, 0, NULL);
            mciSendString("close background_music", NULL, 0, NULL);
            isPlaying = false;
        }
    }

    // 检查音乐是否正在播放
    bool isMusicPlaying() const {
        return isPlaying;
    }
};