#include "codec.h"
#include "encoder.h"
#include "decoder.h"
#include "define.h"
#include "decoder.h"
#include "saved.h"

SAVEDCodecContext *saved_codec_alloc(){
    SAVEDCodecContext *ctx = (SAVEDCodecContext*)malloc(sizeof(*ctx));
    RETIFNULL(ctx) NULL;
	memset(ctx, 0, sizeof(SAVEDCodecContext));
    ctx->isencoder = -1;
    ctx->decoder_ctx = NULL;
    ctx->encoder_ctx = NULL;
    return ctx;
}

int saved_codec_close(SAVEDCodecContext *savctx){
    RETIFNULL(savctx) SAVED_E_USE_NULL;

    if (savctx->decoder_ctx){
        saved_decoder_close(savctx->decoder_ctx);
        savctx->decoder_ctx = NULL;
    }
    if(savctx->encoder_ctx){
        saved_encoder_close(savctx->encoder_ctx);
        savctx->encoder_ctx = NULL;
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
		if (ictx->decoder_ctx == NULL) {
			ictx->decoder_ctx = saved_decoder_alloc();
		}
        return saved_decoder_init(ictx->decoder_ctx,fmt,NULL);
    }

    return  SAVED_E_UNDEFINE;
}

int saved_codec_open_with_option(SAVEDCodecContext *savctx,SAVEDFormat *fmt, AVDictionary *options) {
	if (savctx->isencoder)
	{
		if (savctx->encoder_ctx== NULL) {
			savctx->encoder_ctx = saved_encoder_alloc();
		}
	}
	else
	{
		if (savctx->decoder_ctx == NULL) {
			savctx->decoder_ctx = saved_decoder_alloc();
		}
	}
}


int saved_codec_open_with_par(SAVEDCodecContext *savctx, int vh, int vw, int vfmt,int vbit_rate,
                         int asample_rate, int afmt, int ach, int abit_rate ){
    RETIFNULL(savctx) SAVED_E_USE_NULL;

    if(savctx->isencoder){
		if (savctx->encoder_ctx == NULL) {
			savctx->encoder_ctx = saved_encoder_alloc();
		}
        int ret = saved_encoder_open_with_par(savctx->encoder_ctx,vw,vh,vbit_rate,asample_rate,ach,abit_rate);
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
       ret = saved_decoder_send_pkt(ictx->decoder_ctx,pkt);
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
        ret = saved_encoder_recive_pkt(ictx->encoder_ctx,pkt);
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
       ret = saved_encoder_send_frame(ictx->encoder_ctx,f);
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
        ret = saved_decoder_recive_frame(ictx->decoder_ctx,f->internalframe,f->type);
        if(ret == SAVED_OP_OK && f->type == SAVED_MEDIA_TYPE_VIDEO){
            int ysize = ictx->decoder_ctx->idst_frame->linesize[0] * ictx->decoder_ctx->video_scale_ctx->tgt->height;

            int usize = ictx->decoder_ctx->idst_frame->linesize[1] *ictx->decoder_ctx->video_scale_ctx->tgt->height/2;
            int vsize = ictx->decoder_ctx->idst_frame->linesize[2] *ictx->decoder_ctx->video_scale_ctx->tgt->height/2;
            int yuvsize = ysize+usize+vsize;

            if(ictx->decoder_ctx->idst_frame->format == AV_PIX_FMT_YUV420P){
                if(f->data == NULL){
                    f->data = (unsigned char*)malloc(yuvsize);
                    memset(f->data,0,yuvsize);
                }
            memcpy(f->data,ictx->decoder_ctx->idst_frame->data[0],ysize);
            memcpy(f->data+ysize,ictx->decoder_ctx->idst_frame->data[1],usize);
            memcpy(f->data+(int)(ysize+usize),ictx->decoder_ctx->idst_frame->data[2],vsize);
            } else if(ictx->decoder_ctx->idst_frame->format == AV_PIX_FMT_NV12){
                ysize = ictx->decoder_ctx->idst_frame->linesize[0]*ictx->decoder_ctx->idst_frame->height;
                int uvsize = ictx->decoder_ctx->idst_frame->linesize[1] *ictx->decoder_ctx->idst_frame->height;
                yuvsize = ysize+uvsize;
                if(f->data == NULL){
                    f->data = (unsigned char*)malloc(yuvsize);
                }
                memcpy(f->data,ictx->decoder_ctx->idst_frame->data[0],ysize);
                memcpy(f->data+ysize,ictx->decoder_ctx->idst_frame->data[1],uvsize);
            } else{
                //fixme
            }
            //printf("video src pts:%ld\n",ictx->decoderctx->isrc_frame->pts);
            f->pts = av_q2d(ictx->decoder_ctx->v_time_base)* ictx->decoder_ctx->isrc_frame->pts;
            ictx->decoder_ctx->decpts = f->pts;
            f->duration = av_q2d(ictx->decoder_ctx->v_time_base)* ictx->decoder_ctx->isrc_frame->pkt_duration;
            f->fmt  = ictx->decoder_ctx->idst_frame->format;
            f->width = ictx->decoder_ctx->video_scale_ctx->tgt->width;
            f->height = ictx->decoder_ctx->video_scale_ctx->tgt->height;
            f->size = yuvsize;
            memcpy(f->linesize,ictx->decoder_ctx->idst_frame->linesize,8*sizeof(int));

        }

        if(ret == SAVED_OP_OK && f->type == SAVED_MEDIA_TYPE_AUDIO){
            AVFrame *iframe = f->internalframe;
            f->size = iframe->nb_samples*ictx->decoder_ctx->audio_resample_ctx->tgt->ch
                    *av_get_bytes_per_sample(ictx->decoder_ctx->audio_resample_ctx->tgt->fmt);
            if(f->data != NULL){
                free(f->data);
            }
                f->data = (unsigned char *) malloc(f->size);
            //fixme when fmt is plane
            memcpy(f->data,ictx->decoder_ctx->iadst_frame->data[0],f->size);
            f->pts =av_q2d(ictx->decoder_ctx->a_time_base) * ictx->decoder_ctx->isrc_frame->pts;
            f->duration = av_q2d(ictx->decoder_ctx->a_time_base)* ictx->decoder_ctx->isrc_frame->pkt_duration;
            ictx->decoder_ctx->decpts = f->pts;
            f->nb_sample = iframe->nb_samples;
            f->fmt = ictx->decoder_ctx->audio_resample_ctx->tgt->fmt;
            f->ch = ictx->decoder_ctx->audio_resample_ctx->tgt->ch;
        }
    }

    return ret;
}

int saved_codec_set_force_video_par(SAVEDCodecContext *ictx, SAVEDVideoPar *par) {
	RETIFNULL(ictx) SAVED_E_USE_NULL;
	RETIFNULL(par) SAVED_E_USE_NULL;
	if (ictx->isencoder ==1)
	{
		//todo
	}
	else
	{
		if (ictx->decoder_ctx == NULL)
		{
			ictx->decoder_ctx = saved_decoder_alloc();
		}
		saved_decoder_set_video_par(ictx->decoder_ctx, par);
	}
	return SAVED_OP_OK;
}

int saved_codec_set_force_audio_par(SAVEDCodecContext *ictx, SAVEDAudioPar *par) {
	RETIFNULL(ictx) SAVED_E_USE_NULL;
	RETIFNULL(par) SAVED_E_USE_NULL;
	if (ictx->isencoder == 1) {
		//todo
	}
	else
	{
		if (ictx->decoder_ctx == NULL)
		{
			ictx->decoder_ctx = saved_decoder_alloc();
		}
		saved_decoder_set_audio_par(ictx->decoder_ctx, par);
	}

	return SAVED_OP_OK;
}

int saved_codec_get_video_par(SAVEDCodecContext *ictx, SAVEDVideoPar *par) {
	RETIFNULL(ictx) SAVED_E_USE_NULL;
	RETIFNULL(par) SAVED_E_USE_NULL;
	int ret = SAVED_E_UNDEFINE;
	if (ictx->isencoder)
	{
		//todo
	}
	else
	{
		if (ictx->decoder_ctx == NULL)
		{
			ictx->decoder_ctx = saved_decoder_alloc();
		}
		ret = saved_decoder_get_video_par(ictx, par);
	}
	return ret;
}

int saved_codec_get_audio_par(SAVEDCodecContext *ictx, SAVEDAudioPar *par) {
	RETIFNULL(ictx) SAVED_E_USE_NULL;
	RETIFNULL(par) SAVED_E_USE_NULL;
	int ret = SAVED_E_UNDEFINE;
	if (ictx->isencoder)
	{
		//todo
	}
	else
	{
		if (ictx->decoder_ctx == NULL)
		{
			ictx->decoder_ctx = saved_decoder_alloc();
		}
		ret = saved_decoder_get_audio_par(ictx, par);
	}
	return ret;
}
