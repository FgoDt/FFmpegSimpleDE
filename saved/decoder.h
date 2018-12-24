#ifndef __DECODER_H__
#define __DECODER_H__



#include "codec.h"
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>


SAVEDDecoderContext* saved_decoder_alloc();

int saved_decoder_init(SAVEDDecoderContext *ictx, SAVEDFormat *fmt, char *hwname);

int saved_decoder_create(SAVEDDecoderContext *ictx,char *name,AVStream *audiostream, AVStream *videostream, AVStream *substream);

/*
* @return 0 on success, otherwise negative error code:
*      AVERROR(EAGAIN):   input is not accepted in the current state - user
*                         must read output with avcodec_receive_frame() (once
*                         all output is read, the packet should be resent, and
*                         the call will not fail with EAGAIN).
*      AVERROR_EOF:       the decoder has been flushed, and no new packets can
*                         be sent to it (also returned if more than 1 flush
*                         packet is sent)
*      AVERROR(EINVAL):   codec not opened, it is an encoder, or requires flush
*      AVERROR(ENOMEM):   failed to add packet to internal queue, or similar
*      other errors: legitimate decoding errors
*/

int saved_decoder_send_pkt(SAVEDDecoderContext *ictx,SAVEDPkt *pkt);

int saved_decoder_recive_frame(SAVEDDecoderContext *ictx, AVFrame *f, enum AVMediaType type) ;

void saved_decoder_close(SAVEDDecoderContext *ictx);

#endif // !__DECODER_H__