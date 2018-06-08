#include "decoder.h"

#ifdef  __ANDROID_NDK__
#include <jni.h>
#include <libavcodec/jni.h>
#endif


int saved_decoder_create(SAVEDInternalContext *ictx) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(ictx->savctx) SAVED_E_USE_NULL;

    SAVEDCodecContext *savctx = ictx->savctx;
    AVStream *astream = ictx->fmt->astream;
    AVStream *vstream = ictx->fmt->vstream;


    if (astream)
    {
        savctx->actx = avcodec_alloc_context3(NULL);
        if ((avcodec_parameters_to_context(savctx->actx, astream->codecpar)) < 0) {
            SAVLOGE("can't parse par to context");
            return SAVED_E_AVLIB_ERROR;
        }
        AVCodec *acodec = avcodec_find_decoder(savctx->actx->codec_id);
        if (acodec == NULL)
        {
            SAVLOGE("can't find decoder for audio ");
            return SAVED_E_AVLIB_ERROR;
        }
        SAVLOGD("find audio default decoder: %s", acodec->name);
        
    }
    if (vstream)
    {
        savctx->vctx = avcodec_alloc_context3(NULL);
        if ((avcodec_parameters_to_context(savctx->vctx, vstream->codecpar)) < 0) {
            SAVLOGE("can't parse par to context");
            return SAVED_E_AVLIB_ERROR;
        }
        AVCodec *vcodec = avcodec_find_decoder(savctx->vctx->codec_id);
        if (vcodec == NULL)
        {
            SAVLOGE("can't find decoder for video");
            return SAVED_E_AVLIB_ERROR;
        }
        SAVLOGD("find video default decoder: %s", vcodec->name);

#if __ANDROID_NDK__

        if (av_jni_get_java_vm(NULL)==NULL)
        {
            goto skip_mc;
        }

        AVCodec *MCCodec = NULL;
        char *codec_name = NULL;
        switch (savctx->vctx->codec_id)
        {
        case AV_CODEC_ID_H264:
            codec_name = "h264_mediacodec";
            break;
        case AV_CODEC_ID_HEVC:
            codec_name = "hevc_mediacodec";
            break;
        case AV_CODEC_ID_MPEG2VIDEO:
            codec_name = "mpeg2_mediacodec";
            break;
        case AV_CODEC_ID_MPEG4:
            codec_name = "mpeg4_mediacodec";
            break;
        case AV_CODEC_ID_VP8:
            codec_name = "vp8_mediacodec";
            break;
        case AV_CODEC_ID_VP9:
            codec_name = "vp9_mediacodec";
            break;
        default:
            break;
        }

        MCCodec = avcodec_find_decoder_by_name((const char*)codec_name);

        if (MCCodec == NULL) {
            SAVLOGW("can't nopen android mediacodec ");
            goto skip_mc;
        }

        vcodec = MCCodec;


#endif
        skip_mc:

        //use hardware
        if (savctx->usehw)
        {

        }
    }

}

int saved_decoder_send_pkt(SAVEDInternalContext *ictx, AVPacket *pkt);

int saved_decoder_recive_frame(SAVEDInternalContext *ictx, AVFrame *f);

int saved_decoder_close(SAVEDInternalContext *ictx);