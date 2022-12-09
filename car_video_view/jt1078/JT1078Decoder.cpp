
#include "JT1078Decoder.h"
#include <QDebug>

namespace jt1078 {
decoder_t::decoder_t()
    : m_howmuch(READ_JT1078_HEADER_SIZE)
    , m_read_status(kReadHeaderStep1)
{
    m_packet.m_header = new header_t();
    memset((char*)m_packet.m_header, 0, sizeof(header_t));
    constexpr auto len = std::max<int>(JT1078_MAX_LENGTH + 1, 1024);
    m_packet.m_body = new char[len]; // 应该够了。jt1078包一般包体最大是950
    m_packet.m_body[0] = '\0';
    m_buffer.Reserve(MAX_BUFFER_SIZE / 2); //预开辟空间
}

decoder_t::ErrorType decoder_t::PushBuffer(const Buffer& buffer)
{
    return PushBuffer(buffer.GetBuffer(), buffer.ReadableBytes());
}

decoder_t::ErrorType decoder_t::PushBuffer(const char* buffer, size_t len)
{
    size_t nCapacity = m_buffer.GetCapacity();
    size_t nLeftCapacity = m_buffer.GetLeftCapacity();
    size_t nNewBufferLen = len;
    size_t nCurBufferLen = m_buffer.ReadableBytes();

    // 目前预分配的空间不够，即使是最大空间，也不够。需要再次开辟，这里报错处理
    if (nNewBufferLen + nCurBufferLen > MAX_BUFFER_SIZE) {
        qDebug("decoder_t::PushBuffer,error=kBufferFull,nNewBufferLen:%zd,MAX_BUFFER_SIZE:%zd,nCurBufferLen:%zd,nCapacity:%zd\n",
            nNewBufferLen, MAX_BUFFER_SIZE, nCurBufferLen, nCapacity);
        return kBufferFull;
    }

    // 剩余要求的空间没有达到最大要求，但是也需要开辟空间来存放数据，新开辟的空间足够存储
    if (nNewBufferLen + nCurBufferLen > nCapacity) {
        size_t nNewCapacity = std::min<size_t>(nCapacity * 2, MAX_BUFFER_SIZE);
        qDebug("decoder_t::PushBuffer,need alloc memroy,nNewBufferLen:%zd,nCurBufferLen:%zd,nCapacity:%zd,newCapacity:%zd\n",
            nNewBufferLen, nCurBufferLen, nCapacity, nNewCapacity);
        m_buffer.AutoRemove();
        m_buffer.Reserve(nNewCapacity);
        m_buffer.Append(buffer, len);
    }
    // 新加的数据，只需要旧数据往左移动，即可填充完
    else if (nNewBufferLen > nLeftCapacity) {
        m_buffer.AutoRemove();
        m_buffer.Append(buffer, len);
    }
    //剩余空间足够，新添加的数据直接追加到后面即可
    else // nNewBufferLen <=nLeftCapacity
    {
        m_buffer.Append(buffer, len);
    }

    return kNoError;
}
decoder_t::ErrorType decoder_t::Decode()
{
    if (m_buffer.ReadableBytes() < m_howmuch) {
        return kNeedMoreData;
    }

    if (m_read_status == kReadHeaderStep1) {
        m_packet.Clear();
        m_buffer.ReadBuffer((char*)m_packet.m_header, m_howmuch);
        m_read_status = kReadBody;
        m_howmuch = m_packet.m_header->WdBodyLen;
    }

    if (m_buffer.ReadableBytes() < m_howmuch) {
        return kNeedMoreData;
    }
    if (m_read_status == kReadBody) {
        m_buffer.ReadBuffer(m_packet.m_body, m_howmuch);
        m_read_status = kReadHeaderStep1;
        m_howmuch = READ_JT1078_HEADER_SIZE;
    }
    return kNoError;

    /*
    auto error = kNoError;

    while (error == kNoError) {
        if (m_read_status == kReadHeaderStep1) {
            m_packet.Clear();
            error = ParseHeaderStep1(m_packet.m_header);
            if (error == kNoError) {
                m_read_status = kReadHeaderStep2;
                m_howmuch = READ_MIN_SIZE_FOR_HEADER_SECOND; //在ParseHeaderStep2中需要时刻判断剩余空间大小，避免出现空间不够，继续读取的情况
            }
        } else if (m_read_status == kReadHeaderStep2) {
            error = ParseHeaderStep2(m_packet.m_header); // m_howmuch是最小的内容，存在超过最小字节数的情况。要时刻判断剩余空间大小，避免出现空间不够，继续读取的情况
            if (error == kNoError) {
                m_read_status = kReadBody;
                m_howmuch = m_packet.m_header->WdBodyLen;
            }
        } else if (m_read_status == kReadBody) {
            error = ParseBody(&m_packet);
            if (error == kNoError) {
                m_read_status = kReadHeaderStep1;
                m_howmuch = READ_SIZE_FOR_HEADER_FIRST;
            }
            break;
        }
    }
    return error;
    */
}

decoder_t::ErrorType decoder_t::ParseHeaderStep1(header_t* header)
{
    auto error = kNoError;
    do {
        if (m_buffer.ReadableBytes() < m_howmuch) {
            // sword::Trace("decoder_t::ParseHeaderStep1, buffer need more data,ReadableBytes:{},need size:{}", m_buffer.ReadableBytes(), m_howmuch);
            error = kNeedMoreData;
            break;
        }

        auto value = m_buffer.ReadInt32();
        header->DWFramHeadMark = value /*m_buffer.ReadInt32()*/
            ;
        if (header->DWFramHeadMark != JT_1078_HEAD_MARK) {
            qDebug("decoder_t::ParseHeaderStep1, pHead->DWFrameHeadMark = 0x%llx, fixed value:0x%llx\n", header->DWFramHeadMark, JT_1078_HEAD_MARK);
            error = kInvalidHeader;
            break;
        }

        uint8_t uInt8 = m_buffer.ReadInt8();
        qDebug("uInt8=%x, uInt8>>6=%x, (uInt8>>6)&0x3=%x\n", uInt8, uInt8 >> 6, (uInt8 >> 6) & 0x3);
        header->V2 = (uInt8 >> 6) & 0x3; // 0000 0011
        if (header->V2 != 2) {
            auto n = header->V2;
            qDebug("decoder_t::ParseHeaderStep1, pHead->V2=%d, fixed value:2\n", n);
            error = kInvalidHeader;
            break;
        }
        header->P1 = (uInt8 >> 5) & 0x1; // 0000 0001
        if (header->P1 != 0) {
            auto n = header->P1;
            qDebug("decoder_t::ParseHeaderStep1, pHead->P1=%d, fixed value:0\n", n);
            error = kInvalidHeader;
            break;
        }
        header->X1 = (uInt8 >> 4) & 0x1; // 0000 0001
        if (header->X1 != 0) {
            auto n = header->X1;
            qDebug("decoder_t::ParseHeaderStep1, pHead->X1=%d, fixed value:0\n", n);
            error = kInvalidHeader;
            break;
        }
        header->CC4 = uInt8 & 0xF; // 0000 1111
        if (header->CC4 != 1) {
            auto n = header->CC4;
            qDebug("decoder_t::ParseHeaderStep1, pHead->CC4=%d, fixed value:1\n", n);
            error = kInvalidHeader;
            break;
        }

        uInt8 = m_buffer.ReadInt8();
        header->M1 = (uInt8 >> 7) & 0x1; // 0000 0001
        header->PT7 = uInt8 & 0x7F; // 0111 1111

        if (!IsSupportCodingType(header->PT7)) {
            auto n = header->PT7;
            qDebug("decoder_t::ParseHeaderStep1, not support jt1078 coding type:%d\n", n);
            error = kInvalidHeader;
            break;
        }

        //包序号
        header->WdPackageSequence = m_buffer.ReadInt16();

        // 终端设备的SIM卡号，占6个字节。
        m_buffer.ReadBuffer(reinterpret_cast<char*>(header->BCDSIMCardNumber), sizeof(header->BCDSIMCardNumber));
        header->Bt1LogicChannelNumber = m_buffer.ReadInt8();

        uInt8 = m_buffer.ReadInt8();
        header->DataType4 = (uInt8 >> 4) & 0x0F; // 0000 1111
        if (header->DataType4 != DATA_TYPE_VIDEO_I && header->DataType4 != DATA_TYPE_VIDEO_P && header->DataType4 != DATA_TYPE_VIDEO_B && header->DataType4 != DATA_TYPE_AUDIO && header->DataType4 != DATA_TYPE_ROUTE) {
            // 0000视频I帧，0001视频P帧 0010视频B帧 0011音频帧 0100透传数据
            auto n = header->DataType4;
            qDebug("decoder_t::ParseHeaderStep1, not support jt1078 data type:%d\n", n);
            error = kInvalidHeader;
            break;
        }
        header->SubpackageHandleMark4 = uInt8 & 0x0F; // 0000 1111
        if (header->SubpackageHandleMark4 != RCV_ATOMIC_PACKAGE && header->SubpackageHandleMark4 != RCV_FIRST_PACKAGE && header->SubpackageHandleMark4 != RCV_LAST_PACKAGE && header->SubpackageHandleMark4 != RCV_MIDDLE_PACKAGE) {
            auto pkt_mark = header->SubpackageHandleMark4;
            qDebug("decoder_t::ParseHeaderStep1, not support jt1078 packet mark:%d\n", pkt_mark);
            error = kInvalidHeader;
            break;
        }
    } while (0);

    return error;
}

// 解析剩余头部数据
decoder_t::ErrorType decoder_t::ParseHeaderStep2(header_t* header)
{
    ErrorType error = kNoError;
    do {
        if (m_buffer.ReadableBytes() < m_howmuch) {
            // sword::Trace("decoder_t::ParseHeaderStep2, buffer need more data,ReadableBytes:{},min need size:{}", m_buffer.ReadableBytes(), m_howmuch);
            error = kNeedMoreData;
            break;
        }

        // 由于此时m_howmuch是取的最小值的情况。
        // 音视频数据包，需要的更多

        bool bIsVideo = false; // 是否是视频

        // 时间戳参数，是只有音频和视频才有的参数。透传时没有此参数
        if (header->DataType4 == DATA_TYPE_VIDEO_I || header->DataType4 == DATA_TYPE_VIDEO_P || header->DataType4 == DATA_TYPE_VIDEO_B) {
            // 视频包最少需要
            if (m_buffer.ReadableBytes() < READ_VIDEO_SIZE_FOR_HEADER_SECOND) {
                // sword::Trace("decoder_t::ParseHeaderStep2, video header need more data,ReadableBytes:{},need size:{}", m_buffer.ReadableBytes(), READ_VIDEO_SIZE_FOR_HEADER_SECOND);
                error = kNeedMoreData;
                break;
            }

            header->Bt8timeStamp = m_buffer.ReadInt64();
            bIsVideo = true;
        } else if (header->DataType4 == DATA_TYPE_AUDIO) {
            if (m_buffer.ReadableBytes() < READ_AUDIO_SIZE_FOR_HEADER_SECOND) {
                // sword::Trace("decoder_t::ParseHeaderStep2, audio header need more data,ReadableBytes:{},need size:{}", m_buffer.ReadableBytes(), READ_AUDIO_SIZE_FOR_HEADER_SECOND);
                error = kNeedMoreData;
                break;
            }
            header->Bt8timeStamp = m_buffer.ReadInt64();
        } else if (header->DataType4 == DATA_TYPE_ROUTE) {
            qDebug("decoder_t::ParseHeaderStep2, get route data\n");
        } else {
            auto n = header->DataType4;
            qDebug("decoder_t::ParseHeaderStep2, invalid jt1078 data type:%d\n", n);
            error = kInvalidHeader;
            break;
        }

        // Last Frame Interval 该帧与上一关键帧的时间间隔，单位毫秒。 当数据类型为非视频帧时，没有该字段。
        if (bIsVideo) {
            header->WdLastIFrameInterval = m_buffer.ReadInt16();
            header->WdLastFrameInterval = m_buffer.ReadInt16();
        }

        header->WdBodyLen = m_buffer.ReadInt16();
        if (header->WdBodyLen > JT1078_MAX_LENGTH) {
            qDebug("decoder_t::ParseHeaderStep2, invalid jt1078 body len:%d, expect max len:%d\n", header->WdBodyLen, JT1078_MAX_LENGTH);
            error = kInvalidHeader;
            break;
        }
        //  sword::Trace("decoder_t::ParseHeaderStep2, body len:{}", header->WdBodyLen);
    } while (0);

    return error;
}

decoder_t::ErrorType decoder_t::ParseBody(packet_t* packet)
{
    if (m_buffer.ReadableBytes() < m_howmuch) {
        // sword::Trace("decoder_t::ParseBody, buffer need more data,ReadableBytes:{},need size:{}", m_buffer.ReadableBytes(), m_howmuch);
        return kNeedMoreData;
    }

    m_buffer.ReadBuffer(packet->m_body, m_howmuch);
    packet->m_body[m_howmuch] = '\0';

    return kNoError;
}

bool decoder_t::IsSupportCodingType(size_t uCodingType)
{
    switch (uCodingType) {
    case 6: // G711A
    case 7: // G711U
    case 8: // G726
    case 26: // AdPCM
    case 91: // 透传数据
    case 98: // H264
        return true;
        break;
    default:
        return false;
    }
}
} // namespace jt1078
