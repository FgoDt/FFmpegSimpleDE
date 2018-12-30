#include "codec.h"
#include "encoder.h"
#include "decoder.h"
#include "define.h"
#include "decoder.h"
#include "saved.h"

SAVEDCodecContext *saved_codec_alloc(){
    SAVEDCodecContext *ctx = (SAVEDCodecContext*)malloc(sizeof(*ctx));
    RETIFNULL(ctx) NULL;
    ctx->isencoder = -1;
    ctx->decoderctx = NULL;
    return ctx;
}

int saved_codec_close(SAVEDCodecContext *savctx){
    RETIFNULL(savctx) SAVED_E_USE_NULL;

    if (savctx->decoderctx){
        saved_decoder_close(savctx->decoderctx);
        savctx->decoderctx = NULL;
    }
    if(savctx->encoderctx){
        saved_encoder_close(savctx->encoderctx);
        savctx->encoderctx = NULL;
    }

    free(savctx);
    SAVLOGD("saved codec close");
    return SAVED_OP_OK;
}

int saved_codec_open(SAVEDCodecContext *ictx, SAVEDFormat *fmt){
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(fmt) SAVED_E_USE_NULL;

    if(ictx->isencoder){
        return SAVED_E_FATAL;
    }else{
        ictx->decoderctx = saved_decoder_alloc();
        return saved_decoder_init(ictx->decoderctx,fmt,NULL);
    }

    return  SAVED_E_UNDEFINE;
}


int saved_codec_open_with_par(SAVEDCodecContext *savctx, int vh, int vw, int vfmt,int vbit_rate,
                                                                                                                                int asample_rate, int afmt, int ach, int abit_rate ){

    if(savctx->isencoder){
        savctx->encoderctx= saved_encoder_alloc();
        int ret = saved_encoder_open_with_par(savctx->encoderctx,vw,vh,vbit_rate,asample_rate,ach,abit_rate);
        return ret;
    }
    return SAVED_E_FATAL;
}


int saved_codec_send_pkt(SAVEDCodecContext  *ictx, SAVEDPkt *pkt) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(pkt) SAVED_E_USE_NULL;

    int ret = SAVED_E_UNDEFINE;

    if (ictx->isencoder)
    {
        SAVLOGE("ictx is encoder can not call send pkt");
        return SAVED_E_FATAL;
    }
    else
    {
       ret = saved_decoder_send_pkt(ictx->decoderctx,pkt);
       //ret = saved_decoder_send_pkt(NULL, NULL);
    }

    return ret;

}

int saved_codec_get_pkt(SAVEDCodecContext *ictx, SAVEDPkt *pkt) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(pkt) SAVED_E_USE_NULL;

    int ret = SAVED_E_UNDEFINE;

    if (ictx->isencoder)
    {
        ret = saved_encoder_recive_pkt(ictx->encoderctx,pkt);
    }
    else
    {
        SAVLOGE("ictx is decoder can not call get pkt");
        return SAVED_E_FATAL;
    }
    return ret;
}


int saved_codec_send_frame(SAVEDCodecContext *ictx, SAVEDFrame *f) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(f) SAVED_E_USE_NULL;

    int ret = SAVED_E_UNDEFINE;

    if (ictx->isencoder)
    {
        saved_encoder_send_frame(ictx->encoderctx,f);
    }
    else
    {
        SAVLOGE("ictx is decoder can not call send frame");
        ret = SAVED_E_FATAL;
    }
    return ret;
}
int saved_codec_get_frame(SAVEDCodecContext *ictx, SAVEDFrame *f) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(f) SAVED_E_USE_NULL;

    int ret;

    if (ictx->isencoder)
    {
        SAVLOGE("ictx is encoder can not call get frame");
        ret = SAVED_E_FATAL;
    }
    else
    {
        ret = saved_decoder_recive_frame(ictx->decoderctx,f->internalframe,f->type);
        if(ret == SAVED_OP_OK && f->type == SAVED_MEDIA_TYPE_VIDEO){
            int yuvsize = ictx->decoderctx->videoScaleCtx->tgt->width * ictx->decoderctx->videoScaleCtx->tgt->height * 1.5;

            if(f->data == NULL){
                f->data = (unsigned char*)malloc(yuvsize);
            }
            int ysize = yuvsize/1.5;
            memcpy(f->data,ictx->decoderctx->idst_frame->data[0],ysize);
            memcpy(f->data+ysize,ictx->decoderctx->idst_frame->data[1],ysize/4);
            memcpy(f->data+(int)(ysize*1.25),ictx->decoderctx->idst_frame->data[2],ysize/4);
            f->pts = av_q2d(ictx->decoderctx->v_time_base)* ictx->decoderctx->isrc_frame->pts;
            f->duration = av_q2d(ictx->decoderctx->v_time_base)* ictx->decoderctx->idst_frame->pkt_duration;
            f->fmt  = ictx->decoderctx->videoScaleCtx->tgt->fmt;
            f->size = yuvsize;

        }

        if(ret == SAVED_OP_OK && f->type == SAVED_MEDIA_TYPE_AUDIO){
            AVFrame *iframe = f->internalframe;
            f->size = iframe->nb_samples*ictx->decoderctx->audioResampleCtx->tgt->ch
                    *av_get_bytes_per_sample(ictx->decoderctx->audioResampleCtx->tgt->fmt);
            if(f->data != NULL){
                free(f->data);
            }
                f->data = (unsigned char *) malloc(f->size);
            memcpy(f->data,ictx->decoderctx->iadst_frame->extended_data,f->size);
            f->pts =av_q2d(ictx->decoderctx->a_time_base) * ictx->decoderctx->isrc_frame->pts;
            f->duration = av_q2d(ictx->decoderctx->a_time_base)* ictx->decoderctx->isrc_frame->pkt_duration;
            f->nb_sample = iframe->nb_samples;
            f->fmt = ictx->decoderctx->audioResampleCtx->tgt->fmt;
            f->ch = ictx->decoderctx->audioResampleCtx->tgt->ch;
        }
    }

    return ret;
}
