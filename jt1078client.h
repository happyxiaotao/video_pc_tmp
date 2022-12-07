#ifndef JT1078CLIENT_H
#define JT1078CLIENT_H

#include <QHostAddress>
#include <QImage>
#include <QTcpSocket>

#include "../audio/CCodec.h"
#include "../audio_player/audioplayer.h"
#include "../jt1078/JT1078Decoder.h"
#include "../video/h264decoder.h"

#include "../ipc_packet/IpcPktDecoder.h"

class Jt1078Client : public QObject {
    Q_OBJECT
public:
    Jt1078Client();
    ~Jt1078Client();

public:
    bool ConnectToHost(const QHostAddress& host, uint16_t port, const std::string& strDeviceId);
public slots:
    void OnConnected();
    void OnDisconnected();
    void OnReadReady();
    void OnError(QAbstractSocket::SocketError error);

private:
    int ProcessIpcPacketCompleted(const ipc::packet_t& packet);
    int ProcessIpcPacketError();

    int ProcessJt1078Packet(const jt1078::packet_t* packet);
    int ProcessJt1078VideoData(const char* buffer, size_t len);
    int ProcessJt1078AudioData(const char* buffer, size_t len, uint64_t timestamp);

private:
    int SendIpcPkt(ipc::IpcPktType type, const char* data, size_t len);
    uint32_t GetSendIpcPktSeqId();

public:
    void ProcessRGB24(const uchar* buffer, int width, int height, int per_line_size);
signals:
    void sig_update_video(QImage* img);
    void sig_update_audio(const std::string* str);

public:
    QTcpSocket* m_client_socket;
    QHostAddress m_server_address;
    uint16_t m_server_port;
    std::string m_strDeviceId;

    enum { TMP_BUFFER_MAX_LEN = 20480 };
    char* m_tmp_buffer;
    jt1078::decoder_t m_jt1078_decoder;
    jt1078::packet_t m_jt1078_packet;

    std::string m_video_buffer;
    std::string m_audio_buffer;

    H264Decoder m_h264_decoder;
    CCodec m_audio_codec;
    AudioPlayer m_audio_player;

    ipc::decoder_t m_ipc_decoder;
    uint32_t m_uLastSendIpcPktSeqId;
};

#endif // JT1078CLIENT_H
