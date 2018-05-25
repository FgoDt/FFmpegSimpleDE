#pragma once
#include "saved.h"
#include "define.h"
#include<libavformat/avformat.h>

typedef struct SAVEDFormat {
    AVFormatContext *fmt;
    int best_audio_index;
    int best_video_index;
}SAVEDFormat;

SAVEDFormat *saved_format_alloc();

int saved_format_open_input(SAVEDContext* ctx,const char *path, const char *options);

int saved_format_open_output(SAVEDContext* ctx,const char *path,const char *options);

SAVEDFormat *saved_format_close();