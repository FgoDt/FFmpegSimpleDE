#pragma once
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include "format.h"
#include "internal_context.h"


typedef struct SAVEDCodec {
    AVCodecContext *actx; //audio
    AVCodecContext *vctx; //video
    AVCodecContext *sctx; //sub
    int usehw;
}SAVEDCodecContext;


int saved_codec_send_pkt(SAVEDInternalContext *ictx, SAVEDPkt *pkt);
int saved_codec_get_pkt(SAVEDInternalContext *ictx, SAVEDPkt *pkt);
int saved_codec_send_frame(SAVEDInternalContext *ictx, SAVEDFrame *f);
int saved_codec_get_frame(SAVEDInternalContext *ictx, SAVEDFrame *f);
