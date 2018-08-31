#include "saved/saved.h"


int main() {

    SAVEDContext *ctx = saved_create_context();
    saved_open(ctx, "/home/fftest/t.flv", NULL, 0);
    SAVEDPkt *pkt = saved_create_pkt();
    SAVEDFrame *f = saved_create_frame();
    SAVEDFrame *af = saved_create_frame();
    f->type = SAVED_MEDIA_TYPE_VIDEO;
    af->type = SAVED_MEDIA_TYPE_AUDIO;

    int flag = -1;
    FILE *yuvtest;
    yuvtest = fopen("/home/fftest/abc.yuv","wb");
    int count = 0;
    while (1) {
        count++;
        if(saved_get_pkt(ctx, pkt)<0)
            break;
        if(pkt->type == SAVED_MEDIA_TYPE_VIDEO)
              saved_send_pkt(ctx, pkt);
        flag =  saved_get_frame(ctx, f);
        //saved_get_frame(ctx,af);
               printf("decode %d\n",count);
        if(count == 871){
            printf("jjjjj");
        }
        saved_pkt_unref(pkt);
        if(flag == 0){
            //fwrite(f->data,1,f->size,yuvtest);
          //  printf("get yuv");
        }
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
    fclose(yuvtest);
  //  ctx = saved_create_context();
  //  saved_open(ctx, "/home/fftest/t.flv", NULL, 0);
  //  saved_close(ctx);

  //  ctx = saved_create_context();
  //  saved_open(ctx, "/home/fftest/t.flv", NULL, 0);
  //  saved_close(ctx);

    return 0;
}