//
// Created by fftest on 18-7-9.
//

#include "audioresample.h"
#include <stdlib.h>
#include <libswresample/swresample.h>

SAVEDAudioResampleCtx* saved_resmaple_alloc(){
    SAVEDAudioResampleCtx *ctx = (SAVEDAudioResampleCtx*)malloc(sizeof(SAVEDAudioResampleCtx));
    memset(ctx,0,sizeof(SAVEDAudioResampleCtx));
    ctx->src = NULL;
    ctx->tgt = NULL;
    return ctx;
}

int saved_resmaple_create(SAVEDAudioResampleCtx* ctx,AVStream *instream){

}

void saved_reample_close(SAVEDAudioResampleCtx* ctx){

}
