//
// Created by fftest on 18-7-9.
//

#include "audioresample.h"
#include <stdlib.h>
#include <libswresample/swresample.h>
#include "define.h"

SAVEDAudioResampleCtx* saved_resample_alloc(){
    SAVEDAudioResampleCtx *ctx = (SAVEDAudioResampleCtx*)malloc(sizeof(SAVEDAudioResampleCtx));
    RETIFNULL(ctx) NULL;
    memset(ctx,0,sizeof(SAVEDAudioResampleCtx));
    ctx->src = NULL;
    ctx->tgt = NULL;
    ctx->src =(SAVEDAudioPar*) malloc(sizeof(SAVEDAudioPar));
    if(ctx->src == NULL)
        goto fail;

    ctx->tgt =(SAVEDAudioPar*) malloc(sizeof(SAVEDAudioPar));
    if(ctx->tgt == NULL)
        goto fail;
    ctx->src->fmt = AV_SAMPLE_FMT_NONE;
    ctx->tgt->fmt = AV_SAMPLE_FMT_NONE;
    return ctx;

    fail:
    SAVLOGE("NO MEM for amlloc saved audio resample ctx");
    saved_resample_close(ctx);
    return  NULL;
}



int saved_resample_open(SAVEDAudioResampleCtx* ctx){
    RETIFNULL(ctx) SAVED_E_USE_NULL;

    if (ctx->src == NULL || ctx->tgt == NULL ||
        ctx->src->fmt == AV_SAMPLE_FMT_NONE || ctx->tgt->fmt == AV_SAMPLE_FMT_NONE){
        return SAVED_E_USE_NULL;
    }

    if(ctx->swrContext!=NULL){
        swr_close(ctx->swrContext);
        swr_free(&ctx->swrContext);
        ctx->swrContext = NULL;
    }

    ctx->swrContext = swr_alloc_set_opts(NULL,ctx->tgt->ch_layout,ctx->tgt->fmt,ctx->tgt->sample,
            ctx->src->ch_layout,ctx->src->fmt,ctx->src->sample,0,NULL);

    int ret = swr_init(ctx->swrContext);
    return  ret;


}

int saved_resample(SAVEDAudioResampleCtx* ctx,AVFrame *inf,AVFrame *out){
    RETIFNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(inf) SAVED_E_USE_NULL;
    RETIFNULL(out) SAVED_E_USE_NULL;

    if(ctx->src->sample != inf->sample_rate ||
            ctx->src->fmt != inf->format ||
            ctx->src->ch != inf->channels){
        int ret =  saved_resample_set_fmtpar(ctx->src,inf->format,inf->channels,inf->format);
        ret |= saved_resample_open(ctx);
        if(ret!=SAVED_OP_OK){
            SAVLOGW("reopen audio resample  error");
            return ret;
        }
    }

    int nb_out_sample = (int)inf->nb_samples*ctx->tgt->sample/inf->sample_rate+256;

    int ret = swr_convert(ctx->swrContext,out->data,nb_out_sample,inf->extended_data,inf->nb_samples);


    return  ret;

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
    if(ctx->swrContext){
        swr_close(ctx->swrContext);
        swr_free(&ctx->swrContext);
        ctx->swrContext = NULL;
    }
    free(ctx);
}
int saved_resample_set_fmtpar(SAVEDAudioPar *par,enum AVSampleFormat fmt, int ch ,int samplerate ){
    RETIFNULL(par) SAVED_E_USE_NULL;
    par->fmt = fmt;
    par->ch = ch;
    par->sample = samplerate;
    par->ch_layout = av_get_default_channel_layout(ch);
    return  SAVED_OP_OK;
}
