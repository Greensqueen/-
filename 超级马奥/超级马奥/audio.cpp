// ���ֹ�����
class MusicManager {
private:
    bool isPlaying;
    char musicPath[MAX_PATH];

public:
    MusicManager() : isPlaying(false) {
        // Ĭ������·��
        strcpy_s(musicPath, MAX_PATH, "background.wav");
    }

    // ���������ļ�·��
    void setMusicPath(const char* path) {
        strcpy_s(musicPath, MAX_PATH, path);
    }

    // ��������
    void play() {
        if (!isPlaying) {
            // �򿪲��������֣��ظ�����
            mciSendString("close background_music", NULL, 0, NULL);
            char command[256];
            sprintf_s(command, "open \"%s\" alias background_music", musicPath);
            mciSendString(command, NULL, 0, NULL);
            mciSendString("play background_music repeat", NULL, 0, NULL);
            isPlaying = true;
        }
    }

    // ��ͣ����
    void pause() {
        if (isPlaying) {
            mciSendString("stop background_music", NULL, 0, NULL);
            mciSendString("close background_music", NULL, 0, NULL);
            isPlaying = false;
        }
    }

    // ��������Ƿ����ڲ���
    bool isMusicPlaying() const {
        return isPlaying;
    }
};