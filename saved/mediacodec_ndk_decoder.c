//
// Created by rt-zl on 2019/2/19.
//
// some code from googlesamples/android-ndk
// see https://github.com/googlesamples/android-ndk/blob/master/native-codec/app/src/main/cpp/native-codec-jni.cpp

#include <string.h>
#include <stdlib.h>
#include "mediacodec_ndk_decoder.h"
#include "define.h"

SAVEDMediaCodecCtx * saved_mediacodec_alloc(){
    SAVEDMediaCodecCtx *ctx = (SAVEDMediaCodecCtx*)malloc(sizeof(SAVEDMediaCodecCtx));
    memset(ctx,0, sizeof(SAVEDMediaCodecCtx));
    return ctx;
}

int decode(SAVEDMediaCodecCtx *ctx ){
}

int saved_mediacodec_open(SAVEDMediaCodecCtx *ctx,SAVEDAndroidData *adata,const char *mime,int width, int height){

    if(ctx->window!=NULL){
        ANativeWindow_release(ctx->window);
    }
    ctx->androidData = ctx->androidData;

    JNIEnv  *env = NULL;
    JavaVM  *vm = ctx->androidData->vm;

    int ret = (*vm)->GetEnv(vm,&env,JNI_VERSION_1_6);
    int attach = 0;
    if(ret<0){
        ret = (*vm)->AttachCurrentThread(vm,&env,NULL);
        if(ret < 0){
            SAVLOGE("get jni env error");
            return SAVED_E_FATAL;
        }
        attach = 1;
    }

    ctx->window = ANativeWindow_fromSurface(env,ctx->androidData->gsurface);
    if(ctx->window == NULL){
        SAVLOGE("create window error");
        return  SAVED_E_FATAL;
    }

    ctx->vmediaformt = AMediaFormat_new();
    AMediaFormat *format = ctx->vmediaformt;
    AMediaFormat_setString(format,AMEDIAFORMAT_KEY_MIME,mime);
    AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_WIDTH,width);
    AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_HEIGHT,height);
    AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_FRAME_RATE,24);
    AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_PROFILE,8);

    ctx->vcodec = AMediaCodec_createDecoderByType(mime);

    AMediaCodec_configure(ctx->vcodec,format,NULL,NULL,0);
    AMediaCodec_start(ctx->vcodec);


}

int saved_mediacodec_sendpkt(SAVEDMediaCodecCtx *ctx,SAVEDPkt *pkt){
    int bufidx = AMediaCodec_dequeueInputBuffer(ctx->vcodec,0);
    if(bufidx>0){
        size_t bufsize;
        uint8_t  *buf = AMediaCodec_getInputBuffer(ctx->vcodec,bufidx,&bufsize);
        if(bufsize<pkt->size){
            SAVLOGE("data size too large for MediaCodec");
            return SAVED_E_NO_MEM;
        }

        long long time = pkt->pts * 1000 *1000;
        AMediaCodec_queueInputBuffer(ctx->vcodec,bufidx,0,
                pkt->size,time,0);
        return SAVED_OP_OK;
    }
    return -11;
}

int saved_mediacodec_recivepkt(SAVEDMediaCodecCtx *ctx,SAVEDFrame *frame){
    AMediaCodecBufferInfo info;
    int status= AMediaCodec_dequeueOutputBuffer(ctx->vcodec,&info,0);
    if(status >= 0){

    }
}

void saved_mediacodec_close(SAVEDMediaCodecCtx *ctx);

#endif //SAMPLE_MEDIACODEC_NDK_DECODER_H
