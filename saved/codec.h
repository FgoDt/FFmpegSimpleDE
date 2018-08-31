#ifndef __CODEC_H__
#define __CODEC_H__

#include "saved.h"
#include "format.h"
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include "videoscale.h"



typedef struct SAVEDDecoderContext {
    int use_hw;
    char *hw_name;
    AVBufferRef *hw_bufferref;

    AVCodecContext *actx; //audio
    AVCodecContext *vctx; //video
    AVCodecContext *sctx; //sub

    AVFrame *idst_frame; // internal swsscale frame
    AVFrame *isrc_frame; // internal avcodec decode frame
    AVFrame *ihw_frame;  // internal hard ware decode frame

    AVPacket *ipkt;  // internal packet
    uint8_t *picswbuf;//

    SAVEDVideoScaleCtx *videoScaleCtx;

}SAVEDDecoderContext;


typedef struct SAVEDEncoderContext {
    int use_hw;
    char *hw_name;
    AVBufferRef *hw_bufferref;
}SAVEDEncoderContext;



typedef struct SAVEDCodecContext{

    SAVEDDecoderContext *decoderctx;
    int isencoder;
}SAVEDCodecContext;

SAVEDCodecContext* saved_codec_alloc();

int saved_codec_open(SAVEDCodecContext *savctx, SAVEDFormat *fmt);

int saved_codec_close(SAVEDCodecContext *savctx);


int saved_codec_send_pkt(SAVEDCodecContext *ictx, SAVEDPkt *pkt);
int saved_codec_get_pkt(SAVEDCodecContext *ictx, SAVEDPkt *pkt);
int saved_codec_send_frame(SAVEDCodecContext *ictx, SAVEDFrame *f);
int saved_codec_get_frame(SAVEDCodecContext *ictx, SAVEDFrame *f);

#endif // !__CODEC_H__
