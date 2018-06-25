#ifndef __DECODER_H__
#define __DECODER_H__



#include "internal_context.h"
#include <libswresample/swresample.h>

typedef struct SAVEDDecoderContext {
    int use_hw;
    char *hw_name;
    AVBufferRef *hw_bufferref;
    
}SAVEDDecoderContext;

int saved_decoder_create(SAVEDInternalContext *ictx,char *name);

int saved_decoder_send_pkt(SAVEDInternalContext *ictx,AVPacket *pkt);

int saved_decoder_recive_frame(SAVEDInternalContext *ictx, AVFrame *f);

int saved_decoder_close(SAVEDInternalContext *ictx);

#endif // !__DECODER_H__