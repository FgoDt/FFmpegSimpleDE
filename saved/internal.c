
#include "internal.h"
#include "encoder.h"
#include "decoder.h"
#include "log.h"
#include "saved.h"
#include <libavutil/avutil.h>

SAVEDInternalContext* saved_internal_alloc() {
    SAVEDInternalContext *ctx = (SAVEDInternalContext*)malloc(sizeof(SAVEDInternalContext));
    if (ctx == NULL)
    {
        SAVLOGE("no mem");
        return NULL;
    }
	memset(ctx, 0, sizeof(SAVEDInternalContex));
	ctx->savctx = saved_codec_alloc();
    ctx->isencoder = 0;
    ctx->fmt = saved_format_alloc();

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

static int parse_options(AVDictionary **dict, const char *options) {
	RETIFNULL(options) SAVED_E_USE_NULL;
	RETIFNULL(dict) SAVED_E_USE_NULL;

	char key[100] = { 0 };
	char val[100] = { 0 };
	int new_key_val = 0;
	int is_key = 0;
	int key_index = 0;
	int val_index = 0;
	int has_key_val = 0;
	if (options[0]!='-')
	{
		return -1;
	}
	while (options[0]!='\0'&&options[0]!=NULL)
	{
		if (options[0] == '-')
		{
			new_key_val = 1;
			is_key = 1;
			key_index = 0;
			val_index = 0;
			options++;
			continue;
		}
		if (options[0] == ' ') {
			is_key = 0;
			options++;
			continue;
		}
		if (new_key_val) {
			if (has_key_val) {
				av_dict_set(dict, key, val, 0);
				has_key_val = 0;
			}
			memset(key, 0, 100);
			memset(val, 0, 100);
			new_key_val = 0;
		}
		if (is_key ) {
			if (key_index < 100) {
				key[key_index] = options[0];
				key_index++;
				has_key_val = 1;
			}
		}
		else
		{
			if (val_index < 100) {
				val[val_index] = options[0];
				val_index++;
				has_key_val = 1;
			}
		}
		options++;

	}
	//last loop key val not store
	if (has_key_val) {
		av_dict_set(dict, key, val, 0);
	}
	return 0;
	
}

int saved_internal_open(SAVEDInternalContext *ictx,const char* path, void *options) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;


    int ret = 0;
	ret = parse_options(&ictx->options, options);
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

int saved_internal_open_with_par(SAVEDInternalContext *ictx, const char *path, const char *options,
                                                                        int vh, int vw, int vbit_rate, 
                                                                        int ach, int asample_rate, int abit_rate){
    if(ictx->isencoder){
        int ret = saved_codec_open_with_par(ictx->savctx,vh,vw,NULL,vbit_rate,asample_rate,NULL,ach,abit_rate);

        if(path != NULL){
            SAVEDCodecContext *enctx = (SAVEDCodecContext*)ictx->savctx;
            ret =  saved_format_open_output(ictx->fmt,enctx->encoder_ctx,path,options);
        }

        return ret;
    }

    return SAVED_E_UNDEFINE;
}

int saved_internal_open_with_vcodec(SAVEDInternalContext *ictx,SAVEDInternalContext *ivctx, const char *path, const char *options,
                                    int vh, int vw, int vbit_rate,
                                    int ach, int asample_rate, int abit_rate){
     if(ictx->isencoder){
        int ret = saved_codec_open_with_par(ictx->savctx,vh,vw,NULL,vbit_rate,asample_rate,NULL,ach,abit_rate);

        if(path != NULL){
            SAVEDCodecContext *enctx = (SAVEDCodecContext*)ictx->savctx;
            ret =  saved_format_open_output_with_vpar(ictx->fmt,enctx->encoder_ctx,ivctx->fmt->vstream,path,options);
        }

        return ret;
    }

    return SAVED_E_UNDEFINE;
}

int saved_internal_open_encoder_with_codec(SAVEDInternalContex *ictx, SAVEDInternalContex *src_ctx, int copy_flags, const char *options) {
	RETIFNULL(ictx) SAVED_E_USE_NULL;
	RETIFNULL(src_ctx) SAVED_E_USE_NULL;

	int ret = parse_options(ictx->options, options);

	ret = saved_codec_open
}

int saved_internal_get_pkt(SAVEDInternalContext *ictx, SAVEDPkt *pkt) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(pkt) SAVED_E_USE_NULL;

    int ret = SAVED_E_UNDEFINE;

    if (ictx->isencoder)
    {
       ret =  saved_codec_get_pkt(ictx->savctx,pkt);
       saved_copy_pkt_dsc(pkt);
       if(pkt->data!=NULL){
           free(pkt->data);
       }
       struct AVPacket *ipkt = (struct AVPacket*)pkt->internalPkt;
       pkt->data = (unsigned char*)malloc(pkt->size);
       memcpy(pkt->data,ipkt->data,pkt->size);
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
        pkt->data = (unsigned char*)malloc(pkt->size);
        memcpy(pkt->data,ipkt->data,pkt->size);
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


int saved_internal_set_audio_par(SAVEDInternalContext *ictx,SAVEDAudioPar *par){
    RETIFNULL(ictx) SAVED_E_USE_NULL;
	int ret = saved_codec_set_force_audio_par(ictx->savctx, par);
    return ret;
}

int saved_internal_set_video_par(SAVEDInternalContext *ictx,SAVEDVideoPar *par){
    RETIFNULL(ictx) SAVED_E_USE_NULL;
	int ret = saved_codec_set_force_video_par(ictx->savctx, par);
    return  ret;
}

int saved_internal_get_audio_par(SAVEDInternalContext *ictx,SAVEDAudioPar *par){
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    if(ictx->isencoder){
        return SAVED_E_NO_MEDIAFILE;
    }
	int ret = saved_codec_get_audio_par(ictx, par);
    return  ret;
}

int saved_internal_get_video_par(SAVEDInternalContext *ictx, SAVEDVideoPar *par){
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    if(ictx->isencoder){
        return SAVED_E_NO_MEDIAFILE;
    }
	int ret = saved_codec_get_video_par(ictx->savctx, par);
    return  ret;
}

int saved_internal_seek(SAVEDInternalContext *ictx,double pts){
    RETIFNULL(ictx) SAVED_E_USE_NULL;
    RETIFNULL(ictx->fmt) SAVED_E_USE_NULL;
    if(ictx->isencoder){
        return SAVED_E_AVLIB_ERROR;
    }
    double rel = pts - ictx->savctx->decoder_ctx->decpts;
    rel *=1000000;
    int64_t seek_target = pts*1000*1000;
    int64_t seek_min    = rel > 0 ? seek_target - rel + 2: INT64_MIN;
    int64_t seek_max    = rel < 0 ? seek_target - rel - 2: INT64_MAX;

    int ret =avformat_seek_file(ictx->fmt->fmt,-1,seek_min,seek_target,seek_max,AVSEEK_FLAG_BACKWARD);
    return ret;
}

int saved_internal_set_option(SAVEDInternalContext *ictx,const char *key, const char *val) {
	RETIFNULL(key) SAVED_E_USE_NULL;
	RETIFNULL(val) SAVED_E_USE_NULL;
	RETIFNULL(ictx) SAVED_E_USE_NULL;
	int ret = av_dict_set(&ictx->options, key, val, 0);
	return ret;
}

int saved_internal_get_option(SAVEDInternalContext *ictx,const char *key, char **val) {
	RETIFNULL(key) SAVED_E_USE_NULL;
	RETIFNULL(val) SAVED_E_USE_NULL;
	RETIFNULL(ictx) SAVED_E_USE_NULL;
	AVDictionaryEntry *entry;
	entry = av_dict_get(ictx->options, key, NULL, 0);
	if (entry != NULL) {
		*val = malloc(strlen(entry->value) + 1);
		(*val)[strlen(entry->value)] = 0;
		memcpy(*val, entry->value, strlen(entry->value));
	}
	return 0;
}
