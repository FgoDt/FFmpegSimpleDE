#ifndef __INTERNAL_CONTEXT_H__
#define __INTERNAL_CONTEXT_H__


#include "saved.h"
#include "codec.h"
#include "format.h"
#include<libavformat/avformat.h>
#include<libavcodec/avcodec.h>
#include<libswresample/swresample.h>
#include<libswscale/swscale.h>
#include<libavutil/time.h>
#include<libavutil/avutil.h>

typedef  struct SAVECodecContext SAVECodecContext;

typedef struct SAVEDInternalContext{
    SAVEDFormat *fmt;
    SAVEDCodecContext *savctx;
	AVDictionary *options;
    int isencoder;
}SAVEDInternalContext;

/**
 * create internal ctx
 **/
SAVEDInternalContext* saved_internal_alloc();

/**
 * open input 
 * @param ictx 
 * @param path media file path can be net path or local path
 * @param options open option
 * return >= 0 if ok 
 **/
int saved_internal_open(SAVEDInternalContext *ictx, const char* path, void *options);

int saved_internal_open_with_par(SAVEDInternalContext *ictx, const char *path, const char *options,
                                                                        int vh, int vw, int vbit_rate, 
                                                                        int ach, int asample_rate, int abit_rate);
int saved_internal_open_with_vcodec(SAVEDInternalContext *ictx,SAVEDInternalContext *ivctx, const char *path, const char *options,
                                 int vh, int vw, int vbit_rate,
                                 int ach, int asample_rate, int abit_rate);

int saved_internal_open_encoder_with_codec(SAVEDInternalContex *ictx, SAVEDInternalContex *src_ctx, int copy_flags, const char *options);

int saved_internal_close(SAVEDInternalContext *ictx);

int saved_internal_get_pkt(SAVEDInternalContext *ictx, SAVEDPkt *pkt);

int saved_internal_send_pkt(SAVEDInternalContext *ictx, SAVEDPkt *pkt);

int saved_internal_get_frame(SAVEDInternalContext *ictx, SAVEDFrame *f);

int saved_internal_send_frame(SAVEDInternalContext *ictx, SAVEDFrame *f);

int saved_internal_get_metatdata(SAVEDInternalContext *ictx,char *key,char **val);

int saved_internal_set_audio_par(SAVEDInternalContext *ictx, SAVEDAudioPar *par);

int saved_internal_set_video_par(SAVEDInternalContext *ictx, SAVEDVideoPar *par);

int saved_internal_get_audio_par(SAVEDInternalContext *ictx,SAVEDAudioPar *par);

int saved_internal_get_video_par(SAVEDInternalContext *ictx,SAVEDVideoPar *par);

int saved_internal_seek(SAVEDInternalContext *ictx,double pts);

int saved_internal_set_option(const char *key, const char *val);

int saved_internal_get_option(const char *key,char **val);

#endif