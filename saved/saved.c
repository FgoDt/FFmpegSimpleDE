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

    SAVEDInternalContext *ictx = ctx->ictx;
    ictx->fmt = saved_format_alloc();
    RETIFNULL(ictx->fmt) SAVED_E_USE_NULL;

    ictx->isencoder = isencoder;
     
    if (path == NULL && !isencoder)
    {
        SAVLOGE("there is no path for decoder use");
        return SAVED_E_USE_NULL;
    }
    int ret =  saved_internal_open(ictx, path, options);
    if (ret != SAVED_OP_OK)
    {
        return ret;
    }

    return ret;
}


SAVEDPkt* saved_create_pkt() {
    SAVEDPkt *pkt = (SAVEDPkt*)malloc(sizeof(SAVEDPkt));
    pkt->data = NULL;
    pkt->duration = -1;
    pkt->internalPkt = NULL;
    pkt->pts = -1;
    pkt->size = -1;
    pkt->type = AVMEDIA_TYPE_UNKNOWN;
    pkt->useinternal = 0;
    return pkt;
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
    }
    return  SAVED_OP_OK;
}

SAVEDFrame* saved_create_frame() {
    SAVEDFrame *f = (SAVEDFrame*)malloc(sizeof(SAVEDFrame));
    f->data = NULL;
    f->duration = -1;
    f->internalframe = NULL;
    f->pts = -1;
    f->size = -1;
    f->type = AVMEDIA_TYPE_UNKNOWN;
    f->useinternal = 0;
    return f;
}

int saved_del_frame(SAVEDFrame *pkt) {
    RETIFNULL(pkt) SAVED_E_USE_NULL;
    if (pkt->data)
    {
        free(pkt->data);
    }
    if (pkt->internalframe) {
        free(pkt->internalframe);
    }
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

int saved_get_frame(SAVEDContext *ctx, SAVEDFrame *f) {

    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(f) SAVED_E_USE_NULL;

    int ret = saved_internal_get_frame(ctx->ictx, f);
    return ret;
}

int saved_send_frame(SAVEDContext *ctx, SAVEDFrame *f) {

    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(f) SAVED_E_USE_NULL;

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