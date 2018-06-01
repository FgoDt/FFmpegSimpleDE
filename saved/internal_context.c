#include "internal_context.h"

SAVEDInternalContext* saved_internal_alloc() {
    SAVEDInternalContext *ctx = (SAVEDInternalContext*)malloc(sizeof(SAVEDInternalContext));
    if (ctx == NULL)
    {
        SAVLOGE("no mem");
        return NULL;
    }
    ctx->fmt = NULL;
    ctx->savctx = NULL;
    ctx->isencoder = 0;
    return ctx;
}

static int open_encoder() {
}

static int open_decoder() {
}



int saved_internal_open(SAVEDInternalContext *ictx,const char* path, const char *options) {
    RETIFNULL(ictx) SAVED_E_USE_NULL;

    int ret = 0;
    if (ictx->isencoder&&path!=NULL)
    {
       ret = saved_format_open_output(ictx->fmt, path, options);
    }
    else if(path!=NULL)
    {
       ret = saved_format_open_input(ictx->fmt, path, options);
    }

    if (ret != SAVED_OP_OK)
    {
        return ret;
    }

    if (ictx->isencoder)
    {
        return SAVED_E_UNDEFINE;
    }
    else
    {
        
    }
    

    return SAVED_E_UNDEFINE;
}



int close() {

}

int get_pkt() {
}

int send_pkt() {

}