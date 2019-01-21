
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
    ctx->fmt = NULL;
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
        return SAVED_E_UNDEFINE;
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

int saved_internal_opne_with_par(SAVEDInternalContext *ictx, const char *path, const char *options,
                                                                        int vh, int vw, int vbit_rate, 
                                                                        int ach, int asample_rate, int abit_rate){
    if(ictx->isencoder){
        ictx->savctx = saved_codec_alloc();
        int ret = saved_codec_open_with_par(ictx->savctx,vh,vw,NULL,vbit_rate,asample_rate,NULL,ach,abit_rate);

        if(path != NULL){
            ictx->fmt = saved_format_alloc();
            SAVEDCodecContext *enctx = (SAVEDCodecContext*)ictx->savctx;
            ret =  saved_format_open_output(ictx->fmt,enctx->encoderctx,path,options);
        }

        return ret;
    }

    return SAVED_E_UNDEFINE;
}


int saved_internal_get_pkt(SAVEDInternalContext *ictx, SAVEDPkt *pkt) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(pkt) SAVED_E_USE_NULL;

    int ret = SAVED_E_UNDEFINE;

    if (ictx->isencoder)
    {
       ret =  saved_codec_get_pkt(ictx->savctx,pkt);
    }
    else
    {
        ret = saved_format_get_pkt(ictx->fmt, pkt->internalPkt);
        if (ret != SAVED_OP_OK) {
            return ret;
        }

        saved_copy_pkt_dsc(pkt);
        AVPacket *ipkt = (AVPacket*)pkt->internalPkt;
        pkt->pts = av_q2d(ictx->fmt->fmt->streams[ipkt->stream_index]->time_base)*pkt->pts;
        pkt->duration = av_q2d(ictx->fmt->fmt->streams[ipkt->stream_index]->time_base)*pkt->duration;
        pkt->type = ictx->fmt->allTypes[ipkt->stream_index];
    }


    return ret;
}


int saved_internal_send_pkt(SAVEDInternalContext *ictx, SAVEDPkt *pkt) {

    int ret = 0;
    if(ictx->isencoder){
       ret = saved_format_send_pkt(ictx->fmt,pkt);
    } else{
       ret = saved_codec_send_pkt((SAVEDCodecContext *) ictx->savctx, pkt);

    }
    return ret;
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

    if (!ictx->isencoder)
    {
        SAVLOGE("decoder can't use send pkt");
        return SAVED_E_FATAL;
    }

    int ret = saved_codec_send_frame(ictx->savctx,f);

    return ret;
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


int saved_internal_set_audio_par(SAVEDInternalContext *ictx,int ach, int sample, int fmt){
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    SAVEDCodecContext *codecContext = (SAVEDCodecContext*)ictx->savctx;
    if(codecContext->forceAudioPar == NULL){
        codecContext->forceAudioPar = (SAVEDAudioPar*)malloc(sizeof(SAVEDAudioPar));
    }
    codecContext->forceAudioPar->ch=ach;
    codecContext->forceAudioPar->sample = sample;
    codecContext->forceAudioPar->fmt = fmt;
    return SAVED_OP_OK;
}

int saved_internal_set_video_par(SAVEDInternalContext *ictx,int vw, int vh, int fmt){
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    SAVEDCodecContext *codecContext = (SAVEDCodecContext*)ictx->savctx;
    if(codecContext->forceVideoPar== NULL){
        codecContext->forceVideoPar = (SAVEDPicPar*)malloc(sizeof(SAVEDPicPar));
    }
    codecContext->forceVideoPar->fmt = fmt;
    codecContext->forceVideoPar->height = vh;
    codecContext->forceVideoPar->width = vw;
    return  SAVED_OP_OK;
}

int saved_internal_get_audio_par(SAVEDInternalContext *ictx,int *ach, int* sample, int* fmt){
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    SAVEDCodecContext *codecContext = (SAVEDCodecContext*)ictx->savctx;
    if(ictx->isencoder){
        return SAVED_E_NO_MEDIAFILE;
    }

    *ach = codecContext->decoderctx->audioResampleCtx->tgt->ch;
    *fmt= codecContext->decoderctx->audioResampleCtx->tgt->fmt;
    *sample = codecContext->decoderctx->audioResampleCtx->tgt->sample;
    return  SAVED_OP_OK;
}

int saved_internal_get_video_par(SAVEDInternalContext *ictx,int* vw, int* vh, int* fmt){
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    SAVEDCodecContext *codecContext = (SAVEDCodecContext*)ictx->savctx;
    if(ictx->isencoder){
        return SAVED_E_NO_MEDIAFILE;
    }
    *vw = codecContext->decoderctx->videoScaleCtx->tgt->width;
    *vh = codecContext->decoderctx->videoScaleCtx->tgt->height;
    *fmt= codecContext->decoderctx->videoScaleCtx->tgt->fmt;
    return  SAVED_OP_OK;
}