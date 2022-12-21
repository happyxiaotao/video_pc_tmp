#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include "myaudiodevice.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <QIODevice>

class AudioPlayer {
public:
    AudioPlayer();
    ~AudioPlayer();

public:
    void WriteData(const char* buffer, int len);
    bool IsActiveState() { return m_audio_out->state() == QAudio::ActiveState; }

    void Stop();
    void Play();

private:
    QAudioFormat m_audio_fmt;
    QAudioOutput* m_audio_out;
    MyAudioDevice* m_audio_device;
    bool m_bPlay; //是否可以播放音频，用户点击暂停时，不能播放
};

#endif // AUDIOPLAYER_H
