//
// Created by rt-zl on 2019/2/19.
//

#ifndef SAMPLE_MEDIACODEC_NDK_DECODER_H
#define SAMPLE_MEDIACODEC_NDK_DECODER_H

#include <media/NdkMediaCodec.h>
#include <android/native_window_jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "saved.h"
#include "android_utils.h"

typedef struct SAVEDMediaCodecCtx{
    AMediaFormat *vmediaformt;
    AMediaCodec *vcodec;
    ANativeWindow *window;
    SAVEDAndroidData *androidData;
}SAVEDMediaCodecCtx;

SAVEDMediaCodecCtx * saved_mediacodec_alloc();

int saved_mediacodec_open(SAVEDMediaCodecCtx *ctx);

int saved_mediacodec_sendpkt(SAVEDMediaCodecCtx *ctx,SAVEDPkt *pkt);

int saved_mediacodec_recivepkt(SAVEDMediaCodecCtx *ctx,SAVEDFrame *frame);

void saved_mediacodec_close(SAVEDMediaCodecCtx *ctx);

#endif //SAMPLE_MEDIACODEC_NDK_DECODER_H
