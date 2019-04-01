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

typedef struct SAVEDAudioPar SAVEDAudioPar;
typedef struct SAVEDPicPar SAVEDPicPar;
typedef struct SAVEDInternalContext SAVEDInternalContex;
typedef struct AVFrame AVFrame;
typedef struct AVPacket AVPacket;

typedef struct SAVEDContext {
    struct SAVEDInternalContext *ictx;
    SAVEDAudioPar *audioPar;
    SAVEDPicPar *picPar;
    int openmark;
}SAVEDContext;

typedef struct{
    double duration;
    double pts;
    SAVEDMEDIA_TYPE type;
    unsigned char * data;
    int size;

    AVPacket *internalPkt;//ffmpeg avpacket
    int useinternal;//1 for use internaldata
}SAVEDPkt;

typedef struct{ 
    double duration;
    double pts;
    SAVEDMEDIA_TYPE type;
    unsigned char * data;//raw data
    int size;   //raw data size
                // if fmt is yuv
                // y size = size/1.5
    int nb_sample;
    int fmt;
    int ch;
    int width;
    int height;
    int linesize[8];
    AVFrame *internalframe;//ffmpeg avframe
    int useinternal;// 1 for use internaldata
}SAVEDFrame;


SAVEDContext* saved_create_context(void);

int saved_del_context(void);

int saved_open(SAVEDContext *ctx, const char *path,const char *options, int isencoder );

int saved_open_with_par(SAVEDContext *ctx, const char *path, const char *options, int isencoder, 
                                                    int vh, int vw, int vfmt, int vbitrate,
                                                    int asample_rate, int ach, int afmt, int abitrate);
int saved_open_with_vcodec(SAVEDContext *ctx,SAVEDContext *vctx, const char *path, const char *options, int isencoder,
                        int vh, int vw, int vfmt, int vbitrate,
                        int asample_rate, int ach, int afmt, int abitrate);

int saved_close(SAVEDContext *ctx);

SAVEDPkt* saved_create_pkt(void);

int saved_del_pkt(SAVEDPkt *pkt);

int saved_pkt_unref(SAVEDPkt *pkt);

SAVEDFrame* saved_create_frame(void);

int saved_del_frame(SAVEDFrame *savedFrame);

int saved_frame_unref(SAVEDFrame *savedFrame);

int saved_get_pkt(SAVEDContext *ctx, SAVEDPkt *pkt);

int saved_send_pkt(SAVEDContext *ctx, SAVEDPkt *pkt);

int saved_get_frame(SAVEDContext *ctx, SAVEDFrame *f);

int saved_send_frame(SAVEDContext *ctx, SAVEDFrame *f);

int saved_get_frame_raw(SAVEDContext *ctx,unsigned char **data, int linesize[4]);

int saved_get_pkt_raw(SAVEDContext *ctx,unsigned char * data, int size);

int saved_get_metadata(SAVEDContext *ctx, char *key, char **val);

int saved_set_audio_par(SAVEDContext *ctx,int ch, int sample_rate, int fmt);

int saved_set_video_par(SAVEDContext *ctx, int w, int h, int fmt);

int saved_get_audio_par(SAVEDContext *ctx,int* ch, int* sample_rate, int* fmt);

int saved_get_video_par(SAVEDContext *ctx,int *w, int *h, int *fmt);

int saved_seek(SAVEDContext *ctx,double pts);

int saved_get_duration(SAVEDContext *ctx, double *duration);

int saved_set_video_codec(SAVEDContext *ctx, SAVEDContext *src);


int test(void);


#endif // __SAVED_H__
