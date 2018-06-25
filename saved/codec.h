#ifndef __CODEC_H__
#define __CODEC_H__

#include "saved.h"
#include "internal_context.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include "decoder.h"
#include "format.h"


typedef struct SAVEDCodec {
    AVCodecContext *actx; //audio
    AVCodecContext *vctx; //video
    AVCodecContext *sctx; //sub
    SAVEDDecoderContext *decoderctx;
}SAVEDCodecContext;


int saved_codec_send_pkt(SAVEDInternalContext *ictx, SAVEDPkt *pkt);
int saved_codec_get_pkt(SAVEDInternalContext *ictx, SAVEDPkt *pkt);
int saved_codec_send_frame(SAVEDInternalContext *ictx, SAVEDFrame *f);
int saved_codec_get_frame(SAVEDInternalContext *ictx, SAVEDFrame *f);

#endif // !__CODEC_H__
