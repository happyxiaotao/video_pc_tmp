#ifndef MYAUDIODEVICE_H
#define MYAUDIODEVICE_H

#include <QIODevice>
class MyAudioDevice : public QIODevice {
public:
    explicit MyAudioDevice(QObject* parent = nullptr);
    ~MyAudioDevice();

signals:
public:
    void AddPcmData(const char* buffer, size_t len);
    virtual qint64 readData(char* data, qint64 maxlen) override;
    virtual qint64 writeData(const char* data, qint64 len) override;

private:
    std::string m_pcm_buffer;

    std::string m_fake_data;
    size_t m_audio_need_to_skip; //需要跳过的音频数据大小，当填充假数据时，增加。当有真实数据来的时候，先删除后使用。
};

#endif // MYAUDIODEVICE_H
