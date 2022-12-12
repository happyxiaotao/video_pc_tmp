#include "myaudiodevice.h"

MyAudioDevice::MyAudioDevice(QObject* parent)
    : QIODevice { parent }
    , m_audio_need_to_skip(0)
{
    this->open(QIODevice::ReadOnly);

    const int fake_size = 640 * 2;
    m_fake_data.resize(fake_size, '\0');
}

MyAudioDevice::~MyAudioDevice()
{
    this->close();
}

void MyAudioDevice::AddPcmData(const char* buffer, size_t len)
{
    // qDebug("%s, m_audio_need_to_skip=%ld,len=%ld\n", __FUNCTION__, m_audio_need_to_skip, len);

    // 需要将之前传递的假数据占用的字节数，去掉。避免长期下去导致的音视频不同步。
    //    if (len > m_audio_need_to_skip) {
    //        m_pcm_buffer.append(buffer + m_audio_need_to_skip, len - m_audio_need_to_skip);
    //    } else {
    //        m_audio_need_to_skip -= len;
    //    }
    m_pcm_buffer.append(buffer, len);
}

// data 为声卡的数据缓冲区地址，maxlen为声卡缓冲区最大能存放的字节数
qint64 MyAudioDevice::readData(char* data, qint64 maxlen)
{
    const char* buffer = m_pcm_buffer.c_str();
    size_t buffer_size = m_pcm_buffer.size();
    qint64 copy_size = std::min<qint64>(maxlen, buffer_size);

    // qDebug("%s, maxlen=%ld,buffer_size=%ld,copy_size=%ld \n", __FUNCTION__, maxlen, buffer_size, copy_size);
    if (copy_size > 0) {
        memcpy(data, buffer, copy_size);
        m_pcm_buffer.erase(0, copy_size);
    } else if (copy_size == 0) { //没有音频数据了，需要返回假数据，避免多次无数据后，不再回调此接口。
        //    // 拷贝假数据
        //    memcpy(data, m_fake_data.c_str(), m_fake_data.size());
        //    copy_size = m_fake_data.size();
        //    m_audio_need_to_skip += m_fake_data.size();
        //    qDebug("readData, add fake audio data, copy_size=%d,m_audio_need_to_skip=%ld\n", m_fake_data.size(), m_audio_need_to_skip);
    }

    return copy_size;
}

qint64 MyAudioDevice::writeData(const char* data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return len;
}
