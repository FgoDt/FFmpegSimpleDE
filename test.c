#include <stdlib.h>
#include "saved/saved.h"


int sub_aac(){
    FILE *afile = NULL;
    afile = fopen("/home/fftest/a.aac","rb");
    FILE *bfile = NULL;
    bfile = fopen("/home/fftest/b.aac","rb");

    FILE *cfile = NULL;
    cfile = fopen("/home/fftest/c.aac","wb");

    unsigned  char buf[1024] ={0};
    int redcount = 0;

    while ((redcount = fread(buf,1,1024,afile))>0){
        fwrite(buf,1,redcount,cfile);
    }

    while ((redcount = fread(buf,1,1024,bfile))>0){
        fwrite(buf,1,redcount,cfile);
    }

    fclose(afile);
    fclose(bfile);
    fclose(cfile);

}


int main(int argc,char **argv) {


    char *url = NULL;
    if (argc == 2)
    {
        url = argv[1];
    } else{
        url = "/home/fftest/05 - Burning.mp3";
    }


    SAVEDContext *ctx = saved_create_context();
    saved_open(ctx, url, NULL, 0);
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


        if(saved_get_pkt(ctx, pkt)<0) {
            saved_pkt_unref(pkt);
            break;
        }
        count++;
        printf("GET PKT %d\n",count);
       // if(pkt->type == SAVED_MEDIA_TYPE_VIDEO)
              saved_send_pkt(ctx, pkt);
        flag =  saved_get_frame(ctx, f);
        flag =  saved_get_frame(ctx,af);
        saved_pkt_unref(pkt);
        if(flag == 0){
            //fwrite(f->data,1,f->size,yuvtest);
           // printf("get yuv");
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
