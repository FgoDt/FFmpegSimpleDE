#include <stdlib.h>
#include "saved/saved.h"



int main(int argc,char **argv) {


    char *url = NULL;
    if (argc == 2)
    {
        url = argv[1];
    } else{
        url = "d://abc.flv";
    }


    SAVEDContext *ctx = saved_context_alloc();
    SAVEDContext *enctx = saved_context_alloc();
   // saved_open_with_par(enctx,NULL,NULL,1,840,480,0,0,44100,2,-1,-1);


	saved_set_audio_par(ctx, 2, 4800, SAVED_AUDIO_FMT_FLT);
	saved_set_video_par(ctx, 1920, 1080, SAVED_VIDEO_FMT_YUV420P);
    saved_open(ctx, url, "d://abc.flv", 0);
    saved_open_with_par(enctx,"/home/fftest/avaava.mp4",NULL,1,240,320,0,0,44100,2,-1,-1);
    //saved_open_with_par(enctx,NULL,NULL,1,480,640,0,0,44100,2,-1,-1);
    SAVEDPkt *pkt = saved_pkt_alloc();
    SAVEDPkt *enpkt = saved_pkt_alloc();
    SAVEDPkt *envpkt = saved_pkt_alloc();
    SAVEDFrame *f = saved_frame_alloc();
    SAVEDFrame *af = saved_frame_alloc();
    f->type = SAVED_MEDIA_TYPE_VIDEO;
    af->type = SAVED_MEDIA_TYPE_AUDIO;
    enpkt->type = SAVED_MEDIA_TYPE_AUDIO;
    envpkt->type = SAVED_MEDIA_TYPE_VIDEO;


    int flag = -1;
    int count = 0;
    while (1) {


        if(saved_get_pkt(ctx, pkt)<0) {
            saved_pkt_unref(pkt);
            break;
        }
        count++;
      //  printf("GET PKT %d type %d pts %f dur %f\n",count,pkt->type,pkt->pts, pkt ->duration);
       // if(pkt->type == SAVED_MEDIA_TYPE_VIDEO)
              saved_send_pkt(ctx, pkt);
        flag =  saved_get_frame(ctx, f);
        //printf("get frame dur %f pts %f size %d \n",f->duration,f->pts, f->size);
        if(flag == 0){
            int ret = 0;
            ret = saved_send_frame(enctx,f);
            ret = saved_get_pkt(enctx,envpkt);
            if(ret == 0){
                saved_send_pkt(enctx,envpkt);
            }
            saved_pkt_unref(envpkt);
        }
        flag =  saved_get_frame(ctx,af);
        saved_pkt_unref(pkt);
        if(flag == 0){

            int ret = 0;
            re_send_frame:
            ret = saved_send_frame(enctx,af);

            if(ret == -11){
                ret = 0;
                while (ret==0) {
                    printf("in error -11\n");
                    ret = saved_get_pkt(enctx, enpkt);
                    if(ret == 0){
                        saved_send_pkt(enctx,enpkt);
                    }
                    saved_pkt_unref(enpkt);
                }
                goto re_send_frame;
            }
            ret = saved_get_pkt(enctx, enpkt);
            if(ret == 0){
                saved_send_pkt(enctx,enpkt);
            }
        }
        saved_pkt_unref(enpkt);
    }
    int ret = 0;
     ret = saved_get_frame(ctx,af);
    while((ret = saved_get_pkt(enctx,enpkt)) == 0){
        saved_send_pkt(enctx,enpkt);
        saved_pkt_unref(enpkt);
    }
    while((saved_get_pkt(enctx,envpkt))==0){
        saved_send_pkt(enctx,envpkt);
        saved_pkt_unref(envpkt);
    }
//    saved_get_pkt(ctx,pkt);
//    saved_send_pkt(ctx,pkt);
//    saved_get_frame(ctx,f);
//    saved_pkt_unref(pkt);
//
//    saved_get_pkt(ctx,pkt);
//    saved_send_pkt(ctx,pkt);
 //   saved_get_frame(ctx,f);

    saved_pkt_free(pkt);
    saved_pkt_free(enpkt);
    saved_pkt_free(envpkt);
    saved_frame_free(f);
    saved_frame_free(af);
    saved_close(ctx);
    saved_close(enctx);
  //  ctx = saved_create_context();
  //  saved_open(ctx, "/home/fftest/t.flv", NULL, 0);
  //  saved_close(ctx);

  //  ctx = saved_create_context();
  //  saved_open(ctx, "/home/fftest/t.flv", NULL, 0);
  //  saved_close(ctx);

    return 0;
}
