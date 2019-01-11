#if 1

#include "saved.h"
#include "log.h"
#include "define.h"
#include "codec.h"
#include "internal.h"
#include "format.h"

#include<libavformat/avformat.h>
#include<libavcodec/avcodec.h>
#include<libswresample/swresample.h>
#include<libswscale/swscale.h>
#include<libavutil/time.h>
#include<libavutil/avutil.h>


SAVEDContext* saved_create_context() {
    saved_set_log_level(SAVEDLOG_LEVEL_D);
    SAVEDContext *ctx = (SAVEDContext*)malloc(sizeof(SAVEDContext));
    ctx->ictx = NULL;
    ctx->openmark = OPENMARK;
    SAVLOGD("create context done");
    return ctx;
}


int saved_del_context() {
return -1;
}


int saved_open(SAVEDContext *ctx, const char *path, const char *options, int isencoder) {

    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;

    ctx->ictx = saved_internal_alloc();


    if (path == NULL && !isencoder)
    {
        SAVLOGE("there is no path for decoder use");
        return SAVED_E_USE_NULL;
    }

    int ret =  saved_internal_open(ctx->ictx, path, options);
    if (ret != SAVED_OP_OK)
    {
        return ret;
    }

    return ret;
}

int saved_open_with_par(SAVEDContext *ctx, const char *path, const char *options, int isencoder, 
                                                    int vh, int vw, int vfmt, int vbitrate,
                                                    int asample_rate, int ach, int afmt, int abitrate){
    ctx->ictx = saved_internal_alloc();
    SAVEDInternalContext *ictx = ctx->ictx;
    ictx->isencoder = isencoder;
    int ret = saved_internal_opne_with_par(ctx->ictx,path,options,vh,vw,vbitrate,ach,asample_rate,abitrate);
    return ret;
}

SAVEDPkt* saved_create_pkt() {
    SAVEDPkt *pkt = (SAVEDPkt*)malloc(sizeof(SAVEDPkt));
    pkt->data = NULL;
    pkt->duration = -1;
    pkt->internalPkt = av_packet_alloc();
    pkt->pts = -1;
    pkt->size = -1;
    pkt->type = AVMEDIA_TYPE_UNKNOWN;
    pkt->useinternal = 0;
    return pkt;
}

int saved_pkt_unref(SAVEDPkt *pkt){
    RETIFNULL(pkt) SAVED_E_USE_NULL;
    if(pkt->internalPkt != NULL){
        av_packet_unref(pkt->internalPkt);
    }
    return SAVED_OP_OK;
}

int saved_del_pkt(SAVEDPkt *pkt) {
    RETIFNULL(pkt) SAVED_E_USE_NULL;

    if (pkt->data)
    {
        free(pkt->data);
    }
    if (pkt->internalPkt)
    {
        av_packet_unref(pkt->internalPkt);
        av_packet_free((AVPacket**)&pkt->internalPkt);
    }
    free(pkt);
    return  SAVED_OP_OK;
}

SAVEDFrame* saved_create_frame() {
    SAVEDFrame *f = (SAVEDFrame*)malloc(sizeof(SAVEDFrame));
    f->data = NULL;
    f->duration = -1;
    f->internalframe = av_frame_alloc();
    f->pts = -1;
    f->size = -1;
    f->type = AVMEDIA_TYPE_UNKNOWN;
    f->useinternal = 1;
    return f;
}

int saved_frame_unref(SAVEDFrame *savedFrame){
    RETIFNULL(savedFrame) SAVED_E_USE_NULL;
    if(savedFrame->internalframe!=NULL){
        av_frame_unref(savedFrame->internalframe);
    }
    return  SAVED_OP_OK;
}

int saved_del_frame(SAVEDFrame *savedFrame) {
    RETIFNULL(savedFrame) SAVED_E_USE_NULL;
    if (savedFrame->data)
    {
        free(savedFrame->data);
    }
    if (savedFrame->internalframe) {
        av_frame_unref(savedFrame->internalframe);
        av_frame_free((AVFrame **)&savedFrame->internalframe);
    }
    free(savedFrame);
    return SAVED_OP_OK;
}

int saved_get_pkt(SAVEDContext *ctx, SAVEDPkt *pkt) {
    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(pkt) SAVED_E_USE_NULL;

    int ret =  saved_internal_get_pkt(ctx->ictx, pkt);



    return ret;
}

int saved_send_pkt(SAVEDContext *ctx, SAVEDPkt *pkt) {
    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(pkt) SAVED_E_USE_NULL;

    int ret = saved_internal_send_pkt(ctx->ictx, pkt);
    return ret;
}

int saved_get_frame(SAVEDContext *ctx, SAVEDFrame *f){

    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(f) SAVED_E_USE_NULL;

    int ret = saved_internal_get_frame(ctx->ictx,f);
    return ret;
}

int saved_send_frame(SAVEDContext *ctx, SAVEDFrame *f) {

    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(f) SAVED_E_USE_NULL;
    RETIFNULL(f->internalframe) SAVED_E_USE_NULL;

    AVFrame *iframe = (AVFrame*)f->internalframe;
    iframe->pts = f->pts;
    iframe->pkt_duration = f->duration;
    iframe->format = f->fmt;
    if(f->type == SAVED_MEDIA_TYPE_AUDIO){
        iframe->nb_samples = f->nb_sample;
        iframe->channels = f->ch;
        if(iframe->data[0] == NULL){
            av_frame_get_buffer(iframe,0);

        }
        avcodec_fill_audio_frame(iframe,f->ch,f->fmt,f->data,f->size,0);
    }
    if(f->type == SAVED_MEDIA_TYPE_VIDEO){
        iframe->width =f->width;
        iframe->height = f->height;
        iframe->format = AV_PIX_FMT_YUV420P;
        if(iframe->data[0] == NULL){
            av_frame_get_buffer(iframe,0);
        }
        int ysize = f->size/1.5;
        memcpy(iframe->data[0],f->data,ysize);
        memcpy(iframe->data[1],f->data+ysize,ysize/4);
        memcpy(iframe->data[2],f->data+ysize+ysize/4,ysize/4);

    }



//    if(f->fmt<AV_SAMPLE_FMT_U8P){
//        iframe->linesize[0]=iframe->channels*iframe->nb_samples*av_get_bytes_per_sample(iframe->format);
//    } else{
//        iframe->linesize[0]=iframe->nb_samples*av_get_bytes_per_sample(iframe->format);
//    }

    int ret = saved_internal_send_frame(ctx->ictx, f);
    return ret;
}

int saved_get_frame_raw(SAVEDContext *ctx, unsigned char **data, int linesize[4]) {
    RETIFNULL(data) SAVED_E_USE_NULL;

    SAVEDFrame *f = saved_create_frame();
    int ret = saved_get_frame(ctx, f);
    //todo 
    saved_del_frame(f);
    return ret;

}

int saved_get_pkt_raw(SAVEDContext *ctx, unsigned char * data, int size) {
    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(data) SAVED_E_USE_NULL;

    SAVEDPkt *pkt = saved_create_pkt();
    int ret = saved_get_pkt(ctx, pkt);
    //todo
    saved_del_pkt(pkt);
    return ret;
}

int saved_close(SAVEDContext *ctx){
    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;

    saved_internal_close(ctx->ictx);
    free(ctx);
    return  SAVED_OP_OK;
}

int saved_get_metadata(SAVEDContext *ctx, char *key, char **val){
    RETIFNULL(ctx) SAVED_E_USE_NULL;
    int ret = saved_internal_get_metatdata(ctx->ictx,key,val);
    return  ret;
}



#else

#include "saved.h"
#include "define.h"

int test(){
    saved_set_log_level(3);
    SAVLOGD("hello");
    SAVLOGW("hello");
    SAVLOGE("hello");
    return 0;
}

#endif