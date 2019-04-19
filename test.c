#include <stdlib.h>
#include "saved/saved.h"



int main(int argc,char **argv) {


    char *url = NULL;
    if (argc == 2)
    {
        url = argv[1];
    } else{
		url = "C:\\Users\\admin\\Videos\\abc.flv";
    }


    SAVEDContext *ctx = saved_create_context();
    SAVEDContext *enctx = saved_create_context();

	saved_set_audio_par(ctx, 2, 44100, SAVED_VIDEO_FMT_NV12);

    saved_open(ctx, url, NULL, 0);
	int w, h, fmt;

	int ret = saved_get_video_par(ctx, &w, &h, &fmt);

	char *buf = (char*)malloc(w*h*1.5);
	int linesize[4] = { 0 };

	SAVEDPkt *pkt = saved_create_pkt();
	SAVEDFrame *aframe = saved_create_frame();
	aframe->type = SAVED_MEDIA_TYPE_AUDIO;
	SAVEDFrame *vframe = saved_create_frame();
	vframe->type = SAVED_MEDIA_TYPE_VIDEO;
  
	while ((ret = saved_get_pkt(ctx,pkt))==0)
	{
		ret = saved_send_pkt(ctx, pkt);
		if (ret < 0) {
			break;
		}
		ret = saved_get_frame(ctx, aframe);
		if (ret < 0 && ret != -11) {
			break;
		}
		ret = saved_get_frame(ctx, vframe);
		if (ret < 0 && ret != -11) {
			break;
		}
		if(ret == 0)
			saved_get_frame_raw(vframe, buf, linesize);
		saved_pkt_unref(pkt);
	}

    return 0;
}
