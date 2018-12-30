//
// Created by fftest on 18-7-9.
//

#ifndef EST_AUDIORESAMPLE_H
#define EST_AUDIORESAMPLE_H

#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

typedef struct SAVEDAudioPar{
    int ch;
    int fmt;
    int sample;
    int nb_sample_size;
    int ch_layout;
}SAVEDAudioPar;

typedef struct SAVEDAudioResampleCtx{
    SAVEDAudioPar *tgt;
    SAVEDAudioPar *src;
    SwrContext  *swrContext;
}SAVEDAudioResampleCtx;

SAVEDAudioResampleCtx* saved_resample_alloc();

int saved_resample_open(SAVEDAudioResampleCtx* ctx);

int saved_resample_set_fmtpar(SAVEDAudioPar *par,enum AVSampleFormat fmt, int ch ,int samplerate );

int saved_resample(SAVEDAudioResampleCtx *ctx, AVFrame *inf, uint8_t **out);

void saved_resample_close(SAVEDAudioResampleCtx* ctx);



#endif //EST_AUDIORESAMPLE_H
