#ifndef __SAVED_H__
#define __SAVED_H__


#include<stdio.h>


typedef enum SAVEDMEDIA_TYPE {
    SAVED_MEDIA_TYPE_NONE = -1,
    SAVED_MEDIA_TYPE_VIDEO,
    SAVED_MEDIA_TYPE_AUDIO,
    SAVED_MEDIA_TYPE_DATA,
    SAVED_MEDIA_TYPE_SUBTITLE,
    SAVED_MEDIA_TYPE_ATTACHMENT,    ///< Opaque data information usually sparse
    SAVED_MEDIA_TYPE_NB
}SAVEDMEDIA_TYPE;

typedef struct SAVEDContext {
    void *ictx;
    int openmark;
}SAVEDContext;

typedef struct{
    double duration;
    double pts;
    SAVEDMEDIA_TYPE type;
    unsigned char * data;
    int size;
    void *internalPkt;//ffmpeg avpacket
    int useinternal;//1 for use internaldata
}SAVEDPkt;

typedef struct{ 
    double duration;
    double pts;
    SAVEDMEDIA_TYPE type;
    unsigned char * data;//raw data
    int size;
    void *internalframe;//ffmpeg avframe
    int useinternal;// 1 for use internaldata
}SAVEDFrame;


SAVEDContext* saved_create_context();

int saved_del_context();

int saved_open(SAVEDContext *ctx, const char *path,const char *options, int isencoder );

int saved_close(SAVEDContext *ctx);

SAVEDPkt* saved_create_pkt();

int saved_del_pkt(SAVEDPkt *pkt);

int saved_pkt_unref(SAVEDPkt *pkt);

SAVEDFrame* saved_create_frame();

int saved_del_frame(SAVEDFrame *savedFrame);

int saved_frame_unref(SAVEDFrame *savedFrame);

int saved_get_pkt(SAVEDContext *ctx, SAVEDPkt *pkt);

int saved_send_pkt(SAVEDContext *ctx, SAVEDPkt *pkt);

int saved_get_frame(SAVEDContext *ctx, SAVEDFrame *f);

int saved_send_frame(SAVEDContext *ctx, SAVEDFrame *f);

int saved_get_frame_raw(SAVEDContext *ctx,unsigned char **data, int linesize[4]);

int saved_get_pkt_raw(SAVEDContext *ctx,unsigned char * data, int size);


int test();

// TODO: 在此处引用程序需要的其他标头。

#endif // __SAVED_H__
