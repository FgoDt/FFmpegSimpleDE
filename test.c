#include <stdlib.h>
#include "saved/saved.h"



int main(int argc,char **argv) {


    char *url = NULL;
    if (argc == 2)
    {
        url = argv[1];
    } else{
        url = "/home/fgodt/style.flac";
    }


    SAVEDContext *ctx = saved_create_context();
    SAVEDContext *enctx = saved_create_context();
    saved_open_with_par(enctx,NULL,NULL,1,0,0,0,0,44100,2,-1,-1);

    

    saved_open(ctx, url, NULL, 0);
    SAVEDPkt *pkt = saved_create_pkt();
    SAVEDPkt *enpkt = saved_create_pkt();
    SAVEDFrame *f = saved_create_frame();
    SAVEDFrame *af = saved_create_frame();
    f->type = SAVED_MEDIA_TYPE_VIDEO;
    af->type = SAVED_MEDIA_TYPE_AUDIO;


    int flag = -1;
    int count = 0;
    while (1) {


        if(saved_get_pkt(ctx, pkt)<0) {
            saved_pkt_unref(pkt);
            break;
        }
        count++;
        printf("GET PKT %d type %d pts %f dur %f\n",count,pkt->type,pkt->pts, pkt ->duration);
       // if(pkt->type == SAVED_MEDIA_TYPE_VIDEO)
              saved_send_pkt(ctx, pkt);
        flag =  saved_get_frame(ctx, f);
        flag =  saved_get_frame(ctx,af);
        saved_pkt_unref(pkt);
        printf("get audio dur %f pts %f size %d \n",af->duration,af->pts, af->size);
        if(flag == 0){
            int ret = saved_send_frame(enctx,af);
            ret = saved_get_pkt(enctx,enpkt);
        }
        saved_pkt_unref(enpkt);
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
    saved_del_frame(f);
    saved_del_frame(af);
    saved_close(ctx);
  //  ctx = saved_create_context();
  //  saved_open(ctx, "/home/fftest/t.flv", NULL, 0);
  //  saved_close(ctx);

  //  ctx = saved_create_context();
  //  saved_open(ctx, "/home/fftest/t.flv", NULL, 0);
  //  saved_close(ctx);

    return 0;
}
