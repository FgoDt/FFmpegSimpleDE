//
// Created by fftest on 18-7-9.
//

#ifndef EST_AUDIORESAMPLE_H
#define EST_AUDIORESAMPLE_H

#include <libavformat/avformat.h>

typedef struct SAVEDAudioPar{
    int ch;
    int fmt;
    int sample;
    int nb_sample_size;
}SAVEDAudioPar;

typedef struct SAVEDAudioResampleCtx{
    SAVEDAudioPar *tgt;
    SAVEDAudioPar *src;
}SAVEDAudioResampleCtx;

SAVEDAudioResampleCtx* saved_resmaple_alloc();

int saved_resample_create(SAVEDAudioResampleCtx* ctx,AVStream *instream );

int saved_resample_set_fmtpar(SAVEDAudioPar *par,enum AVSampleFormat fmt, int ch ,int samplerate );

void saved_resample_close(SAVEDAudioResampleCtx* ctx);



#endif //EST_AUDIORESAMPLE_H
