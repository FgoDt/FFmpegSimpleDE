#ifndef __SAVED_H__
#define __SAVED_H__

#    define saved_deprecated __declspec(deprecated)

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

typedef enum SAVED_VIDEO_FMT_TYPE {
	SAVED_VIDEO_FMT_NONE = -1,
	SAVED_VIDEO_FMT_YUV420P,
	SAVED_VIDEO_FMT_NV12,
	SAVED_VIDEO_FMT_RGB
}SAVED_VIDEO_FMT_TYPE;

typedef enum SAVED_AUDIO_FMT_TYPE {
	SAVED_AUDIO_FMT_NONE = -1,
	SAVED_AUDIO_FMT_U8,          ///< unsigned 8 bits
	SAVED_AUDIO_FMT_S16,         ///< signed 16 bits
	SAVED_AUDIO_FMT_S32,         ///< signed 32 bits
	SAVED_AUDIO_FMT_FLT,         ///< float
	SAVED_AUDIO_FMT_DBL,         ///< double

	SAVED_AUDIO_FMT_U8P,         ///< unsigned 8 bits, planar
	SAVED_AUDIO_FMT_S16P,        ///< signed 16 bits, planar
	SAVED_AUDIO_FMT_S32P,        ///< signed 32 bits, planar
	SAVED_AUDIO_FMT_FLTP,        ///< float, planar
	SAVED_AUDIO_FMT_DBLP,        ///< double, planar
	SAVED_AUDIO_FMT_S64,         ///< signed 64 bits
	SAVED_AUDIO_FMT_S64P,        ///< signed 64 bits, planar

	SAVED_AUDIO_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
}SAVED_AUDIO_FMT_TYPE;

typedef struct SAVEDAudioPar SAVEDAudioPar;
typedef struct SAVEDPicPar SAVEDVideoPar;
typedef struct SAVEDInternalContext SAVEDInternalContex;
typedef struct AVFrame AVFrame;
typedef struct AVPacket AVPacket;

typedef struct SAVEDContext {
    struct SAVEDInternalContext *ictx;
    SAVEDAudioPar *audioPar;
    SAVEDVideoPar *picPar;
    int openmark;
}SAVEDContext;

/**
 * saved use SAVEDPkt store coded data, such aac h264 
 */
typedef struct{
    double duration;        // media duration
    double pts;             // media pts
    SAVEDMEDIA_TYPE type;   // media type
    unsigned char * data;   // media data
    int size;               // data size

    AVPacket *internalPkt;  // ffmpeg avpacket
    int useinternal;        // 1 for use internaldata
}SAVEDPkt;

/**
 * saved use SAVEDFrame store decoded data, such as pcm yuv
 */
typedef struct{ 
    double duration;
    double pts;
    SAVEDMEDIA_TYPE type;
    unsigned char * data;       // raw data
    int size;                   // raw data size
                                //      if fmt is yuv
                                //      y size = size/1.5
    int nb_sample;              // audio sample size
    int fmt;                    // data format
    int ch;                     // audio channels
    int width;                  // video width
    int height;                 // video height
    int linesize[8];            // data linesize
    AVFrame *internalframe;     // ffmpeg avframe
    int useinternal;            // 1 for use internaldata
}SAVEDFrame;


/**
* alloc SAVEDContext,just alloc 
* @return NULL for error
*/
SAVEDContext* saved_context_alloc(void);


/***
 * open saved
 * @param ctx SAVEDContext 
 * @param path media path, is maybe net url, local path  
 *                   when saved is decoder path is playsource
 *                   saved is encoder path is output path
 * @param options set options to ffmpeg (you can use all ffmpeg option)
 * @param isencoder  1 for encoder
 *                   0 for decoder
 * @return 0 for success
 */"{\"oper\":\"close\"}"
int saved_open(SAVEDContext *ctx, const char *path,const char *options, int isencoder );

saved_deprecated
int saved_open_with_par(SAVEDContext *ctx, const char *path, const char *options, int isencoder, 
                                                    int vh, int vw, int vfmt, int vbitrate,
                                                    int asample_rate, int ach, int afmt, int abitrate);

saved_deprecated
int saved_open_with_vcodec(SAVEDContext *ctx,SAVEDContext *vctx, const char *path, const char *options, int isencoder,
                        int vh, int vw, int vfmt, int vbitrate,
                        int asample_rate, int ach, int afmt, int abitrate);

/**
* open saved as encoder, copy decoder context param to encoder
* @param ctx, encoder context
* @param src_ctx, decoder context 
* @param codec_copy_flag flag for which stream param copy
*                        0 for video stream, saved encoder will copy src_ctx video param for encode
*                        1 for audio stream, saved encoder will copy src_ctx audio param for encode
*                        2 for audio and video  
* @param options set options
* @return 0 for success
*/
int saved_open_encoder_width_codec(SAVEDContext *ctx, SAVEDContext *src_ctx,int codec_copy_flag,const char *options);

/**
* close codec and free SAVEDContext
* @return 0 for success
*/
int saved_close(SAVEDContext *ctx);

/**
 * alloc saved pkt
 * @return NULL for error
 */
SAVEDPkt* saved_pkt_alloc(void);

/**
 * free saved pkt
 * @param pkt saved pkt
 * @return 0 for ok
 */
int saved_pkt_free(SAVEDPkt *pkt);

/**
 * unref saved pkt,like ffmpeg av_packet_unref
 *                 when you call this function, saved pkt will free pkt->data and unref pkt->internal
 *                 so you can use saved_pkt_unref reuse pkt
 * @param pkt saved pkt 
 * @return 0 for ok
 */
int saved_pkt_unref(SAVEDPkt *pkt);

/**
 * salloc saved frame
 * @return NULL for fail
 */
SAVEDFrame* saved_frame_alloc(void);

/**
 * free saved frame
 * @saved_frame free frame
 * @return 0 for ok
 */
int saved_frame_free(SAVEDFrame *savedFrame);

/**
 * unref saved fream, like ffmpeg av_frame_unref
 *                    when you call this function, saved fream will free fream internal data
 *                    so you can use saved_frame_unref reuse fream
 * @return 0 for ok
 */
int saved_frame_unref(SAVEDFrame *savedFrame);

/**
 * get pkt
 * @param ctx SAVEDContext when ctx is decoder, it will get coded data(such as aac h264 etc.)from local media or net data
 *                         when ctx is encoder, it will get coded data from encoder(when you call saved_send_frame for encoder)
 * @pkt coded data, pkt->type will tell this data type
 * @return for decoder 0 for ok
 *                     other for error
 *         for encoder 0 for ok get data
 *                     -11 for not enough data for encoder, you need send more frame by call saved_send_frame function
 *                     other for error 
 */
int saved_get_pkt(SAVEDContext *ctx, SAVEDPkt *pkt);

/**
 * send pkt
 * @param ctx SAVEDContext when ctx is decoder, coded data will send to decoder, saved use this function to deocder data
 *                         when ctx is encoder, coded data will send to formater,saved use this function to save file or send to network
 * @param pkt send data
 * @return like ffmpeg, 0 for ok
 *                      when ctx is decoder -11 for decoder is full need call saved_get_frame
 *                      other error
 */
int saved_send_pkt(SAVEDContext *ctx, SAVEDPkt *pkt);

/**
 * get frame
 * @param ctx SAVEDContext when ctx is decoder, call this function get decoded data
 *                         when ctx is encoder, not worke
 * @param f SAVEDFrame decoded data
 * @return when ctx is decoder 0 for ok 
 *                             -11 for decoder is empty need call saved_send_pkt to send more data
 *                             other error
 */
int saved_get_frame(SAVEDContext *ctx, SAVEDFrame *f);

/**
 * send frame
 * @param ctx SAVEDContext when ctx is decoder, not worke
 *                         when ctx is encoder, call this function send yuv or pcm data to encoder
 * @param f send data
 * @return when ctx is encoder 0 for ok 
 *                             -11 for retry , encoder is full need call saved_get_pkt to recive encoded data(aac/h264)
 *                             other error
 */
int saved_send_frame(SAVEDContext *ctx, SAVEDFrame *f);

int saved_get_frame_raw(const SAVEDFrame *src,unsigned char *data, int linesize[4]);

int saved_get_pkt_raw(const SAVEDPkt* src,unsigned char * data, int size);

/**
 * get media metadata(such as title artist etc) just for decoder
 * @param ctx SAVEDContext 
 * @param key metadata key
 * @param val metadata value, once you don't need value need free value
 * ex. 
 *      char *title = NULL
 *      int ret = saved_get_metadata(ctx,"title",&title);
 *      if(ret == 0){
 *          printf("%s\n",title);
 *      }
 *      if(title != NULL)
 *          free(title)
 * @return 0 for ok
 */
int saved_get_metadata(SAVEDContext *ctx, char *key, char **val);

/**
 * set audio param
 * encoder MUST SET if you don't use saved_open_encoder_with_codec
 * @param ctx when ctx is encoder, encoder will setting by this param
 *            when ctx is decoder, decoder will force setting by this param, if you don't call decoder will output default setting
 * @param ch audio channels
 * @param sample_rate audio sample rate
 * @param fmt audio format @see enum SAVED_AUDIO_FMT_TYPE
 * @return 0 for ok
 */
int saved_set_audio_par(SAVEDContext *ctx,int ch, int sample_rate, int fmt);

/**
 * set video param
 * encoder MUST SET if you don't use saved_open_encoder_with_codec
 * @param ctx when ctx is encoder, encoder will setting by this param
 *            when ctx is decoder, decoder will force setting by this param, if you don't call decoder will output default setting
 * @param w video width
 * @param h video height
 * @param fmt video format @see enum SAVED_VIDEO_FMT_TYPE
 * @return 0 for ok
 */
int saved_set_video_par(SAVEDContext *ctx, int w, int h, int fmt);

/**
 * get audio param only for decoder
 * @param ctx 
 * @param ch audio channels
 * @param sample_rate audio sample rate
 * @param fmt audio format @see enum SAVED_AUDIO_FMT_TYPE
 * @return 0 for ok
 */
int saved_get_audio_par(SAVEDContext *ctx,int* ch, int* sample_rate, int* fmt);

/**
 * get video param only for decoder
 * @param ctx
 * @param w video width
 * @param h video height
 * @param fmt video format @see enum SAVED_VIDEO_FMT_TYPE
 * @return 0 for ok
 */
int saved_get_video_par(SAVEDContext *ctx,int *w, int *h, int *fmt);

/**
 * seek media only for decoder
 * @param ctx 
 * @param pts seek time (sec)
 * @return 0 for ok
 */
int saved_seek(SAVEDContext *ctx,double pts);

/**
 * get media duration only for decoder
 * @param ctx
 * @param duration media duration (sec)
 * @return 0 for ok
 */
int saved_get_duration(SAVEDContext *ctx, double *duration);



int test(void);


#endif // __SAVED_H__
