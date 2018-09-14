//
// Created by fftest on 18-7-9.
//

#include "audioresample.h"
#include <stdlib.h>
#include <libswresample/swresample.h>
#include "define.h"

SAVEDAudioResampleCtx* saved_resample_alloc(){
    SAVEDAudioResampleCtx *ctx = (SAVEDAudioResampleCtx*)malloc(sizeof(SAVEDAudioResampleCtx));
    RETIFNULL(ctx) SAVED_E_NO_MEM;
    memset(ctx,0,sizeof(SAVEDAudioResampleCtx));
    ctx->src = NULL;
    ctx->tgt = NULL;
    return ctx;
}


int saved_resample_create(SAVEDAudioResampleCtx* ctx,AVStream *instream){
    RETIFNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(instream) SAVED_E_USE_NULL;

    ctx->src =(SAVEDAudioPar*) malloc(sizeof(SAVEDAudioPar));
    ctx->tgt =(SAVEDAudioPar*) malloc(sizeof(SAVEDAudioPar));

    if (ctx->src == NULL || ctx->tgt == NULL){
        return SAVED_E_NO_MEM;
    }

    saved_resample_set_fmtpar(ctx->src,instream->codecpar->format,instream->codecpar->channels,instream->codecpar->sample_rate);
    saved_resample_set_fmtpar(ctx->tgt,AV_SAMPLE_FMT_FLTP,instream->codecpar->channels,instream->codecpar->sample_rate);

}

int saved_resample(SAVEDAudioResampleCtx* ctx){

}

void saved_resample_close(SAVEDAudioResampleCtx* ctx){
    RETIFNULL(ctx) ;
    if (ctx->tgt){
        free(ctx->tgt);
        ctx->tgt = NULL;
    }
    if(ctx->src){
        free(ctx->src);
        ctx->src = NULL;
    }

    free(ctx);
}
int saved_resample_set_fmtpar(SAVEDAudioPar *par,enum AVSampleFormat fmt, int ch ,int samplerate ){
    RETIFNULL(par) SAVED_E_USE_NULL;
    par->fmt = fmt;
    par->ch = ch;
    par->sample = samplerate;
    return  SAVED_OP_OK;
}
