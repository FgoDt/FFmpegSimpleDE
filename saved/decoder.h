#pragma once
#include "internal_context.h"

int saved_decoder_create(SAVEDInternalContext *ictx);

int saved_decoder_send_pkt(SAVEDInternalContext *ictx,AVPacket *pkt);

int saved_decoder_recive_frame(SAVEDInternalContext *ictx, AVFrame *f);

int saved_decoder_close(SAVEDInternalContext *ictx);