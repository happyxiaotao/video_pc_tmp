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

private:
    bool m_start;
    QAudioFormat m_audio_fmt;
    QAudioOutput* m_audio_out;
    MyAudioDevice* m_audio_device;
};

#endif // AUDIOPLAYER_H
