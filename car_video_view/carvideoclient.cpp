#include "carvideoclient.h"
#include <QDebug>
#include <QPixmap>
#include <QThread>

CarVideoClient::CarVideoClient(QObject* parent)
    : QObject { parent }
    , m_client_socket(nullptr)
    , m_server_address(nullptr)
    , m_server_port(nullptr)
    , m_device_id(nullptr)
    , m_tmp_read_buffer(nullptr)
    , m_ipc_decoder(nullptr)
    , m_uLastSendIpcPktSeqId(nullptr)
    , m_tmp_write_buffer(nullptr)
    , m_jt1078_packet(nullptr)
    , m_video_buffer(nullptr)
    , m_audio_buffer(nullptr)
    , m_h264_decoder(nullptr)
    , m_audio_codec(nullptr)
    , m_audio_player(nullptr)
{
}

CarVideoClient::~CarVideoClient()
{
    if (m_client_socket) {
        disconnect(m_client_socket);
    }
    ReleaseVar();
}

void CarVideoClient::slot_connect(QHostAddress* host, uint16_t* port, QString* strDeviceId)
{
    InitVar();
    ConnectToHost(*host, *port, *strDeviceId);

    delete host;
    host = nullptr;
    delete port;
    port = nullptr;
    delete strDeviceId;
    strDeviceId = nullptr;
}

void CarVideoClient::slot_disconnect()
{
}

void CarVideoClient::ConnectToHost(const QHostAddress& host, uint16_t port, const QString& strDeviceId)
{
    *m_server_address = host;
    *m_server_port = port;
    *m_device_id = strDeviceId;
    m_client_socket->connectToHost(*m_server_address, *m_server_port);
}

void CarVideoClient::Disconnect()
{
    qDebug() << __FUNCTION__ << ",state=" << m_client_socket->state() << "\n";
    if (m_client_socket->state() == QTcpSocket::ConnectedState) {
        auto str = m_device_id->toStdString();
        SendIpcPkt(ipc::IPC_PKT_UNSUBSCRIBE_DEVICE_ID, str); //其实没必要发，pc_server会自动取消订阅，但是使用有始有终心里舒服些，还是发一个，哈哈
    }
    m_client_socket->close();
}

int CarVideoClient::SendIpcPkt(ipc::IpcPktType type, const char* data, size_t len)
{
    ipc::packet_t packet;
    packet.m_uHeadLength = sizeof(packet);
    packet.m_uDataLength = len;
    packet.m_uPktSeqId = GetSendIpcPktSeqId();
    packet.m_uPktType = type;
    m_tmp_write_buffer->clear();
    m_tmp_write_buffer->append((char*)&packet, packet.m_uHeadLength);
    m_tmp_write_buffer->append(data, len);
    int ret = m_client_socket->write(m_tmp_write_buffer->c_str(), m_tmp_write_buffer->size());
    qDebug("SendIpcPkt,seq_id=%d,ret=%d", packet.m_uPktSeqId, ret);
    m_client_socket->flush();
    return ret;
}

uint32_t CarVideoClient::GetSendIpcPktSeqId()
{
    if (*m_uLastSendIpcPktSeqId == ipc::INVALID_PKT_SEQ_ID) {

        *m_uLastSendIpcPktSeqId = 0;
    } else {
        ++(*m_uLastSendIpcPktSeqId);
    }
    return *m_uLastSendIpcPktSeqId;
}

void CarVideoClient::OnConnected()
{
    qDebug() << __FUNCTION__ << ",host:" << *m_server_address << ",port:" << *m_server_port << ",device_id:" << *m_device_id << "\n";
    // 发送获取视频数据请求
    int ret = SendIpcPkt(ipc::IPC_PKT_SUBSCRIBE_DEVICE_ID, m_device_id->toStdString());
    if (ret < 0) {
        qDebug() << "CarVideoClient::OnConnect, send ipc packet faield, device_id:" << *m_device_id << "\n";
    }
}

void CarVideoClient::OnDisconnected()
{
    qDebug() << __FUNCTION__ << "\n";
    m_client_socket->disconnectFromHost();
}

void CarVideoClient::OnReadReady()
{
    qDebug() << __FUNCTION__ << "\n";
    int nread = m_client_socket->read(m_tmp_read_buffer, TMP_READ_BUFFER_LEN);
    m_tmp_read_buffer[nread] = '\0';

    auto error = m_ipc_decoder->PushBuffer(m_tmp_read_buffer, nread);
    if (error == ipc::decoder_t::kBufferFull) {
        ProcessIpcPacketError();
        return;
    }

    while ((error = m_ipc_decoder->Decode()) == ipc::decoder_t::kNoError) {
        const auto& packet = m_ipc_decoder->GetPacket();
        ProcessIpcPacketCompleted(packet);
    }
    if (error != ipc::decoder_t::kNeedMoreData) {
        ProcessIpcPacketError();
    }
}

void CarVideoClient::OnError(QAbstractSocket::SocketError error)
{
    qDebug() << __FUNCTION__ << error << "\n";
    Disconnect();
}

void CarVideoClient::InitVar()
{
    m_client_socket = new QTcpSocket();
    connect(m_client_socket, &QTcpSocket::connected, this, &CarVideoClient::OnConnected);
    connect(m_client_socket, &QTcpSocket::disconnected, this, &CarVideoClient::OnDisconnected);
    connect(m_client_socket, &QTcpSocket::readyRead, this, &CarVideoClient::OnReadReady);
    connect(m_client_socket, &QTcpSocket::errorOccurred, this, &CarVideoClient::OnError);

    m_server_address = new QHostAddress();
    m_server_port = new uint16_t(-1);
    m_device_id = new QString();

    m_tmp_read_buffer = new char[TMP_READ_BUFFER_LEN + 1];

    m_ipc_decoder = new ipc::decoder_t();
    m_uLastSendIpcPktSeqId = new uint32_t(ipc::INVALID_PKT_SEQ_ID);

    m_tmp_write_buffer = new std::string();

    m_jt1078_packet = new jt1078::packet_t();
    m_jt1078_packet->m_header = new jt1078::header_t();
    m_jt1078_packet->m_body = new char[1024];

    m_video_buffer = new std::string();
    m_audio_buffer = new std::string();

    m_h264_decoder = new H264Decoder();
    m_h264_decoder->Init();
    m_h264_decoder->SetHandlerVideoBuffer(std::bind(&CarVideoClient::ProcessRGB24, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

    m_audio_codec = new CCodec();
    m_audio_player = new AudioPlayer();
}

void CarVideoClient::ReleaseVar()
{
    m_client_socket->deleteLater();
    delete m_server_address;
    delete m_server_port;
    delete m_device_id;
    delete[] m_tmp_read_buffer;
    delete m_ipc_decoder;
    delete m_uLastSendIpcPktSeqId;
    delete m_tmp_write_buffer;
    delete m_jt1078_packet;
    delete m_video_buffer;
    delete m_audio_buffer;
    delete m_h264_decoder;
    delete m_audio_codec;
    delete m_audio_player;
}

int CarVideoClient::ProcessIpcPacketCompleted(const ipc::packet_t& packet)
{
    const char* p = packet.m_data;
    int len = packet.m_uDataLength;
    jt1078::device_id_t device_id;
    memcpy(&device_id, p, sizeof(device_id));
    p += sizeof(device_id);
    len -= sizeof(device_id);

    memcpy(m_jt1078_packet->m_header, p, sizeof(jt1078::header_t));
    p += sizeof(jt1078::header_t);
    len -= sizeof(jt1078::header_t);
    memcpy(m_jt1078_packet->m_body, p, len);
    m_jt1078_packet->m_body[len] = '\0';
    p += len;
    len = 0;
    qDebug("1078 packet,seq=%ld", m_jt1078_packet->m_header->WdPackageSequence);

    ProcessJt1078Packet(m_jt1078_packet);

    //    //之前能处理jt1078数据包的逻辑，是正确的。目前一个ipc包，包含一个jt1078数据包
    //    auto error = m_jt1078_decoder.PushBuffer(p, len);
    //    if (error != jt1078::decoder_t::ErrorType::kNoError) {
    //        qDebug("Jt1078Client::OnReadReady, PushBuffer failed,error:%d\n", error);
    //        return -1;
    //    }
    //
    //    while ((error = m_jt1078_decoder.Decode()) == jt1078::decoder_t::kNoError) {
    //        const auto& packet = m_jt1078_decoder.GetPacket();
    //        ProcessJt1078Packet(&packet);
    //    }
    //
    //    if (error != jt1078::decoder_t::ErrorType::kNeedMoreData) {
    //        qDebug("Jt1078Client::OnReadReady, Decode failed, invalid jt1078 packet\n");
    //        return -2;
    //    }
    return 0;
}

int CarVideoClient::ProcessIpcPacketError()
{
    qDebug("%s\n", __FUNCTION__);
    Disconnect();
    return 0;
}

int CarVideoClient::ProcessJt1078Packet(const jt1078::packet_t* packet)
{
    // 解析jt1078数据包
    int nRet = 0;
    do {
        bool bComplated = false;
        // 当时原子包或者最后一个数据包时，表示读取到了全部包，进行处理
        if (packet->m_header->SubpackageHandleMark4 == RCV_ATOMIC_PACKAGE || packet->m_header->SubpackageHandleMark4 == RCV_LAST_PACKAGE) {
            bComplated = true;
        }
        // 是视频数据包
        if (packet->m_header->DataType4 == DATA_TYPE_VIDEO_I
            || packet->m_header->DataType4 == DATA_TYPE_VIDEO_P
            || packet->m_header->DataType4 == DATA_TYPE_VIDEO_B) {

            // 添加到buffer中
            m_video_buffer->append(packet->m_body, packet->m_header->WdBodyLen);

            qDebug("jt1078 video packet, seq=%d, timestamp=%llu, bodylen=%d, video_buffer.size()=%llu\n",
                packet->m_header->WdPackageSequence, packet->m_header->Bt8timeStamp, packet->m_header->WdBodyLen, m_video_buffer->size());

            if (bComplated) {
                bool bIsKeyFrame = false;
                // 这里将帧转化下
                switch (packet->m_header->DataType4) {
                case DATA_TYPE_VIDEO_I:
                    bIsKeyFrame = true;
                    break;
                case DATA_TYPE_VIDEO_B:
                    break;
                case DATA_TYPE_VIDEO_P:
                    break;
                default:
                    break;
                }

                // nRet = WriteVideoData(m_video_buffer.c_str(), m_video_buffer.size(), bIsKeyFrame, packet->m_header->Bt8timeStamp);

                nRet = ProcessJt1078VideoData(m_video_buffer->c_str(), m_video_buffer->size());
                qDebug("video timestamp=%llu, total len=%ld\n", packet->m_header->Bt8timeStamp, m_video_buffer->size());
                m_video_buffer->clear(); // 使用之后，清空数据

                //  nRet = m_rmpt_stream->WriteData(AVMEDIA_TYPE_VIDEO, (char *)m_video_buffer.c_str(), m_video_buffer.size(), bIsKeyFrame, packet->m_header->Bt8timeStamp);
                if (nRet == -1) {
                    nRet = -2;
                    break;
                }
            }
        } else if (packet->m_header->DataType4 == DATA_TYPE_AUDIO) {
            m_audio_buffer->append(packet->m_body, packet->m_header->WdBodyLen);

            AUDIO_CODING_TYPE audio_type = CCodec::AUDIO_CODING_TYPE::eUnSupport;
            switch (packet->m_header->PT7) {
            case 6:
                audio_type = AUDIO_CODING_TYPE::eG711A;
                break;
            case 7:
                audio_type = AUDIO_CODING_TYPE::eG711U;
                break;
            case 8:
                audio_type = AUDIO_CODING_TYPE::eG726;
                break;
            case 26:
                audio_type = AUDIO_CODING_TYPE::eAdpcm;
                break;
            default:
                break;
            }

            if (audio_type == AUDIO_CODING_TYPE::eUnSupport) {
                int type = packet->m_header->PT7;
                qDebug("not support 1078 audio type, PT7:%d, device_id:%s\n", type, m_device_id->toStdString().c_str());
                nRet = -3;
                break;
            }

            if (bComplated) {
                DECODE_RESULT& iResult = m_audio_codec->DecodeAudio(m_audio_buffer->c_str(), m_audio_buffer->size(), audio_type);
                m_audio_codec->m_uPT = packet->m_header->PT7;
                ProcessJt1078AudioData(iResult.m_pOutBuf, iResult.m_nOutBufLen, packet->m_header->Bt8timeStamp);
                m_audio_buffer->clear();
            }
        } else if (packet->m_header->DataType4 == DATA_TYPE_ROUTE) {
        } else {
            int datatype = packet->m_header->DataType4;
            qDebug("not support 1078 data type:%d\n", datatype);
            nRet = -5;
            break;
        }
    } while (0);
    return nRet;
}

#include <fstream>
int CarVideoClient::ProcessJt1078VideoData(const char* buffer, size_t len)
{
    // 将H264文件保存起来，使用工具查看
    // static std::ofstream ofs;
    // if (!ofs.is_open()) {
    //     ofs.open("E:/tmp/tmp_test.h264", std::ios::binary | std::ios::out | std::ios::trunc);
    // }
    // ofs.write(buffer, len);

    return m_h264_decoder->Decode((uchar*)buffer, len);
}

int CarVideoClient::ProcessJt1078AudioData(const char* buffer, size_t len, uint64_t timestamp)
{
    m_audio_player->WriteData(buffer, len);
    return 0;
}

void CarVideoClient::ProcessRGB24(const uchar* buffer, int width, int height, int per_line_size)
{
    // 注意：在多线程下：QImage可能因为内部的处理机制(看源码好像是不会进行拷贝处理，而是保存对应的指针）
    //          需要将QImage的数据copy出来一份，然后再传递。
    //          否则直接emit img参数，外界在处理参数时，会导致程序崩溃。copy一份之后就不会了。
    qDebug() << __FUNCTION__ << ",width:" << width << ",height:" << height << ",per_line_size:" << per_line_size << "thread_id:" << QThread::currentThreadId() << "\n";
    QImage img(buffer, width, height, per_line_size, QImage::Format_RGB888);
    QImage* new_img = new QImage(img.copy(img.rect()));
    emit sig_update_image(new_img);
}