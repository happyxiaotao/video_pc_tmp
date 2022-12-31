#ifndef CARVIDEOCLIENT_H
#define CARVIDEOCLIENT_H

#include "audio/CCodec.h"
#include "audio_player/audioplayer.h"
#include "ipc_packet/IpcPktDecoder.h"
#include "jt1078/JT1078Packet.h"
#include "video/h264decoder.h"
#include <QHostAddress>
#include <QImage>
#include <QPixmap>
#include <QTcpSocket>

class CarVideoClient : public QObject {
    Q_OBJECT
public:
    explicit CarVideoClient(QObject* parent = nullptr);
    ~CarVideoClient();

signals:
    void sig_connected();
    void sig_disconnected();
    void sig_update_image(QImage* img);

public slots:
    //收到信号，连接服务器，获取指定deviceid的数据
    void slot_connect(QHostAddress* host, uint16_t* port, QString* strDeviceId);
    //收到信号，断开服务器
    void slot_disconnect();
    // 释放资源
    void slot_release();

    // 停止音频播放
    void slot_stop_audio();
    void slot_start_audio();
private slots:
    void OnConnected();
    void OnDisconnected();
    void OnReadReady();
    void OnError(QAbstractSocket::SocketError error);

private:
    void InitVar(); // 有些变量需要在子线程中创建，并使用
    void ReleaseVar();

    void ConnectToHost(const QHostAddress& host, uint16_t port, const QString& strDeviceId);
    void Disconnect();

    inline int SendIpcPkt(uint32_t type, const std::string& str) { return SendIpcPkt(type, str.c_str(), str.size()); }
    int SendIpcPkt(uint32_t type, const char* data, size_t len);
    uint32_t GetSendIpcPktSeqId();

private:
    int ProcessIpcPacketCompleted(const ipc::packet_t& packet);
    int ProcessIpcPacketError();

    int ProcessJt1078Packet(const jt1078::packet_t* packet);
    int ProcessJt1078VideoData(const char* buffer, size_t len);
    int ProcessJt1078AudioData(const char* buffer, size_t len, uint64_t timestamp);
    void ProcessRGB24(const uchar* buffer, int width, int height, int per_line_size);

private:
    QTcpSocket* m_client_socket;
    QHostAddress* m_server_address;
    uint16_t* m_server_port;
    QString* m_device_id;

    enum { TMP_READ_BUFFER_LEN = 100 * 1024 };
    char* m_tmp_read_buffer;

    ipc::decoder_t* m_ipc_decoder;
    uint32_t* m_uLastSendIpcPktSeqId;
    std::string* m_tmp_write_buffer;

    jt1078::packet_t* m_jt1078_packet;
    std::string* m_video_buffer;
    std::string* m_audio_buffer;
    H264Decoder* m_h264_decoder;
    CCodec* m_audio_codec;
    AudioPlayer* m_audio_player;
};

#endif // CARVIDEOCLIENT_H
