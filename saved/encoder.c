#include "encoder.h"
#include "define.h"
#include "videoscale.h"
#include "saved.h"

SAVEDEncoderContext * saved_encoder_alloc(){
    SAVEDEncoderContext *ctx = (SAVEDEncoderContext*)malloc(sizeof(SAVEDEncoderContext));
    RETIFNULL(ctx) NULL;
    memset(ctx,0,sizeof(SAVEDEncoderContext));
    return ctx;
}

int saved_encoder_open(SAVEDEncoderContext *ctx, const char *options){
    RETIFNULL(ctx) SAVED_E_USE_NULL;
    if(options  != NULL){
    }

    //default encoder aac h264

    AVCodec *acodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if(acodec == NULL){
        SAVLOGE("find aac encoder error\n");
        goto OPEN_ERROR;
    }

    AVCodec *vcodec = avcodec_find_encoder(AV_CODEC_ID_H264);

    if(vcodec == NULL){
        SAVLOGE("find h264 encoder error \n");
        goto OPEN_ERROR;
    }



    OPEN_ERROR:
    saved_encoder_close(ctx);
    return SAVED_E_AVLIB_ERROR;
}

int saved_encoder_open_with_par(SAVEDEncoderContext *ctx, 
                                                                        int vw, int vh , int vbit_rate, 
                                                                        int asample_rate, int ach, int abit_rate){
    RETIFNULL(ctx)  SAVED_E_USE_NULL;
    if(vw <=0 || vh <= 0){
        goto done_vencoder;
    }
    AVCodec *vcodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if(vcodec == NULL){
        goto done_vencoder;
    }

    ctx->vctx = avcodec_alloc_context3(vcodec);
    if(vcodec == NULL)
        goto error_vencoder;

    ctx->vctx->width = vw;
    ctx->vctx->height = vh;
    ctx->vctx->time_base = (AVRational){1,1000};
    ctx->vctx->pix_fmt = AV_PIX_FMT_YUV420P;
   // ctx->vctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    ctx->videoScaleCtx = saved_video_scale_alloc();
    if(ctx->videoScaleCtx == NULL){
        goto error_vencoder;
    }

    int ret = saved_video_scale_set_picpar(ctx->videoScaleCtx->tgt,AV_PIX_FMT_YUV420P,vh,vw);
    if(ret<0){
        goto error_vencoder;
    }

    if(vbit_rate>0)
        ctx->vctx->bit_rate = vbit_rate;
    ret = avcodec_open2(ctx->vctx,vcodec,NULL);

    if(ret<0){
        SAVLOGW("avcodec_open2 error on vcodec \n");
        goto error_vencoder;
    }
    goto done_vencoder;

    error_vencoder:
        avcodec_close(ctx->vctx);
        avcodec_free_context(&ctx->vctx);
        ctx->vctx = NULL;

    done_vencoder:

    if(asample_rate <= 0 || ach <= 0 ){
        goto done_aencoder;
    }

    AVCodec *acodec = avcodec_find_encoder_by_name("libfdk_aac");
    if(acodec == NULL){
        SAVLOGW("find aac encoder error \n");
        goto done_aencoder;
    }

    ctx->actx = avcodec_alloc_context3(acodec);
    if(ctx->actx == NULL){
        SAVLOGW("avcodec alloc context error  no mem \n");
        goto error_aencoder;
    }

    ctx->actx->channels = ach;
    ctx->actx->channel_layout = av_get_default_channel_layout(ach);
    ctx->actx->sample_rate = asample_rate;
    ctx->actx->sample_fmt = AV_SAMPLE_FMT_S16;
    ctx->actx->time_base = (AVRational){1,1000};
   // ctx->actx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    ctx->audioResampleCtx = saved_resample_alloc();
    if(NULL== ctx->audioResampleCtx){
        goto error_aencoder;
    }

    ret = saved_resample_set_fmtpar(ctx->audioResampleCtx->tgt,AV_SAMPLE_FMT_S16,ach,asample_rate);
    if (ret <0 ){
        goto error_aencoder;
    }
    
    ret = avcodec_open2(ctx->actx,acodec,NULL);
    if(ret<0){
        goto error_aencoder;
    }
    goto done_aencoder;
    
    error_aencoder:
        avcodec_close(ctx->actx);
        avcodec_free_context(&ctx->actx);
        ctx->actx = NULL;
        return SAVED_E_FATAL;

    done_aencoder:
    return SAVED_OP_OK;
}


static int check_video_scacle(SAVEDEncoderContext *ctx, SAVEDFrame *frame){
    SAVEDPicPar *par = ctx->videoScaleCtx->src;
    AVFrame *f = (AVFrame*)frame->internalframe;
    if(par->fmt != f->format ||
        par->height != f->height ||
        par->width != f->width
    ){
        SAVEDVideoScaleCtx *vsctx = saved_video_scale_alloc();
       int ret =  saved_video_scale_set_picpar(vsctx->tgt,ctx->videoScaleCtx->tgt->fmt,
                                                ctx->videoScaleCtx->tgt->height,ctx->videoScaleCtx->tgt->width);
       ret |= saved_video_scale_set_picpar(vsctx->src,f->format,f->height,f->width);
       saved_video_scale_open(vsctx);
       if(ret<0){
           return SAVED_E_UNDEFINE;
       }

    saved_video_scale_close(ctx->videoScaleCtx);
    ctx->videoScaleCtx = vsctx;
    }
    return SAVED_OP_OK;
}
static int check_audio_resampler(SAVEDEncoderContext *ctx, SAVEDFrame *frame){
    SAVEDAudioPar *par = ctx->audioResampleCtx->src;
    AVFrame *f = (AVFrame*)frame->internalframe;
    if(par->fmt != f->format ||
        par->ch != f->channels||
        par->sample != f->sample_rate
    ){
        SAVEDAudioResampleCtx *arctx = saved_resample_alloc();
       int ret =  saved_resample_set_fmtpar(arctx->src,f->format,f->channels,f->sample_rate);

       ret |= saved_resample_set_fmtpar(arctx->tgt,ctx->audioResampleCtx->tgt->fmt,
              ctx->audioResampleCtx->tgt->ch,ctx->audioResampleCtx->tgt->sample);

       ret |= saved_resample_open(arctx);
       if(ret<0){
           return SAVED_E_UNDEFINE;
       }

    saved_resample_close(ctx->audioResampleCtx);
    ctx-> audioResampleCtx= arctx;
    }
    return SAVED_OP_OK;
}


static  int set_audio_fifo(SAVEDEncoderContext *ctx){
    RETIFNULL(ctx) SAVED_E_USE_NULL;
    if(ctx->fifo!=NULL){
        av_audio_fifo_free(ctx->fifo);
    }
    //alloc 10s fifo
    ctx->fifo = av_audio_fifo_alloc(ctx->audioResampleCtx->tgt->fmt,ctx->audioResampleCtx->tgt->ch,ctx->audioResampleCtx->tgt->sample*10);
    return SAVED_OP_OK;
}

FILE *tf = NULL;
void test_encoder(char *name,int n, void *data){
    if(tf == NULL){
        tf = fopen(name,"wb");
        fwrite(data,1,n,tf);
    }
    fwrite(data,1,n,tf);
}

int saved_encoder_send_frame(SAVEDEncoderContext *ctx, SAVEDFrame  *frame){
    RETIFNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(frame) SAVED_E_USE_NULL;
    int ret = 0;
    AVCodecContext *ictx = NULL;
    if(frame->type == SAVED_MEDIA_TYPE_AUDIO){
        if(ctx->fifo == NULL){
            set_audio_fifo(ctx);
        }
        ictx = ctx->actx;
        if(ictx != NULL && frame->internalframe != NULL){
            ret = check_audio_resampler(ctx,frame);
            if(ret<0){
                return ret;
            }
            if(ctx->iadst_frame == NULL){
                ctx->iadst_frame = av_frame_alloc();
                ctx->iadst_frame->format = ctx->audioResampleCtx->tgt->fmt;
                ctx->iadst_frame->channels = ctx->audioResampleCtx->tgt->ch;

                ctx->iadst_frame->sample_rate = ctx->audioResampleCtx->tgt->sample;
                //dont know how many samples need alloc
                ctx->iadst_frame->nb_samples = 20480 +256;
                ctx->iadst_frame->channel_layout= av_get_default_channel_layout(ctx->iadst_frame->channels);
                av_frame_get_buffer(ctx->iadst_frame,0);
            }
            ret = saved_resample(ctx->audioResampleCtx,frame->internalframe,ctx->iadst_frame->data);
            if(ret<0){
                SAVLOGE("resample audio error \n");
                return ret;
            }

            int fifo_full = 0;
            if(av_audio_fifo_space(ctx->fifo)-av_audio_fifo_size(ctx->fifo)<ret){
                SAVLOGD("audio fifo full need call read pkt function\n");
                fifo_full = 1;
            }
            if(fifo_full == 0){
                av_audio_fifo_write(ctx->fifo,ctx->iadst_frame->data,ret);
            }
            if(av_audio_fifo_size(ctx->fifo)>1024){
                AVFrame *enf = ctx->iadst_frame;
                enf->nb_samples = 1024;
                enf->channels = ctx->audioResampleCtx->tgt->ch;
                enf->channel_layout = ctx->audioResampleCtx->tgt->ch_layout;
                enf->format = ctx->audioResampleCtx->tgt->fmt;
                enf->sample_rate = ctx->audioResampleCtx->tgt->sample;
                ret = av_audio_fifo_peek(ctx->fifo,enf->data,1024);
                if(ret!=1024){
                    SAVLOGE("not enough pcm data for encode\n");
                    goto  done_enf;
                }
                double one_frame_time = (double)1024/ctx->audioResampleCtx->tgt->sample;
                enf->pts = ctx->aenpts*1000;
                enf->pkt_duration =one_frame_time*1000;
                // ipkt->dts = av_rescale_q_rnd(ipkt->dts,stream->time_base,stream->time_base,AV_ROUND_INF|AV_ROUND_PASS_MINMAX);
                ret = avcodec_send_frame(ictx,enf);
                if(ret == 0){
                    av_audio_fifo_drain(ctx->fifo,1024);
                    ctx->aenpts += one_frame_time;
                }
                done_enf:
                if(fifo_full== 1){
                    return  AVERROR(EAGAIN);
                }
                return  ret;
            }
        }
        SAVLOGW("ctx->actx is NULL or frame->internalframe is NULL\n");
        ictx = NULL;
    }

    if(frame->type == SAVED_MEDIA_TYPE_VIDEO){
        ictx = ctx->vctx;
        if(ictx != NULL && frame->internalframe != NULL){
            ret = check_video_scacle(ctx,frame);
            if(ret<0){
                SAVLOGE(" check video scacle error \n");
                return ret;
            }
            if(ctx->ivdst_frame == NULL){
                ctx->ivdst_frame = av_frame_alloc();
                ctx->ivdst_frame->format = AV_PIX_FMT_YUV420P;
                ctx->ivdst_frame->width = ctx->videoScaleCtx->tgt->width;
                ctx->ivdst_frame->height = ctx->videoScaleCtx->tgt->height;
                av_frame_get_buffer(ctx->ivdst_frame,0);
            }
          //  AVFrame *tframe = frame->internalframe;
          //  int val = frame->pts*1000;
           // int ysize = tframe->linesize[0]*tframe->height;
           // memset(tframe->data[0],val%255,ysize);
           // memset(tframe->data[1],val%255,ysize/4);
           // memset(tframe->data[2],val%255,ysize/4);
            ret = saved_video_scale(ctx->videoScaleCtx,frame->internalframe,ctx->ivdst_frame);
           // tframe = ctx->ivdst_frame;
            ctx->ivdst_frame->pts = frame->pts*1000;
            ctx->ivdst_frame->pkt_dts = ctx->ivdst_frame->pts;
            ctx->ivdst_frame->pkt_duration = frame->duration*1000;
            if(ret<0){
                SAVLOGE("video scale error \n");
                return ret;
            }
            ret = avcodec_send_frame(ictx,ctx->ivdst_frame);
            ictx = NULL;
            return ret;
        }
        SAVLOGW("ctx->vctx is NULL or frame->internalframe is NULL\n");
        ictx = NULL;
    }
    return SAVED_E_NO_MEDIAFILE;
}



int fff = 0;

int saved_encoder_recive_pkt(SAVEDEncoderContext *ctx,SAVEDPkt *pkt){
    RETIFNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(pkt) SAVED_E_USE_NULL;

int ret = 0;
if(pkt->type == SAVED_MEDIA_TYPE_AUDIO){
    ret = avcodec_receive_packet(ctx->actx,pkt->internalPkt);
    return ret;
}

if(pkt->type == SAVED_MEDIA_TYPE_VIDEO){
    AVPacket *ipkt = pkt->internalPkt;
    ret = avcodec_receive_packet(ctx->vctx,ipkt);
    if(ret == 0){
        if(fff == 0){
           fff = 1;
            test_encoder("./abc.h264",ctx->vctx->extradata_size,ctx->vctx->extradata);
        }
        test_encoder("./abc.h264",ipkt->size,ipkt->data);
    }
    return ret;
}

    return  SAVED_E_NO_MEDIAFILE;
}

void  saved_encoder_close(SAVEDEncoderContext *ctx){
    if(NULL != ctx->vctx){
        avcodec_flush_buffers(ctx->vctx);
        avcodec_close(ctx->vctx);
        avcodec_free_context(&ctx->vctx);
        ctx->vctx = NULL;
    }
    if(NULL != ctx->actx){
        avcodec_close(ctx->actx);
        avcodec_free_context(&ctx->actx);
        ctx->actx = NULL;
    }

    if(NULL!= ctx->audioResampleCtx){
        saved_resample_close(ctx->audioResampleCtx);
        ctx->audioResampleCtx = NULL;
    }
    if(NULL!= ctx->videoScaleCtx){
        saved_video_scale_close(ctx->videoScaleCtx);
        ctx->videoScaleCtx = NULL;
    }
    if(NULL!= ctx->iadst_frame){
        av_frame_unref(ctx->iadst_frame);
        av_frame_free(&ctx->iadst_frame);
        ctx->iadst_frame = NULL;
    }
    if(NULL!= ctx->ivdst_frame){
        av_frame_unref(ctx->ivdst_frame);
        av_frame_free(&ctx->ivdst_frame);
        ctx->ivdst_frame = NULL;
    }
    if(NULL!= ctx->ipkt){
        av_packet_unref(ctx->ipkt);
        av_packet_free(&ctx->ipkt);
        ctx->ipkt = NULL;
    }
    if(NULL!= ctx->sctx){
        avcodec_close(ctx->sctx);
        avcodec_free_context(&ctx->sctx);
    }
    if(NULL!=ctx->fifo){
        av_audio_fifo_free(ctx->fifo);
    }
    if(NULL!=ctx->iadst_frame){
        av_frame_unref(ctx->iadst_frame);
        av_frame_free(&ctx->iadst_frame);
    }
    if(tf!=NULL){
        fclose(tf);
    }
    free(ctx);
}
