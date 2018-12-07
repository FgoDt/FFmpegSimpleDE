
#include "internal.h"
#include "encoder.h"
#include "decoder.h"
#include "log.h"
#include "saved.h"

SAVEDInternalContext* saved_internal_alloc() {
    SAVEDInternalContext *ctx = (SAVEDInternalContext*)malloc(sizeof(SAVEDInternalContext));
    if (ctx == NULL)
    {
        SAVLOGE("no mem");
        return NULL;
    }
    ctx->savctx = NULL;
    ctx->isencoder = 0;
    return ctx;
}

int saved_internal_close(SAVEDInternalContext *ictx){
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    int ret = SAVED_OP_OK;
    if(ictx->fmt!=NULL){
        ret |= saved_format_close(ictx->fmt);
        SAVED_SET_NULL(ictx->fmt);
    }
    if(ictx->savctx) {
        ret |= saved_codec_close(ictx->savctx);
    }
    free(ictx);
    return  ret;
}

static int open_encoder() {
    return SAVED_E_FATAL;
}

static int open_decoder(SAVEDInternalContext *ictx) {


    ictx->savctx = saved_codec_alloc();
    if(ictx->savctx == NULL){
        SAVLOGE("saved codec alloc error");

        return SAVED_E_NO_MEM;
    }
    ((SAVEDCodecContext*)(ictx->savctx))->isencoder = ictx->isencoder;

    return  saved_codec_open(ictx->savctx,ictx->fmt);

}


void saved_copy_pkt_dsc(SAVEDPkt *pkt) {
    AVPacket *ipkt = pkt->internalPkt;
    pkt->duration = ipkt->duration;
    pkt->pts = ipkt->pts;
    pkt->size = ipkt->size;
}

int saved_internal_open(SAVEDInternalContext *ictx,const char* path, const char *options) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;

    ictx->fmt = saved_format_alloc();

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
        return open_decoder(ictx);
    }
    

    return SAVED_E_UNDEFINE;
}




int saved_internal_get_pkt(SAVEDInternalContext *ictx, SAVEDPkt *pkt) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(pkt) SAVED_E_USE_NULL;

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

    saved_codec_send_pkt((SAVEDCodecContext *) ictx->savctx, pkt);
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

    int ret = saved_codec_get_frame(ictx->savctx,f);

    return ret;
}

int saved_internal_send_frame(SAVEDInternalContext *ictx, SAVEDFrame *f) {

    if (ictx->isencoder)
    {
        SAVLOGE("decoder can't use send pkt");
        return SAVED_E_FATAL;
    }

    return SAVED_E_UNDEFINE;
}

int saved_internal_get_metatdata(SAVEDInternalContext *ictx,char *key,char **val){
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(ictx->fmt) SAVED_E_USE_NULL;
    RETIFNULL(ictx->fmt->fmt) SAVED_E_USE_NULL;
    RETIFNULL(key) SAVED_E_USE_NULL;
    RETIFNULL(val) SAVED_E_USE_NULL;

    AVDictionaryEntry *entry = NULL;
    entry = av_dict_get(ictx->fmt->fmt->metadata,key,NULL,AV_DICT_IGNORE_SUFFIX);

    if(entry == NULL){
        return SAVED_E_NO_MEDIAFILE;
    }

    *val = (char*)malloc(strlen(entry->value)+1);
    char *tmp = *val;
    tmp[strlen(entry->value)] = 0;
    memcpy(*val,entry->value,strlen(entry->value));
    return  SAVED_OP_OK;

}
