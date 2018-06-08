#include "internal_context.h"
#include "encoder.h"
#include "decoder.h"

SAVEDInternalContext* saved_internal_alloc() {
    SAVEDInternalContext *ctx = (SAVEDInternalContext*)malloc(sizeof(SAVEDInternalContext));
    if (ctx == NULL)
    {
        SAVLOGE("no mem");
        return NULL;
    }
    ctx->fmt = NULL;
    ctx->savctx = NULL;
    ctx->isencoder = 0;
    return ctx;
}

static int open_encoder() {
}

static int open_decoder() {
}


void saved_copy_pkt_dsc(SAVEDPkt *pkt) {
    AVPacket *ipkt = pkt->internalPkt;
    pkt->duration = ipkt->duration;
    pkt->pts = ipkt->pts;
    pkt->size = ipkt->size;
}

int saved_internal_open(SAVEDInternalContext *ictx,const char* path, const char *options) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;

    int ret = 0;
    if (ictx->isencoder&&path!=NULL)
    {
       ret = saved_format_open_output(ictx->fmt, path, options);
    }
    else if(path!=NULL)
    {
       ret = saved_format_open_input(ictx->fmt, path, options);
    }

    if (ret != SAVED_OP_OK)
    {
        return ret;
    }

    if (ictx->isencoder)
    {
        return SAVED_E_UNDEFINE;
    }
    else
    {
        
    }
    

    return SAVED_E_UNDEFINE;
}




int saved_internal_get_pkt(SAVEDInternalContext *ictx, SAVEDPkt *pkt) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(pkt) SAVED_E_USE_NULL;

    pkt->internalPkt = av_packet_alloc();
    int ret = SAVED_E_UNDEFINE;

    if (ictx->isencoder)
    {
        ret = saved_encoder_recive_pkt();
    }
    else
    {
        ret = saved_format_get_pkt(ictx->fmt, pkt->internalPkt);
        if (ret != SAVED_OP_OK) {
            return ret;
        }

        saved_copy_pkt_dsc(pkt);
        AVPacket *ipkt = (AVPacket*)pkt->internalPkt;
        pkt->type = ictx->fmt->allTypes[ipkt->stream_index];
    }

    return SAVED_OP_OK;
}


int saved_internal_send_pkt(SAVEDInternalContext *ictx, SAVEDPkt *pkt) {


    return SAVED_E_UNDEFINE;
}

int saved_internal_get_frame(SAVEDInternalContext *ictx, SAVEDFrame *f) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(f) SAVED_E_USE_NULL;

    if (ictx->isencoder)
    {
        SAVLOGE("encoder can't use get frame");
        return SAVED_E_FATAL;
    }

    return SAVED_E_UNDEFINE;
}

int saved_internal_send_frame(SAVEDInternalContext *ictx, SAVEDFrame *f) {

    if (ictx->isencoder)
    {
        SAVLOGE("decoder can't use send pkt");
        return SAVED_E_FATAL;
    }

    return SAVED_E_UNDEFINE;
}