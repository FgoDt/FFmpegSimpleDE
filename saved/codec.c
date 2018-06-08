#include "codec.h"
#include "encoder.h"
#include "decoder.h"

int saved_codec_send_pkt(SAVEDInternalContext *ictx, SAVEDPkt *pkt) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(pkt) SAVED_E_USE_NULL;

    int ret = SAVED_E_UNDEFINE;

    if (ictx->isencoder)
    {
       ret = saved_format_send_pkt();
    }
    else
    {
       ret = saved_decoder_send_pkt();
    }

    return ret;

}

int saved_codec_get_pkt(SAVEDInternalContext *ictx, SAVEDPkt *pkt) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(pkt) SAVED_E_USE_NULL;

    int ret = SAVED_E_UNDEFINE;

    if (ictx->isencoder)
    {
        ret = saved_encoder_recive_pkt();
    }
    else
    {
        ret = saved_format_get_pkt(ictx->fmt, pkt);
    }
    return ret;
}


int saved_codec_send_frame(SAVEDInternalContext *ictx, SAVEDFrame *f) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(f) SAVED_E_USE_NULL;

    int ret = SAVED_E_UNDEFINE;

    if (ictx->isencoder)
    {
        ret = saved_encoder_send_frame();
    }
    else
    {
        ret = SAVED_E_FATAL;
    }
    return ret;
}
int saved_codec_get_frame(SAVEDInternalContext *ictx, SAVEDFrame *f) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(f) SAVED_E_USE_NULL;

    int ret = SAVED_E_UNDEFINE;

    if (ictx->isencoder)
    {
        ret = SAVED_E_FATAL;
    }
    else
    {
        ret = saved_decoder_recive_frame();
    }

    return ret;
}
