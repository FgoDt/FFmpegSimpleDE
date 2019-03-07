#include "format.h"
#include "codec.h"
#include <libavformat/avformat.h>
#include "log.h"
#include "saved.h"
#include <libavutil/error.h>

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
    fmt->is_write_header = 0;
    fmt->start_pts = -1;
    return fmt;
}

int saved_format_close(SAVEDFormat *fmt){
    RETIFNULL(fmt) SAVED_E_USE_NULL;
    if(fmt->flag == SAVED_FORMAT_OPENING){
        fmt->flag = SAVED_FORMAT_FORCE_CLOSE;
    }
    while(fmt->flag == SAVED_FORMAT_TRY_OPEN ||
    fmt->flag == SAVED_FORMAT_OPENING ||
    fmt->flag ==SAVED_FORMAT_FORCE_CLOSE ){
        usleep(10*1000);
    }

    if(fmt->fmt){
        if(fmt->is_write_header){
            av_write_trailer(fmt->fmt);
        }
    }

    if(!fmt->is_write_header){
        avformat_close_input(&fmt->fmt);
    }
    if(fmt->fmt&&!(fmt->fmt->flags&AVFMT_NOFILE)){
        avio_close(fmt->fmt->pb);
    }
    if(fmt->fmt) {
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

int static av_interrrupt_callback(void *data){
    SAVEDFormat * ctx = (SAVEDFormat *)data;
    if(ctx->flag == SAVED_FORMAT_FORCE_CLOSE){
        return AVERROR_EXIT;
    }
    return SAVED_OP_OK;
}

int saved_format_open_input(SAVEDFormat* ctx,const char *path, const char *options) {
    RETIFNULL(ctx) SAVED_E_USE_NULL;

    ctx->flag = SAVED_FORMAT_TRY_OPEN;

#if(LIBAVFORMAT_VERSION_MAJOR<59)
        av_register_all();
#endif

    avformat_network_init();

    ctx->fmt = avformat_alloc_context();

    RETIFNULL(path) SAVED_E_USE_NULL;

    ctx->fmt->interrupt_callback.callback = av_interrrupt_callback;
    ctx->fmt->interrupt_callback.opaque = ctx;


    ctx->flag = SAVED_FORMAT_OPENING;
    if (avformat_open_input(&ctx->fmt, path, NULL, NULL) < 0) {
        SAVLOGE("open input error");
        ctx->flag = SAVED_FORMAT_OPEN_ERROR;
        return SAVED_E_AVLIB_ERROR;
    }
    if(ctx->flag == SAVED_FORMAT_FORCE_CLOSE ){
        ctx->flag = SAVED_FORMAT_OPEN_ERROR;
        return SAVED_E_AVLIB_ERROR;
    }
    ctx->flag = SAVED_FORMAT_OPEN;


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
        ctx->astream = ctx->fmt->streams[ctx->best_audio_index];
        SAVEDLOG1(NULL,SAVEDLOG_LEVEL_D,"find audio at index : %d",ctx->best_audio_index);
    }
    if (ctx->best_video_index <0)
    {
        SAVLOGW("no video in media");
    }else{
        ctx->vstream = ctx->fmt->streams[ctx->best_video_index];
        SAVEDLOG1(NULL,SAVEDLOG_LEVEL_D,"find video at index : %d",ctx->best_video_index);
    }

    if (ctx->best_video_index < 0 && ctx->best_audio_index < 0) {
        return SAVED_E_NO_MEDIAFILE;
    }
    return SAVED_OP_OK;
}

int saved_format_open_output_with_vpar(SAVEDFormat* ctx, void *encoderContext,AVStream *vstream,const char *path,const char *options){
    RETIFNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(encoderContext) SAVED_E_USE_NULL;
    RETIFNULL(path) SAVED_E_USE_NULL;

    SAVEDEncoderContext *enctx = (SAVEDEncoderContext*)encoderContext;

#if LIBAVFORMAT_VERSION_MAJOR < 58
    av_register_all();
#endif
    avformat_network_init();

    // ctx->fmt = avformat_alloc_context();
    int ret = avformat_alloc_output_context2(&ctx->fmt,NULL,NULL,path);
    ctx->astream = avformat_new_stream(ctx->fmt,NULL);
    ctx->vstream = avformat_new_stream(ctx->fmt,NULL);

    if(!(ctx->fmt->flags&AVFMT_NOFILE)){
        ret = avio_open(&ctx->fmt->pb,path,AVIO_FLAG_WRITE);
        if(ret<0){
            SAVLOGE("avio open error may networke error!");
            return SAVED_E_AVLIB_ERROR;
        }
    }
    ctx->is_write_header = 0;

    if(ctx->astream!=NULL) {
        AVCodecParameters *parameters = avcodec_parameters_alloc();
        avcodec_parameters_from_context(parameters,enctx->actx);
        avcodec_parameters_copy(ctx->astream->codecpar,parameters);
        avcodec_parameters_free(&parameters);
    }

    if(ctx->vstream != NULL) {

        avcodec_parameters_copy(ctx->vstream->codecpar,vstream->codecpar);
        ctx->vstream->time_base = vstream->time_base;
    }


    return ret;
}

int saved_format_open_output(SAVEDFormat* ctx, void *encoderContext, const char *path, const char *options) {
    RETIFNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(encoderContext) SAVED_E_USE_NULL;
    RETIFNULL(path) SAVED_E_USE_NULL;

    SAVEDEncoderContext *enctx = (SAVEDEncoderContext*)encoderContext;

#if LIBAVFORMAT_VERSION_MAJOR < 58
        av_register_all();
#endif
    avformat_network_init();

   // ctx->fmt = avformat_alloc_context();
    int ret = avformat_alloc_output_context2(&ctx->fmt,NULL,NULL,path);
    ctx->astream = avformat_new_stream(ctx->fmt,NULL);
    ctx->vstream = avformat_new_stream(ctx->fmt,NULL);

    if(!(ctx->fmt->flags&AVFMT_NOFILE)){
        ret = avio_open(&ctx->fmt->pb,path,AVIO_FLAG_WRITE);
        if(ret<0){
            SAVLOGE("avio open error may networke error!");
            return SAVED_E_AVLIB_ERROR;
        }
    }
    ctx->is_write_header = 0;

    if(ctx->astream!=NULL) {
        AVCodecParameters *parameters = avcodec_parameters_alloc();
        avcodec_parameters_from_context(parameters,enctx->actx);
        avcodec_parameters_copy(ctx->astream->codecpar,parameters);
        avcodec_parameters_free(&parameters);
    }

    if(ctx->vstream != NULL) {

        AVCodecParameters *parameters = avcodec_parameters_alloc();
        avcodec_parameters_from_context(parameters,enctx->vctx);
        avcodec_parameters_copy(ctx->vstream->codecpar,parameters);
        avcodec_parameters_free(&parameters);
    }


    return ret;
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

int saved_format_send_pkt(SAVEDFormat *ctx, SAVEDPkt *pkt) {
    RETIFNULL(ctx) SAVED_E_USE_NULL;
    RETIFNULL(pkt) SAVED_E_USE_NULL;
    RETIFNULL(ctx->fmt) SAVED_E_USE_NULL;
    int ret = 0;

    if(!ctx->is_write_header){
        ret = avformat_write_header(ctx->fmt,NULL);
        if(ret<0){
            SAVLOGE("write header error\n");
            return  ret;
        }
        ctx->is_write_header = 1;
    }


    AVStream *stream  = NULL;
    switch (pkt->type){
        case SAVED_MEDIA_TYPE_AUDIO:
            stream = ctx->astream;
            break;
        case SAVED_MEDIA_TYPE_VIDEO:
            stream = ctx->vstream;
            break;
        default:
            return SAVED_E_UNDEFINE;
    }
    if(stream == NULL){
        return SAVED_E_NO_MEDIAFILE;
    }
    AVPacket *ipkt = (AVPacket*)pkt->internalPkt;
    ipkt->stream_index = stream->index;
    if(pkt->pts != ipkt->pts){
        ipkt->pts = av_q2d(stream->time_base)*ipkt->pts*1000;
        ipkt->duration = av_q2d(stream->time_base)*ipkt->duration*1000;
        ipkt->dts = av_q2d(stream->time_base)*ipkt->dts*1000;
    }


    ipkt->pts = av_rescale_q_rnd(ipkt->pts,(AVRational){1,1000},stream->time_base,AV_ROUND_INF|AV_ROUND_PASS_MINMAX);
    ipkt->dts = av_rescale_q_rnd(ipkt->dts,(AVRational){1,1000},stream->time_base,AV_ROUND_INF|AV_ROUND_PASS_MINMAX);
    ipkt->duration= av_rescale_q(ipkt->duration,(AVRational){1,1000},stream->time_base);
    ipkt->pos  = -1;

    if(stream->start_time < 0){
        stream->start_time = ipkt->pts;
    }
    stream->duration = ipkt->pts-stream->start_time;
    ret = av_write_frame(ctx->fmt,ipkt);

    return ret;
}