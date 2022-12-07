#ifndef H264DECODER_H
#define H264DECODER_H

#include <QObject>

#include "../ffmpeg/FFMPEG_Common.h"

typedef std::function<void(const uchar* buffer, int width, int height, int per_line_size)> FunctorHandlerVideoBuffer;

// 解码之后是RGB16
class H264Decoder : public QObject {
    Q_OBJECT
public:
    H264Decoder();
    ~H264Decoder();

public:
    bool Init();
    int Decode(uchar* in, size_t in_size);
    void UnInit();

    void SetHandlerVideoBuffer(FunctorHandlerVideoBuffer handler) { m_handler_video = handler; }

public:
    void PrintFfmpegVersion();

private:
    int decode_packet(bool& got_frame, AVCodecContext* context, AVPacket* packet, AVFrame* frame_h264, SwsContext* sws_context, AVFrame* frame_yuv);

private:
    const AVCodec* m_av_codec;
    AVCodecParserContext* m_av_parser;
    AVCodecContext* m_av_context;
    AVFrame* m_av_frame_h264;
    AVFrame* m_av_frame_rgb;
    AVPacket* m_av_packet;

    SwsContext* m_sws_context;

    enum { TMP_BUFFER_SIZE = 4096 };
    char* m_tmp_buffer;

    FunctorHandlerVideoBuffer m_handler_video;

    uchar* m_tmp_yuv_buffer;
};

#endif // H264DECODER_H
