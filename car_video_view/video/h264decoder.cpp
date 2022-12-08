#include "h264decoder.h"
#include <QDebug>

H264Decoder::H264Decoder()
    : m_av_codec(nullptr)
    , m_av_parser(nullptr)
    , m_av_context(nullptr)
    , m_av_frame_h264(nullptr)
    , m_av_frame_rgb(nullptr)
    , m_av_packet(nullptr)
    , m_sws_context(nullptr)
    , m_tmp_yuv_buffer(nullptr)
{
    m_tmp_buffer = new char[TMP_BUFFER_SIZE + 1];
    memset(m_tmp_buffer, 0, TMP_BUFFER_SIZE + 1);

    m_tmp_yuv_buffer = new uchar[2048000]; // 2M
    memset(m_tmp_yuv_buffer, 0, 2048000);
}

H264Decoder::~H264Decoder()
{
    UnInit();
    delete[] m_tmp_buffer;
    delete[] m_tmp_yuv_buffer;
}

bool H264Decoder::Init()
{
    m_av_codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (m_av_codec == nullptr) {
        qDebug("H264Decoder::Init failed, avcodec_find_decoder return nullptr, ID=AV_CODEC_ID_H264\n");
        UnInit();
        return false;
    }

    m_av_parser = av_parser_init(m_av_codec->id);
    if (m_av_parser == nullptr) {
        qDebug("H264Decoder::Init failed, av_parser_init return nullptr\n");
        UnInit();
        return false;
    }

    m_av_context = avcodec_alloc_context3(m_av_codec);
    if (m_av_context == nullptr) {
        qDebug("H264Decoder::Init failed, avcodec_allock_context3 return nullptr\n");
        UnInit();
        return false;
    }
    m_av_context->codec_type = AVMEDIA_TYPE_VIDEO;

    if (avcodec_open2(m_av_context, m_av_codec, nullptr) < 0) {
        qDebug("H264Decoder::Init failed, avcodec_open2 return negative value\n");
        UnInit();
        return false;
    }
    m_av_frame_h264 = av_frame_alloc();
    m_av_frame_rgb = av_frame_alloc();
    m_av_packet = av_packet_alloc();

    return true;
}

int H264Decoder::Decode(uchar* in, size_t in_size)
{
    size_t left_size = in_size;
    const uchar* data = in;
    av_packet_unref(m_av_packet);
    while (left_size > 0) {
        int ret = av_parser_parse2(m_av_parser, m_av_context, &m_av_packet->data, &m_av_packet->size,
            data, (int)left_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
        if (ret < 0) {
            qDebug("H264Decoder::Decode failed, av_parser_parse2 return negative value, ret:%d\n", ret);
            return -1;
        }
        data += ret;
        left_size -= ret;

        qDebug("%s, ret=%d, left_size=%d,m_av_packet->size=%d\n", __FUNCTION__, ret, left_size, m_av_packet->size);
        if (m_av_packet->size) {
            bool got_frame = false;
            decode_packet(got_frame, m_av_context, m_av_packet, m_av_frame_h264, m_sws_context, m_av_frame_rgb);
        }
    }
    return in_size;
}

void H264Decoder::UnInit()
{
    m_av_codec = nullptr;
    if (m_av_context) {
        avcodec_close(m_av_context);
        avcodec_free_context(&m_av_context);
        m_av_context = nullptr;
    }
    if (m_av_parser) {
        av_parser_close(m_av_parser);
    }
    if (m_av_frame_h264) {
        av_frame_free(&m_av_frame_h264);
        m_av_frame_h264 = nullptr;
    }
    if (m_av_frame_rgb) {
        av_frame_free(&m_av_frame_rgb);
        m_av_frame_rgb = nullptr;
    }
    if (m_av_packet) {
        av_packet_free(&m_av_packet);
    }

    if (m_sws_context) {
        sws_freeContext(m_sws_context);
        m_sws_context = nullptr;
    }
}

void H264Decoder::PrintFfmpegVersion()
{
    qDebug("-------------------------ffmpeg info start-------------------------------------\n");
    qDebug("%s\n", avcodec_configuration());
    qDebug("version:%d\n", avcodec_version());
    qDebug("-------------------------ffmpeg info end-------------------------------------\n");
}

int H264Decoder::decode_packet(bool& got_frame, AVCodecContext* context, AVPacket* packet, AVFrame* frame_h264, SwsContext* not_used_sws_context, AVFrame* frame_yuv)
{
    got_frame = false;
    int ret = 0;
    ret = avcodec_send_packet(m_av_context, packet);
    if (ret < 0 && ret != AVERROR_EOF) {
        qDebug("H264Decoder::decode_packet faield, avcodec_send_packet return negative value,ret:%d\n", ret);
        return -1;
    }

    while (true) {
        av_frame_unref(frame_h264);
        av_frame_unref(frame_yuv);

        ret = avcodec_receive_frame(context, frame_h264);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            qDebug("H264Decoder::decode_packet, Error during decoding\n");
            break;
        }
        qDebug("saving frame_number%d, picture_type=%d, pix_format=%d, width=%d,height=%d\n",
            context->frame_number, frame_h264->pict_type, frame_h264->format, context->width, context->height);

        // auto src_pix_format = AVPixelFormat::AV_PIX_FMT_YUV420P;
        AVPixelFormat src_pix_format = static_cast<AVPixelFormat>(frame_h264->format); //获取像素编码格式
        auto dst_pix_format = AVPixelFormat::AV_PIX_FMT_RGB24;
        int width = frame_h264->width;
        int height = frame_h264->height;
        uchar* pointers[4];
        int linesizes[4];
        int need_buffer_size = av_image_alloc(pointers, linesizes, width, height, dst_pix_format, 1);
        qDebug("need image buffer size:%d\n", need_buffer_size);

        auto sws_context = sws_getContext(context->width, context->height, src_pix_format,
            width, height, dst_pix_format, SWS_BILINEAR, nullptr, nullptr, nullptr);
        int n = sws_scale(sws_context, (const uchar* const*)frame_h264->data, frame_h264->linesize, 0, context->height, pointers, linesizes);
        qDebug("width=%d,height=%d,n=%d,linesizes[0]=%d\n", frame_h264->width, frame_h264->height, n, linesizes[0]);
        sws_freeContext(sws_context);
        int per_line_size = linesizes[0];
        if (m_handler_video) {
            m_handler_video(pointers[0], width, height, per_line_size);
        }

        av_freep(&pointers[0]);

        got_frame = true;
    }

    return 0;
}
