#ifndef __CODEC_H__
#define __CODEC_H__

#include "saved.h"
#include "format.h"
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include "videoscale.h"
#include "audioresample.h"
#include <libavutil/audio_fifo.h>



typedef struct SAVEDDecoderContext {
    int use_hw;
    char *hw_name;
    AVBufferRef *hw_bufferref;

    AVCodecContext *actx; //audio
    AVCodecContext *vctx; //video
    AVCodecContext *sctx; //sub

    AVRational a_time_base;
    AVRational v_time_base;
    AVRational s_time_base;

    AVFrame *idst_frame; // internal swsscale frame
    AVFrame *isrc_frame; // internal avcodec decode frame
    AVFrame *ihw_frame;  // internal hard ware decode frame

    AVPacket *ipkt;  // internal packet
    uint8_t *picswbuf;//

    AVFrame *iadst_frame; //audio dst frame;

    SAVEDVideoScaleCtx *videoScaleCtx;
    SAVEDAudioResampleCtx *audioResampleCtx;

}SAVEDDecoderContext;


typedef struct SAVEDEncoderContext {
    int use_hw;
    char *hw_name;
    AVBufferRef *hw_bufferref;

    AVCodecContext *actx; //audio
    AVCodecContext *vctx; //video
    AVCodecContext *sctx; //sub

    AVRational a_time_base;
    AVRational v_time_base;
    AVRational s_time_base;

    AVFrame *ivdst_frame; // video scale frame
    AVFrame *iadst_frame; // audio resample frame

    AVPacket *ipkt;  // internal packet

    SAVEDVideoScaleCtx *videoScaleCtx;
    SAVEDAudioResampleCtx *audioResampleCtx;

    AVAudioFifo *fifo;
    double aenpts;
    double venpts;

}SAVEDEncoderContext;



typedef struct SAVEDCodecContext{

    SAVEDDecoderContext *decoderctx;
    SAVEDEncoderContext *encoderctx;
    int isencoder;
}SAVEDCodecContext;

SAVEDCodecContext* saved_codec_alloc();

int saved_codec_open(SAVEDCodecContext *savctx, SAVEDFormat *fmt);

int saved_codec_open_with_par(SAVEDCodecContext *savctx, int vh, int vw, int vfmt,int vbit_rate,
                                                                                                                                int asample_rate, int afmt, int ach, int abit_rate );

int saved_codec_close(SAVEDCodecContext *savctx);


int saved_codec_send_pkt(SAVEDCodecContext *ictx, SAVEDPkt *pkt);
int saved_codec_get_pkt(SAVEDCodecContext *ictx, SAVEDPkt *pkt);
int saved_codec_send_frame(SAVEDCodecContext *ictx, SAVEDFrame *f);
int saved_codec_get_frame(SAVEDCodecContext *ictx, SAVEDFrame *f);

#endif // !__CODEC_H__
