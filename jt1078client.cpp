#include "jt1078client.h"
#include <QByteArray>
#include <QDebug>
#include <QImage>

Jt1078Client::Jt1078Client()
    : m_uLastSendIpcPktSeqId(ipc::INVALID_PKT_SEQ_ID)
{
    m_tmp_buffer = new char[TMP_BUFFER_MAX_LEN + 1];
    memset(m_tmp_buffer, 0, TMP_BUFFER_MAX_LEN + 1);

    m_client_socket = new QTcpSocket();

    connect(m_client_socket, &QTcpSocket::connected, this, &Jt1078Client::OnConnected);
    connect(m_client_socket, &QTcpSocket::disconnected, this, &Jt1078Client::OnDisconnected);
    connect(m_client_socket, &QTcpSocket::readyRead, this, &Jt1078Client::OnReadReady);
    connect(m_client_socket, &QTcpSocket::errorOccurred, this, &Jt1078Client::OnError);

    m_h264_decoder.Init();
    m_h264_decoder.SetHandlerVideoBuffer(std::bind(&Jt1078Client::ProcessRGB24, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

    m_jt1078_packet.m_header = new jt1078::header_t();
    m_jt1078_packet.m_body = new char[1024];
}

Jt1078Client::~Jt1078Client()
{
    delete[] m_tmp_buffer;

    disconnect(m_client_socket);
    if (m_client_socket) {
        m_client_socket->deleteLater();
    }
}

bool Jt1078Client::ConnectToHost(const QHostAddress& host, uint16_t port, const std::string& strDeviceId)
{
    m_server_address = host;
    m_server_port = port;
    m_strDeviceId = strDeviceId;
    m_client_socket->connectToHost(host, port);
    return true;
}

// 发送到808服务器的redis消息示例
//连接某个服务器 {\"port\":\"9991\",\"ip\":\"v.gcpgps.com\",\"iccid\":\"013800138999\",\"channelNo\":\"2\",\"dataTyoe\":\"0\",\"codeStreamType\":\"1\",\"commandId\":\"9101\"}
//断开服务器连接 {\"port\":\"9991\",\"ip\":\"v.gcpgps.com\",\"iccid\":\"013800138999\",\"channelNo\":\"2\",\"dataTyoe\":\"0\",\"codeStreamType\":\"1\",\"commandId\":\"9102\"}
// 河南万里周口运输集团有限公司
// 06475361546203这个车辆，可以看视频
void Jt1078Client::OnConnected()
{
    qDebug() << __FUNCTION__ << "\n";
    // 连接之后，发送订阅消息
    SendIpcPkt(ipc::IPC_PKT_SUBSCRIBE_DEVICE_ID, m_strDeviceId.c_str(), m_strDeviceId.size());
}
void Jt1078Client::OnDisconnected()
{
    qDebug() << __FUNCTION__ << "\n";
    m_client_socket->disconnectFromHost();
}

void Jt1078Client::OnReadReady()
{
    int nread = m_client_socket->read(m_tmp_buffer, TMP_BUFFER_MAX_LEN);
    m_tmp_buffer[nread] = '\0';

    //    return;

    auto error = m_ipc_decoder.PushBuffer(m_tmp_buffer, nread);
    if (error == ipc::decoder_t::kBufferFull) {
        ProcessIpcPacketError();
        return;
    }

    while ((error = m_ipc_decoder.Decode()) == ipc::decoder_t::kNoError) {
        const auto& packet = m_ipc_decoder.GetPacket();
        ProcessIpcPacketCompleted(packet);
    }
    if (error != ipc::decoder_t::kNeedMoreData) {
        ProcessIpcPacketError();
    }
}

void Jt1078Client::OnError(QAbstractSocket::SocketError error)
{
    qDebug() << __FUNCTION__ << "error:" << error << "\n";
}

int Jt1078Client::ProcessIpcPacketError()
{
    qDebug("invalid ipc packet");
    return 0;
}

int Jt1078Client::ProcessIpcPacketCompleted(const ipc::packet_t& packet)
{
    const char* p = packet.m_data;
    int len = packet.m_uDataLength;
    jt1078::device_id_t device_id;
    memcpy(&device_id, p, sizeof(device_id));
    p += sizeof(device_id);
    len -= sizeof(device_id);

    memcpy(m_jt1078_packet.m_header, p, sizeof(jt1078::header_t));
    p += sizeof(jt1078::header_t);
    len -= sizeof(jt1078::header_t);
    memcpy(m_jt1078_packet.m_body, p, len);
    m_jt1078_packet.m_body[len] = '\0';
    qDebug("1078 packet,seq=%ld", m_jt1078_packet.m_header->WdPackageSequence);

    ProcessJt1078Packet(&m_jt1078_packet);

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

int Jt1078Client::ProcessJt1078Packet(const jt1078::packet_t* packet)
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
            m_video_buffer.append(packet->m_body, packet->m_header->WdBodyLen);

            qDebug("jt1078 video packet, seq=%d, timestamp=%llu, bodylen=%d, video_buffer.sizee()=%llu\n",
                packet->m_header->WdPackageSequence, packet->m_header->Bt8timeStamp, packet->m_header->WdBodyLen, m_video_buffer.size());

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

                nRet = ProcessJt1078VideoData(m_video_buffer.c_str(), m_video_buffer.size());
                qDebug("video timestamp=%llu, total len=%ld\n", packet->m_header->Bt8timeStamp, m_video_buffer.size());
                m_video_buffer.clear(); // 使用之后，清空数据

                //  nRet = m_rmpt_stream->WriteData(AVMEDIA_TYPE_VIDEO, (char *)m_video_buffer.c_str(), m_video_buffer.size(), bIsKeyFrame, packet->m_header->Bt8timeStamp);
                if (nRet == -1) {
                    nRet = -2;
                    break;
                }
            }
        } else if (packet->m_header->DataType4 == DATA_TYPE_AUDIO) {
            m_audio_buffer.append(packet->m_body, packet->m_header->WdBodyLen);

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
                qDebug("not support 1078 audio type, PT7:%d, device_id:%s\n", type, m_strDeviceId.c_str());
                nRet = -3;
                break;
            }

            if (bComplated) {
                DECODE_RESULT& iResult = m_audio_codec.DecodeAudio(m_audio_buffer.c_str(), m_audio_buffer.size(), audio_type);
                m_audio_codec.m_uPT = packet->m_header->PT7;

                qDebug("audio, packet->m_header->WdBodyLen=%d\n", packet->m_header->WdBodyLen);
                ProcessJt1078AudioData(iResult.m_pOutBuf, iResult.m_nOutBufLen, packet->m_header->Bt8timeStamp);
                m_audio_buffer.clear();
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
int Jt1078Client::ProcessJt1078VideoData(const char* buffer, size_t len)
{
    // 将H264文件保存起来，使用工具查看
    // static std::ofstream ofs;
    // if (!ofs.is_open()) {
    //     ofs.open("E:/tmp/tmp_test.h264", std::ios::binary | std::ios::out | std::ios::trunc);
    // }
    // ofs.write(buffer, len);

    return m_h264_decoder.Decode((uchar*)buffer, len);
}

int Jt1078Client::ProcessJt1078AudioData(const char* buffer, size_t len, uint64_t timestamp)
{
    m_audio_player.WriteData(buffer, len);
    return 0;
}

int Jt1078Client::SendIpcPkt(ipc::IpcPktType type, const char* data, size_t len)
{
    ipc::packet_t packet;
    packet.m_uHeadLength = sizeof(packet);
    packet.m_uDataLength = len;
    packet.m_uPktSeqId = GetSendIpcPktSeqId();
    packet.m_uPktType = type;
    std::string str;
    str.append((char*)&packet, packet.m_uHeadLength);
    str.append(data, len);
    int ret = m_client_socket->write(str.c_str(), str.size());
    qDebug("SendIpcPkt,ret=%ld", ret);
    return ret;
}

uint32_t Jt1078Client::GetSendIpcPktSeqId()
{
    if (m_uLastSendIpcPktSeqId == ipc::INVALID_PKT_SEQ_ID) {

        m_uLastSendIpcPktSeqId = 0;
    } else {
        ++m_uLastSendIpcPktSeqId;
    }
    return m_uLastSendIpcPktSeqId;
}

void Jt1078Client::ProcessRGB24(const uchar* buffer, int width, int height, int per_line_size)
{
    QImage* img = new QImage(buffer, width, height, per_line_size, QImage::Format_RGB888);
    emit sig_update_video(img);
    // QImage img(buffer, width, height, per_line_size, QImage::Format_RGB888);
    // QImage* new_img = new QImage(img.scaled(width * 2, height * 2));
    // emit sig_update_video(new_img);
}
