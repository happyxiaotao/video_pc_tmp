#include "audioplayer.h"

AudioPlayer::AudioPlayer()
    : m_audio_out(nullptr)
    , m_audio_device(nullptr)
{
    m_audio_fmt.setSampleRate(8000);
    m_audio_fmt.setSampleSize(16);
    m_audio_fmt.setChannelCount(1);
    m_audio_fmt.setCodec("audio/pcm");
    m_audio_fmt.setByteOrder(QAudioFormat::LittleEndian);
    m_audio_fmt.setSampleType(QAudioFormat::UnSignedInt);

    if (!m_audio_fmt.isValid()) {
        qDebug("AudioPlayer::AudioPlayer, audio format not valid\n");
        exit(0);
    }

    m_audio_out = new QAudioOutput(m_audio_fmt);
    m_audio_device = new MyAudioDevice();
}

AudioPlayer::~AudioPlayer()
{
    if (m_audio_out) {
        delete m_audio_out;
    }
    if (m_audio_device) {
        delete m_audio_device;
        m_audio_device = nullptr;
    }
}

void AudioPlayer::WriteData(const char* buffer, int len)
{
    m_audio_device->AddPcmData(buffer, len);

    if (!IsActiveState()) {
        m_audio_out->start(m_audio_device);
        qDebug("AudioPlayer::WriteData, start player\n");
    }
}
