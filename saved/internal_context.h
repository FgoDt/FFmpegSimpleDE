#include "codec_context.h"
#include "format.h"

typedef struct SAVEDInternalContext{
    SAVEDFormat *fmt;
    SAVEDCodecContext *savctx;
    int isencoder;
}SAVEDInternalContext;

/**
 * create internal ctx
 **/
SAVEDInternalContext* saved_internal_alloc();

/**
 * open input 
 * @param ictx 
 * @param path media file path can be net path or local path
 * @param options open option
 * return >= 0 if ok 
 **/
int saved_internal_open(SAVEDInternalContext *ictx, const char* path, const char *options);