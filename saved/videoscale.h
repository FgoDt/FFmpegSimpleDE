//
// Created by fftest on 18-7-9.
//

#ifndef EST_VIDEOSCALE_H
#define EST_VIDEOSCALE_H

#include <libswscale/swscale.h>
#include <libavformat/avformat.h>

typedef struct SAVEDPicPar{
    int width;
    int height;
    enum AVPixelFormat fmt;
}SAVEDVideoPar;

typedef struct SAVEDVideoScaleCtx{
    SAVEDVideoPar *tgt;
    SAVEDVideoPar *src;
    struct SwsContext *sws;
    int usehw;
}SAVEDVideoScaleCtx;

SAVEDVideoScaleCtx* saved_video_scale_alloc(void);


void saved_video_scale_close(SAVEDVideoScaleCtx *ctx);

int saved_video_scale_set_par(SAVEDVideoPar *par,int format, int h, int w);

int saved_video_scale_open(SAVEDVideoScaleCtx *ctx);

int saved_video_scale(SAVEDVideoScaleCtx *ctx, AVFrame *src, AVFrame *dst);


#endif //EST_VIDEOSCALE_H
