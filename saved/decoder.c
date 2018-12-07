#include "decoder.h"
#include <libavutil/hwcontext.h>
#include <libavutil/imgutils.h>
#include "log.h"
#include "define.h"
#include "saved.h"
#include "videoscale.h"

#ifdef  __ANDROID_NDK__
#include <jni.h>
#include <libavcodec/jni.h>
#endif

enum AVPixelFormat saved_find_fmt_by_hw_type(const enum AVHWDeviceType type) {
    enum AVPixelFormat fmt;
    switch (type)
    {
    case AV_HWDEVICE_TYPE_VAAPI:
        fmt = AV_PIX_FMT_VAAPI;
        break;
    case AV_HWDEVICE_TYPE_CUDA:
        fmt = AV_PIX_FMT_CUDA;
        break;
    case AV_HWDEVICE_TYPE_D3D11VA:
        fmt = AV_PIX_FMT_D3D11;
        break;
    case AV_HWDEVICE_TYPE_DXVA2:
        fmt = AV_PIX_FMT_DXVA2_VLD;
        break;
    case AV_HWDEVICE_TYPE_VIDEOTOOLBOX:
        fmt = AV_PIX_FMT_VIDEOTOOLBOX;
        break;
    default:
        fmt = AV_PIX_FMT_NONE;
        break;
    }

    return fmt;
}

enum AVPixelFormat saved_get_hw_format(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts) {
    const enum AVPixelFormat *p;
    for (p = pix_fmts; *p != -1; p++)
    {
        if (*p == ctx->pix_fmt)
        {
            return *p;
        }
    }
    return AV_PIX_FMT_NONE;
}

int saved_hw_decoder_init(SAVEDDecoderContext *ctx, const enum AVHWDeviceType type) {
    RETIFNULL(ctx) SAVED_E_USE_NULL;

    if (0!=av_hwdevice_ctx_create(&ctx->hw_bufferref,type,NULL,NULL,0))
    {
       SAVLOGW("create hw device error");
        
        return SAVED_OP_OK;
    }

    ctx->vctx->hw_device_ctx = av_buffer_ref(ctx->hw_bufferref);

    return SAVED_OP_OK;

}



SAVEDDecoderContext* saved_decoder_alloc() {
    SAVEDDecoderContext *savctx = (SAVEDDecoderContext*)malloc(sizeof(SAVEDDecoderContext));
    memset(savctx,0,sizeof(SAVEDDecoderContext));
    if (savctx==NULL)
    {
        SAVLOGE("no mem");
        return NULL;
    }
    savctx->ipkt = av_packet_alloc();
    savctx->isrc_frame = av_frame_alloc();
    savctx->idst_frame = av_frame_alloc();
    return savctx;
}
int saved_decoder_close(SAVEDDecoderContext *ictx){
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    if(ictx->ipkt) {
        av_packet_unref(ictx->ipkt);
        av_packet_free(&ictx->ipkt);
    }
    if(ictx->isrc_frame)
        av_frame_free(&ictx->isrc_frame);
    if(ictx->idst_frame)
        av_frame_free(&ictx->idst_frame);
    if(ictx->videoScaleCtx){
        saved_video_scale_close(ictx->videoScaleCtx);
    }
    if(ictx->audioResampleCtx){
        saved_resample_close(ictx->audioResampleCtx);
    }
    if(ictx->picswbuf){
        free(ictx->picswbuf);
    }
    if(ictx->audiobuf){
        free(ictx->audiobuf);
    }
    if(ictx->vctx){
        avcodec_close(ictx->vctx);
        avcodec_free_context(&ictx->vctx);
    }
    if(ictx->actx){
        avcodec_close(ictx->actx);
        avcodec_free_context(&ictx->actx);
    }
    if(ictx->sctx){
        avcodec_close(ictx->sctx);
        avcodec_free_context(&ictx->sctx);
    }
    if(ictx->hw_bufferref){
        av_buffer_unref(&ictx->hw_bufferref);
    }
    if(ictx->ihw_frame){
        av_frame_free(&ictx->ihw_frame);
        ictx->ihw_frame = NULL;
    }
    if(ictx->hw_name){
        free(ictx->hw_name);
        ictx->hw_name = NULL;
    }
    free(ictx);
}

int saved_decoder_init(SAVEDDecoderContext *ictx, SAVEDFormat *fmt, char *hwname){
    return  saved_decoder_create(ictx,hwname,fmt->astream,fmt->vstream,fmt->astream);
}

static  int set_video_scale(SAVEDDecoderContext *ctx){
    RETIFNULL(ctx)SAVED_E_USE_NULL;
    RETIFNULL(ctx->vctx) SAVED_E_USE_NULL;
    ctx->videoScaleCtx = saved_video_scale_alloc();
    RETIFNULL(ctx->videoScaleCtx) SAVED_E_NO_MEM;

    saved_video_scale_set_picpar(ctx->videoScaleCtx->src,ctx->vctx->pix_fmt,ctx->vctx->height,ctx->vctx->width);
    ctx->videoScaleCtx->usehw = ctx->use_hw;
    if(ctx->use_hw){
        saved_video_scale_set_picpar(ctx->videoScaleCtx->src,AV_PIX_FMT_NV12,ctx->vctx->height,ctx->vctx->width);
    }
    //default output pix format yuv420p
    saved_video_scale_set_picpar(ctx->videoScaleCtx->tgt,AV_PIX_FMT_YUV420P,ctx->vctx->height,ctx->vctx->width);
    int ret = saved_video_scale_open(ctx->videoScaleCtx);

    ctx->picswbuf = (uint8_t*)malloc(av_image_get_buffer_size(ctx->videoScaleCtx->tgt->fmt,
            ctx->videoScaleCtx->tgt->width,ctx->videoScaleCtx->tgt->height,1));

    av_image_fill_arrays(ctx->idst_frame->data,ctx->idst_frame->linesize,ctx->picswbuf,ctx->videoScaleCtx->tgt->fmt,
    ctx->videoScaleCtx->tgt->width,ctx->videoScaleCtx->tgt->height,1);

    return  ret;
}

static  int set_audio_resample(SAVEDDecoderContext *ctx){
    RETIFNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(ctx->actx) SAVED_E_USE_NULL;

    ctx->audioResampleCtx = saved_resample_alloc();
    RETIFNULL(ctx->audioResampleCtx) SAVED_E_NO_MEM;

   int ret =  saved_resample_set_fmtpar(ctx->audioResampleCtx->tgt,AV_SAMPLE_FMT_FLT,ctx->actx->channels,ctx->actx->sample_rate);

   ret |= saved_resample_set_fmtpar(ctx->audioResampleCtx->src,ctx->actx->sample_fmt,ctx->actx->channels,ctx->actx->sample_rate);

   ret |= saved_resample_open(ctx->audioResampleCtx);

   if(ret!=SAVED_OP_OK)
       return ret;

   ctx->audiobuf = (uint8_t*)malloc(10240*10*10);
   RETIFNULL(ctx->audiobuf) SAVED_E_NO_MEM;

   return SAVED_OP_OK;


}

int saved_decoder_create(SAVEDDecoderContext *ictx,char *chwname,AVStream *audiostream, AVStream *videostream, AVStream *substream) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;

    SAVEDDecoderContext *savctx = ictx;
    AVStream *astream = audiostream;
    AVStream *vstream = videostream;
    AVCodec *acodec = NULL;
    AVCodec *vcodec = NULL;

    //create audio deocer
    if (astream) {
        savctx->actx = avcodec_alloc_context3(NULL);
        if ((avcodec_parameters_to_context(savctx->actx, astream->codecpar)) < 0) {
            SAVLOGE("can't parse par to context");
            return SAVED_E_AVLIB_ERROR;
        }
        acodec = avcodec_find_decoder(savctx->actx->codec_id);
        if (acodec == NULL) {
            SAVLOGE("can't find decoder for audio ");
            return SAVED_E_AVLIB_ERROR;
        }
        SAVEDLOG1(NULL, SAVEDLOG_LEVEL_D, "find audio default decoder: %s", acodec->name);

    }

    //create video decoder
    if (vstream) {
        savctx->vctx = avcodec_alloc_context3(NULL);
        if ((avcodec_parameters_to_context(savctx->vctx, vstream->codecpar)) < 0) {
            SAVLOGE("can't parse par to context");
            return SAVED_E_AVLIB_ERROR;
        }
        vcodec = avcodec_find_decoder(savctx->vctx->codec_id);
        if (vcodec == NULL) {
            SAVLOGE("can't find decoder for video");

            return SAVED_E_AVLIB_ERROR;
        }
        SAVEDLOG1(NULL, SAVEDLOG_LEVEL_D, "find video default decoder: %s", vcodec->name);

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


        skip_mc:
#endif


        //test code
        savctx->use_hw = 1;
        //end test

        //use hardware
        if (savctx->use_hw) {
            enum AVHWDeviceType hwdevice;
            hwdevice = AV_HWDEVICE_TYPE_NONE;

            char *hwname = NULL;

            if (chwname != NULL) {
                hwname = chwname;
                goto set_hw_name_done;
            }

#if _WIN32 || _WIN64
            hwname = "dxva2";
#endif // _WIN32||_WIN64

#if TARGET_OS_IPHONE
            hwname = "videotoolbox";
#endif

#if defined(linux)
            hwname = "vaapi";
#endif



            set_hw_name_done:

            savctx->hw_name =(char*)malloc(strlen(hwname)+1);
            memcpy(savctx->hw_name, hwname, strlen(hwname));

            hwdevice = av_hwdevice_find_type_by_name(hwname);

            if (hwdevice == AV_HWDEVICE_TYPE_NONE) {
                SAVEDLOG1(NULL, SAVEDLOG_LEVEL_W, "can not find hwdevice by name :%s", hwname);
                goto ERROR_ON_USE_HW;
            }

            enum AVHWDeviceType  typtmp = av_hwdevice_iterate_types(AV_HWDEVICE_TYPE_NONE);

            int nb_hwdevices = 0;
            do{
                typtmp = av_hwdevice_iterate_types(typtmp);

                if(typtmp != AV_HWDEVICE_TYPE_NONE){
                    nb_hwdevices++;
                }

                if (typtmp == hwdevice){
                    break;
                }
            }while (typtmp != AV_HWDEVICE_TYPE_NONE);

            if (hwname == NULL || nb_hwdevices == 0) {
                goto ERROR_ON_USE_HW;
            }

            if(typtmp != hwdevice && nb_hwdevices>0){
                hwdevice = av_hwdevice_iterate_types(AV_HWDEVICE_TYPE_NONE);
            }
            const char* default_hw_name =  av_hwdevice_get_type_name(hwdevice);

            if(savctx->hw_name!=NULL){
                free(savctx->hw_name);
            }

            savctx->hw_name =(char*)malloc(strlen(default_hw_name)+1);
            savctx->hw_name[strlen(default_hw_name)] = 0;
            memcpy(savctx->hw_name, default_hw_name, strlen(default_hw_name));


            enum AVPixelFormat hwpixfmt = saved_find_fmt_by_hw_type(hwdevice);

            if (hwpixfmt == -1) {
                SAVEDLOG1(NULL, SAVEDLOG_LEVEL_W, "can not find hw fmt by hwname :%s", hwname);
                goto ERROR_ON_USE_HW;
            }


            savctx->vctx->pix_fmt = hwpixfmt;
            savctx->vctx->get_format = saved_get_hw_format;

            if (saved_hw_decoder_init(savctx, hwdevice) == SAVED_OP_OK) {
                savctx->use_hw = 1;
                SAVLOGD("init hw decoder done");
            } else {

                ERROR_ON_USE_HW:
                SAVLOGW("init hw decoder error");
                if(savctx->hw_bufferref != NULL)
                    av_buffer_unref(&savctx->hw_bufferref);
                savctx->use_hw = 0;
            }

        }


    }

    skip_hw:

    if (acodec != NULL && 0 != avcodec_open2(savctx->actx, acodec, NULL)) {
        SAVLOGE("audio codec open error");
    }

    if (vcodec != NULL && 0 != avcodec_open2(savctx->vctx, vcodec, NULL)) {
        SAVLOGE("video codec open error");
    }


    if(vstream)
        set_video_scale(ictx);
    if(astream)
        set_audio_resample(ictx);

    return SAVED_OP_OK;

}

 int static saved_audio_decod(SAVEDDecoderContext *ictx, AVPacket *pkt){
     if(ictx->actx == NULL){
         SAVLOGE("no actx");
         return  SAVED_E_USE_NULL;
     }
     if(pkt == NULL){
         SAVLOGE("pkt is NULL");
         return  SAVED_E_USE_NULL;
     }
     //todo
     int ret = avcodec_send_packet(ictx->actx,pkt);
     if(ret == AVERROR(EAGAIN)){
         //SAVLOGD("need more data to decode \0");
     }
     return  ret;
}



int static saved_decode_video(SAVEDDecoderContext *ictx, AVPacket *pkt) {
    if(ictx->vctx == NULL){
        SAVLOGE("no actx");
        return  SAVED_E_USE_NULL;
    }
    if(pkt == NULL){
        SAVLOGE("pkt is NULL");
        return  SAVED_E_USE_NULL;
    }
    int ret = avcodec_send_packet(ictx->vctx,pkt);
    if(ret == AVERROR(EAGAIN)){
      //  SAVLOGD("need more data to decode\n");
    }
    if(ret == 0){
    }
    return  ret;
}


int saved_decoder_send_pkt(SAVEDDecoderContext *ictx, SAVEDPkt *pkt) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(pkt) SAVED_E_USE_NULL;

    if(pkt->type == SAVED_MEDIA_TYPE_AUDIO){
        return saved_audio_decod(ictx,pkt->internalPkt);
    }

    if(pkt->type == SAVED_MEDIA_TYPE_VIDEO){
        return  saved_decode_video(ictx,pkt->internalPkt);
    }

    return SAVED_E_UNDEFINE;
}

int saved_decoder_recive_frame(SAVEDDecoderContext *ictx, AVFrame *f, enum AVMediaType type) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(f) SAVED_E_USE_NULL;

    int ret = AVERROR(EFAULT);
    AVCodecContext *codecContext = NULL;
    switch (type){
        case AVMEDIA_TYPE_AUDIO:
            codecContext = ictx->actx;
            break;
        case AVMEDIA_TYPE_VIDEO:
            codecContext = ictx->vctx;
            break;
        case AVMEDIA_TYPE_SUBTITLE:
            codecContext = ictx->sctx;
            break;
        default:
            break;
    }
    if (codecContext == NULL){
        SAVEDLOG1(NULL,SAVEDLOG_LEVEL_E,"saved recive frame error [ codec is NULL ] mediatype is %d",type);
        return  SAVED_E_USE_NULL;
    }
    ret = avcodec_receive_frame(codecContext,ictx->isrc_frame);

    //video sws
    if(ret == 0 && type == AVMEDIA_TYPE_VIDEO){
        ret = saved_video_scale(ictx->videoScaleCtx,ictx->isrc_frame,ictx->idst_frame);
    }

    //audio swr
    if(ret == 0 && type == AVMEDIA_TYPE_AUDIO){
        ret = saved_resample(ictx->audioResampleCtx,ictx->isrc_frame,ictx->audiobuf);
        if(ret>0) {
            f->nb_samples = ret;
            ret = SAVED_OP_OK;
        }
    }


    return  ret;

}
