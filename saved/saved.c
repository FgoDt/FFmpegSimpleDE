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


SAVEDContext* saved_context_alloc() {
    saved_set_log_level(SAVEDLOG_LEVEL_D);
    SAVEDContext *ctx = (SAVEDContext*)malloc(sizeof(SAVEDContext));
    ctx->ictx = saved_internal_alloc();
    ctx->openmark = OPENMARK;
    ctx->audioPar = NULL;
    ctx->picPar = NULL;
    SAVLOGD("create context done");
    return ctx;
}


int saved_open(SAVEDContext *ctx, const char *path, const char *options, int isencoder) {

    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;



    if (path == NULL && !isencoder)
    {
        SAVLOGE("there is no path for decoder use");
        return SAVED_E_USE_NULL;
    }

    int ret =  saved_internal_open(ctx->ictx, path, options);
    if (ret != SAVED_OP_OK)
    {
        return ret;
    }

    return ret;
}

int saved_open_with_par(SAVEDContext *ctx, const char *path, const char *options, int isencoder, 
                                                    int vh, int vw, int vfmt, int vbitrate,
                                                    int asample_rate, int ach, int afmt, int abitrate){
    SAVEDInternalContext *ictx = ctx->ictx;
    ictx->isencoder = isencoder;
    int ret = saved_internal_open_with_par(ctx->ictx,path,options,vh,vw,vbitrate,ach,asample_rate,abitrate);
    return ret;
}
int saved_open_with_vcodec(SAVEDContext *ctx,SAVEDContext *vctx, const char *path, const char *options, int isencoder,
                           int vh, int vw, int vfmt, int vbitrate,
                           int asample_rate, int ach, int afmt, int abitrate){
    SAVEDInternalContext *ictx = ctx->ictx;
    ictx->isencoder = isencoder;
    int ret = saved_internal_open_with_vcodec(ctx->ictx,vctx->ictx,path,options,vh,vw,vbitrate,ach,asample_rate,abitrate);
    return ret;
	
}

int saved_open_encoder_width_codec(SAVEDContext *ctx, SAVEDContext *src_ctx, int codec_copy_flag, const char *options) {
	RETIFNULL(ctx) SAVED_E_USE_NULL;
	RETIFNULL(src_ctx) SAVED_E_USE_NULL;
	if (codec_copy_flag<0)
	{
		SAVLOGE("code copy flag error");
		return SAVED_E_FATAL;
	}
	ctx->ictx->isencoder = 1;
	int ret = saved_internal_open_encoder_with_codec(ctx, src_ctx, codec_copy_flag, options);

	return ret;

}

SAVEDPkt* saved_pkt_alloc() {
    SAVEDPkt *pkt = (SAVEDPkt*)malloc(sizeof(SAVEDPkt));
    pkt->data = NULL;
    pkt->duration = -1;
    pkt->internalPkt = av_packet_alloc();
    pkt->pts = -1;
    pkt->size = -1;
    pkt->type = AVMEDIA_TYPE_UNKNOWN;
    pkt->useinternal = 0;
    return pkt;
}

int saved_pkt_unref(SAVEDPkt *pkt){
    RETIFNULL(pkt) SAVED_E_USE_NULL;
    if(pkt->internalPkt != NULL){
        av_packet_unref(pkt->internalPkt);

    }
    if(pkt->data!=NULL){
        free(pkt->data);
        pkt->data = NULL;
    }
    return SAVED_OP_OK;
}

int saved_pkt_free(SAVEDPkt *pkt) {
    RETIFNULL(pkt) SAVED_E_USE_NULL;

    if (pkt->data)
    {
        free(pkt->data);
        pkt->data = NULL;
    }
    if (pkt->internalPkt)
    {
        av_packet_unref(pkt->internalPkt);
        av_packet_free((AVPacket**)&pkt->internalPkt);
    }
    free(pkt);
    return  SAVED_OP_OK;
}

SAVEDFrame* saved_frame_alloc() {
    SAVEDFrame *f = (SAVEDFrame*)malloc(sizeof(SAVEDFrame));
    f->data = NULL;
    f->duration = -1;
    f->internalframe = av_frame_alloc();
    f->pts = -1;
    f->size = -1;
    f->type = AVMEDIA_TYPE_UNKNOWN;
    f->useinternal = 1;
    return f;
}

int saved_frame_unref(SAVEDFrame *savedFrame){
    RETIFNULL(savedFrame) SAVED_E_USE_NULL;
    if(savedFrame->internalframe!=NULL){
        av_frame_unref(savedFrame->internalframe);
    }
    if(savedFrame->data!=NULL){
        free(savedFrame->data);
        savedFrame->data = NULL;
    }
    return  SAVED_OP_OK;
}

int saved_frame_free(SAVEDFrame *savedFrame) {
    RETIFNULL(savedFrame) SAVED_E_USE_NULL;
    if (savedFrame->data)
    {
        free(savedFrame->data);
    }
    if (savedFrame->internalframe) {
        av_frame_unref(savedFrame->internalframe);
        av_frame_free((AVFrame **)&savedFrame->internalframe);
    }
    free(savedFrame);
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

int saved_get_frame(SAVEDContext *ctx, SAVEDFrame *f){

    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(f) SAVED_E_USE_NULL;

    int ret = saved_internal_get_frame(ctx->ictx,f);
    return ret;
}

int saved_send_frame(SAVEDContext *ctx, SAVEDFrame *f) {

    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(f) SAVED_E_USE_NULL;
    RETIFNULL(f->internalframe) SAVED_E_USE_NULL;

    AVFrame *iframe = (AVFrame*)f->internalframe;
    iframe->pts = f->pts;
    iframe->pkt_duration = f->duration;
    iframe->format = f->fmt;
    if(f->type == SAVED_MEDIA_TYPE_AUDIO){
        iframe->nb_samples = f->nb_sample;
        iframe->channels = f->ch;
        if(iframe->data[0] == NULL){
            av_frame_get_buffer(iframe,0);

        }
        avcodec_fill_audio_frame(iframe,f->ch,f->fmt,f->data,f->size,0);
    }
    if(f->type == SAVED_MEDIA_TYPE_VIDEO){
        iframe->width =f->width;
        iframe->height = f->height;
        iframe->format = AV_PIX_FMT_YUV420P;
        if(iframe->data[0] == NULL){
            av_frame_get_buffer(iframe,0);
        }
        int ysize = f->size/1.5;
        memcpy(iframe->data[0],f->data,ysize);
        memcpy(iframe->data[1],f->data+ysize,ysize/4);
        memcpy(iframe->data[2],f->data+ysize+ysize/4,ysize/4);

    }



//    if(f->fmt<AV_SAMPLE_FMT_U8P){
//        iframe->linesize[0]=iframe->channels*iframe->nb_samples*av_get_bytes_per_sample(iframe->format);
//    } else{
//        iframe->linesize[0]=iframe->nb_samples*av_get_bytes_per_sample(iframe->format);
//    }

    int ret = saved_internal_send_frame(ctx->ictx, f);
    return ret;
}

int saved_get_frame_raw(const SAVEDFrame *src, unsigned char *data, int linesize[4]) {
    RETIFNULL(data) SAVED_E_USE_NULL;
	RETIFNULL(src) SAVED_E_USE_NULL;
	int len = 0;
	for (size_t i = 0; i < 8 && src->data[i] != NULL; i++)
	{
		memcpy(data + len, src->data[i], src->linesize[i] * src->height);
	}
}

int saved_get_pkt_raw(const SAVEDPkt *src, unsigned char * data, int size) {
    RETIFNULL(data) SAVED_E_USE_NULL;
	RETIFNULL(src) SAVED_E_USE_NULL;
}

int saved_close(SAVEDContext *ctx){
    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;

    saved_internal_close(ctx->ictx);
    free(ctx);
    return  SAVED_OP_OK;
}

int saved_get_metadata(SAVEDContext *ctx, char *key, char **val){
    RETIFNULL(ctx) SAVED_E_USE_NULL;
    int ret = saved_internal_get_metatdata(ctx->ictx,key,val);
    return  ret;
}

int saved_set_audio_par(SAVEDContext *ctx,int ch, int sample_rate, int fmt){
    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;
    if(ctx->audioPar == NULL){
        ctx->audioPar = (SAVEDAudioPar*)malloc(sizeof(SAVEDAudioPar));
    }
    ctx->audioPar->ch = ch;
    ctx->audioPar->sample = sample_rate;
    ctx->audioPar->fmt = fmt;
	int ret = saved_internal_set_audio_par(ctx->ictx, ctx->audioPar);
    return  SAVED_OP_OK;
}

int saved_set_video_par(SAVEDContext *ctx, int w, int h, int fmt){
    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;
	if (ctx->picPar == NULL)
	{
		ctx->picPar = (SAVEDVideoPar*)malloc(sizeof(SAVEDVideoPar));
	}
	ctx->picPar->fmt = fmt;
	ctx->picPar->width = w;
	ctx->picPar->height = h;
    int ret = saved_internal_set_video_par(ctx->ictx,ctx->picPar);
    return ret;

}

int saved_get_audio_par(SAVEDContext *ctx,int* ch, int* sample_rate, int* fmt){
    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;
    int ret = saved_internal_get_audio_par(ctx->ictx,ch,sample_rate,fmt);
    return  ret;
}

int saved_get_video_par(SAVEDContext *ctx,int *w, int *h, int *fmt){
    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;
    int ret = saved_internal_get_video_par(ctx->ictx,w,h,fmt);
    return ret;
}

int saved_seek(SAVEDContext *ctx,double pts){
    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;
    int ret = saved_internal_seek(ctx->ictx,pts);
    return  ret;
};

int saved_get_duration(SAVEDContext *ctx, double *duration){
    RETIFCTXNULL(ctx) SAVED_E_USE_NULL;
    *duration = -1;
    if(ctx->ictx!=NULL && ctx->ictx->fmt!= NULL &&
        ctx->ictx->fmt->fmt != NULL){
        if(ctx->ictx->fmt->fmt->nb_streams>0){
            *duration = av_q2d(ctx->ictx->fmt->fmt->streams[0]->time_base)*ctx->ictx->fmt->fmt->streams[0]->duration;
        }
    }
    return SAVED_OP_OK;
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
