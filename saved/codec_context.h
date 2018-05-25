#pragma once
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>


typedef struct SAVEDCodec {
    AVCodec *ictx;
    AVCodecContext *avctx;
    AVFormatContext *pfmt;
}SAVEDCodecContext;
