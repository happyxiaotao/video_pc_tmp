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

    FunctorHandlerVideoBuffer m_handler_video;

    struct ImageBuffer {
        ImageBuffer()
        {
            pointers[0] = pointers[1] = pointers[2] = pointers[3] = nullptr;
            linesizes[0] = linesizes[1] = linesizes[2] = linesizes[3] = 0;
            width = 0;
            height = 0;
            dst_pix_format = AV_PIX_FMT_NONE;
            align = 1;
            sws_ctx = nullptr;
        }
        ~ImageBuffer()
        {
            if (pointers[0] != nullptr) {
                av_freep(&pointers[0]);
            }

            if (sws_ctx) {
                sws_freeContext(sws_ctx);
            }
        }
        uchar* pointers[4];
        int linesizes[4];
        int width;
        int height;
        AVPixelFormat dst_pix_format;
        int align;

        struct SwsContext* sws_ctx;
    };
    ImageBuffer* m_last_image_buffer;
};

#endif // H264DECODER_H
