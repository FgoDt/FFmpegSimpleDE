#pragma once
#include "saved.h"
#include "define.h"
#include<libavformat/avformat.h>

typedef struct SAVEDFormat {
    AVFormatContext *fmt;
    enum AVMediaType *allTypes;
    int best_audio_index;
    int best_video_index;
}SAVEDFormat;

SAVEDFormat *saved_format_alloc();

int saved_format_open_input(SAVEDFormat* ctx,const char *path, const char *options);

int saved_format_open_output(SAVEDFormat* ctx,const char *path,const char *options);

int saved_format_get_pkt(SAVEDFormat *ctx, AVPacket *pkt);

SAVEDFormat *saved_format_close();