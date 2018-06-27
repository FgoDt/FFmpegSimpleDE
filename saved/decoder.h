#ifndef __DECODER_H__
#define __DECODER_H__



#include "codec.h"
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>


SAVEDDecoderContext* saved_decoder_alloc();

int saved_decoder_init(SAVEDDecoderContext *ictx, SAVEDFormat *fmt, char *hwname);

int saved_decoder_create(SAVEDDecoderContext *ictx,char *name,AVStream *audiostream, AVStream *videostream, AVStream *substream);

int saved_decoder_send_pkt(SAVEDDecoderContext *ictx,AVPacket *pkt);

int saved_decoder_recive_frame(SAVEDDecoderContext *ictx, AVFrame *f);

int saved_decoder_close(SAVEDDecoderContext *ictx);

#endif // !__DECODER_H__