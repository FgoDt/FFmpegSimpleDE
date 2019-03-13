#ifndef __SAVED_FORMAT_H__
#define __SAVED_FORMAT_H__

#include "saved.h"
#include "define.h"
#include <libavformat/avformat.h>

#define  MAX_STREAMS 10

#define SAVED_FORMAT_GET_ERROR -3
#define SAVED_FORMAT_FORCE_CLOSE -2
#define SAVED_FORMAT_OPEN_ERROR -1
#define SAVED_FORMAT_TRY_OPEN 0
#define SAVED_FORMAT_OPENING 1
#define SAVED_FORMAT_OPEN 2
#define SAVED_FORMAT_RUNING 3
#define SAVED_FORMAT_TRY_GET 4
#define SAVED_FORMAT_GET 5

typedef struct SAVEDFormat {
    AVFormatContext *fmt;
    AVInputFormat *iformat;
    AVStream *astream; //best_audio stream;
    AVStream *vstream; //best video stream;
    AVStream *sstream; //best sub stream
    AVRational extra_time_base; //use in internal encoder
    enum AVMediaType allTypes[MAX_STREAMS];
    int best_audio_index;
    int best_video_index;
    int is_write_header;
    double start_pts;
    int flag;
}SAVEDFormat;

SAVEDFormat *saved_format_alloc();

int saved_format_free(SAVEDFormat *fmt);

int saved_format_open_input(SAVEDFormat* ctx,const char *path, const char *options);

int saved_format_open_output(SAVEDFormat* ctx, void *encoderContext,const char *path,const char *options);
int saved_format_open_output_with_vpar(SAVEDFormat* ctx, void *encoderContext,AVStream *vstream,const char *path,const char *options);


int saved_format_open_output_with_par(SAVEDFormat *ctx,int vw, int vh, uint8_t *vextradata, int vextradata_size,
                                                        int ach, int asample_rate, uint8_t *aextradata, int aextradata_size);

int saved_format_get_pkt(SAVEDFormat *ctx, AVPacket *pkt);

int saved_format_send_pkt(SAVEDFormat *ctx, SAVEDPkt *pkt);

int saved_format_close(SAVEDFormat *fmt);

#endif
