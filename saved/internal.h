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

typedef struct SAVEDInternalContext{
    SAVEDFormat *fmt;
    SAVEDCodecContext *ctx;
    void *savctx;
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
int saved_internal_open(SAVEDInternalContext *ictx, const char* path, const char *options);


int saved_internal_get_pkt(SAVEDInternalContext *ictx, SAVEDPkt *pkt);

int saved_internal_send_pkt(SAVEDInternalContext *ictx, SAVEDPkt *pkt);

int saved_internal_get_frame(SAVEDInternalContext *ictx, SAVEDFrame *f);

int saved_internal_send_frame(SAVEDInternalContext *ictx, SAVEDFrame *f);

#endif 