#include "format.h"
#include "codec.h"
#include <libavformat/avformat.h>
#include "log.h"

SAVEDFormat* saved_format_alloc() {
    SAVEDFormat* fmt = (SAVEDFormat*)malloc(sizeof(SAVEDFormat));
    if (fmt == NULL)
    {
        SAVLOGE("no mem");
        return fmt;
    }
    fmt->fmt = NULL;
    fmt->best_audio_index = -1;
    fmt->best_video_index = -1;
    fmt->astream = NULL;
    fmt->vstream = NULL;
    fmt->sstream = NULL;
    return fmt;
}

int saved_format_close(SAVEDFormat *fmt){
    RETIFNULL(fmt) SAVED_E_USE_NULL;
    if(fmt->fmt){
        avformat_close_input(&fmt->fmt);
        avformat_free_context(fmt->fmt);
        SAVED_SET_NULL(fmt->fmt);
    }
    fmt->best_video_index = -1;
    fmt->best_audio_index = -1;
    SAVED_SET_NULL(fmt->astream);
    SAVED_SET_NULL(fmt->vstream);
    SAVED_SET_NULL(fmt->sstream);
    free(fmt);
    return  SAVED_OP_OK;
}


int saved_format_open_input(SAVEDFormat* ctx,const char *path, const char *options) {
    RETIFNULL(ctx) SAVED_E_USE_NULL;


    ctx->fmt = avformat_alloc_context();

    RETIFNULL(path) SAVED_E_USE_NULL;


    if (avformat_open_input(&ctx->fmt, path, NULL, NULL) < 0) {
        SAVLOGE("open input error");
        return SAVED_E_AVLIB_ERROR;
    }

    if (avformat_find_stream_info(ctx->fmt, NULL) < 0) {
        SAVLOGE("can not find stream info");
        return SAVED_E_AVLIB_ERROR;
    }

    if (ctx->fmt->nb_streams<0)
    {
        SAVLOGE("streams nb < 0");
        return SAVED_E_AVLIB_ERROR;
    }



    for(int i = 0; i<MAX_STREAMS;i++){
        ctx->allTypes[i] = AVMEDIA_TYPE_UNKNOWN;
    }

    
    for (size_t i = 0; i < ctx->fmt->nb_streams; i++)
    {
        ctx->allTypes[i] = ctx->fmt->streams[i]->codecpar->codec_type;
    }

    ctx->best_audio_index =  av_find_best_stream(ctx->fmt, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    ctx->best_video_index = av_find_best_stream(ctx->fmt, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);


    if (ctx->best_audio_index <0)
    {
        SAVLOGW("no audio in media");
    } else{
        SAVEDLOG1(NULL,SAVEDLOG_LEVEL_D,"find audio at index : %d",ctx->best_audio_index);
    }
    if (ctx->best_video_index <0)
    {
        SAVLOGW("no video in media");
    }else{
        SAVEDLOG1(NULL,SAVEDLOG_LEVEL_D,"find video at index : %d",ctx->best_video_index);
    }
    ctx->astream = ctx->fmt->streams[ctx->best_audio_index];
    ctx->vstream = ctx->fmt->streams[ctx->best_video_index];

    if (ctx->best_video_index < 0 && ctx->best_audio_index < 0) {
        return SAVED_E_NO_MEDIAFILE;
    }
    return SAVED_OP_OK;
}


int saved_format_open_output(SAVEDFormat* ctx, const char *path, const char *options) {
    return SAVED_E_UNDEFINE;
}

int saved_format_get_pkt(SAVEDFormat *ctx, AVPacket *pkt) {
    RETIFNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(pkt) SAVED_E_USE_NULL;
    RETIFNULL(ctx->fmt) SAVED_E_USE_NULL;

    if (av_read_frame(ctx->fmt, pkt) < 0) {
        SAVLOGE("av read frame error");
        return SAVED_E_AVLIB_ERROR;
    }
    return SAVED_OP_OK;
}

int saved_format_send_pkt(SAVEDFormat *ctx, AVPacket *pkt) {
    return SAVED_E_UNDEFINE;
}