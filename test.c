#include <stdlib.h>
#include "saved/saved.h"



int main(int argc,char **argv) {


    char *url = NULL;
    if (argc == 2)
    {
        url = argv[1];
    } else{
        url = "/home/fftest/b.mp4";
    }


    SAVEDContext *ctx = saved_create_context();
    SAVEDContext *enctx = saved_create_context();
   // saved_open_with_par(enctx,NULL,NULL,1,840,480,0,0,44100,2,-1,-1);


    saved_open(ctx, url, NULL, 0);
    saved_open_with_par(enctx,"/home/fftest/avaava.mp4",NULL,1,240,320,0,0,44100,2,-1,-1);
    //saved_open_with_par(enctx,NULL,NULL,1,480,640,0,0,44100,2,-1,-1);
    SAVEDPkt *pkt = saved_create_pkt();
    SAVEDPkt *enpkt = saved_create_pkt();
    SAVEDPkt *envpkt = saved_create_pkt();
    SAVEDFrame *f = saved_create_frame();
    SAVEDFrame *af = saved_create_frame();
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

    saved_del_pkt(pkt);
    saved_del_pkt(enpkt);
    saved_del_pkt(envpkt);
    saved_del_frame(f);
    saved_del_frame(af);
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
