//
// Created by fftest on 18-7-9.
//

#include "videoscale.h"
#include "define.h"


SAVEDVideoScaleCtx* saved_video_scale_alloc(){
    SAVEDVideoScaleCtx *ctx = (SAVEDVideoScaleCtx*)malloc(sizeof(SAVEDVideoScaleCtx));
    if(ctx == NULL){
        SAVLOGE("no MEM");
        return ctx;
    }
    ctx->tgt = NULL;
    ctx->src = NULL;
    ctx->src = malloc(sizeof(SAVEDPicPar));
    ctx->tgt = malloc(sizeof(SAVEDPicPar));
    return  ctx;
}


int saved_video_scale_set_picpar(SAVEDPicPar *par,int format, int h, int w){
    RETIFNULL(par) SAVED_E_USE_NULL;

    par->fmt = format;
    par->height = h;
    par->width  = w;

    return SAVED_OP_OK;
}

int saved_video_scale_open(SAVEDVideoScaleCtx *ctx){
    RETIFNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(ctx->tgt) SAVED_E_USE_NULL;
    RETIFNULL(ctx->src) SAVED_E_USE_NULL;

    ctx->tgt->width /=2;
    ctx->tgt->height/=2;
    ctx->sws = sws_getContext(ctx->src->width,ctx->src->height,ctx->src->fmt,
            ctx->tgt->width,ctx->tgt->height,ctx->tgt->fmt,SWS_POINT,NULL,NULL,NULL);

    if(ctx->sws == NULL) {
        SAVLOGE("sws_getContext error  ctx->sws is NULL");
        return  SAVED_E_AVLIB_ERROR;
    }

    return SAVED_OP_OK;
}

int saved_video_scale(SAVEDVideoScaleCtx *ctx, AVFrame *src, AVFrame *dst){
    RETIFNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(ctx->sws) SAVED_E_USE_NULL;
    RETIFNULL(src) SAVED_E_USE_NULL;
    RETIFNULL(dst) SAVED_E_USE_NULL;

    int ret = sws_scale(ctx->sws,src->data,
            src->linesize,0,ctx->src->height,
            dst->data,dst->linesize);

    if(ret<0){
        SAVLOGE("sws scale error");
        return SAVED_E_AVLIB_ERROR;
    }

    return  SAVED_OP_OK;
}

void saved_video_scale_close(SAVEDVideoScaleCtx *ctx){
    if(ctx->tgt!=NULL){
        free(ctx->tgt);
        ctx->tgt = NULL;
    }
    if(ctx->src!=NULL){
        free(ctx->src);
        ctx->src = NULL;
    }
    if(ctx->sws!=NULL){
        sws_freeContext(ctx->sws);
        ctx->sws = NULL;
    }

    free(ctx);

}
