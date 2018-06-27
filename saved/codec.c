#include "codec.h"
#include "encoder.h"
#include "decoder.h"
#include "define.h"
#include "decoder.h"

SAVEDCodecContext *saved_codec_alloc(){
    SAVEDCodecContext *ctx = (SAVEDCodecContext*)malloc(sizeof(*ctx));
    RETIFNULL(ctx) NULL;
    ctx->isencoder = -1;
    ctx->decoderctx = NULL;
    return ctx;
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
       ret = saved_decoder_send_pkt(NULL, NULL);
    }

    return ret;

}

int saved_codec_get_pkt(SAVEDCodecContext *ictx, SAVEDPkt *pkt) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(pkt) SAVED_E_USE_NULL;

    int ret = SAVED_E_UNDEFINE;

    if (ictx->isencoder)
    {
        ret = saved_encoder_recive_pkt(NULL,NULL);
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
        return SAVED_E_UNDEFINE;
//        ret = saved_encoder_send_frame();
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

    int ret = SAVED_E_UNDEFINE;

    if (ictx->isencoder)
    {
        SAVLOGE("ictx is encoder can not call get frame");
        ret = SAVED_E_FATAL;
    }
    else
    {
        ret = saved_decoder_recive_frame(NULL,NULL);
    }

    return ret;
}
