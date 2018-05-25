#include "format.h"
#include "codec_context.h"
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
    return fmt;
}


int saved_format_open_input(SAVEDFormat* ctx,const char *path, const char *options) {
    RETIFNULL(ctx) SAVED_E_USE_NULL;


    ctx->fmt = avformat_alloc_context();

    RETIFNULL(path) SAVED_E_USE_NULL;

    av_register_all();

    if (avformat_open_input(&ctx->fmt, path, NULL, NULL) < 0) {
        SAVLOGE("open input error");
        return SAVED_E_AVLIB_ERROR;
    }

    if (avformat_find_stream_info(ctx->fmt, NULL) < 0) {
        SAVLOGE("can not find stream info");
        return SAVED_E_AVLIB_ERROR;
    }

    

    return SAVED_E_UNDEFINE;
}

int saved_format_open_output(SAVEDContext* ctx) {
    return SAVED_E_UNDEFINE;
}