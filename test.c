#include "saved/saved.h"

int main() {

    SAVEDContext *ctx = saved_create_context();
    saved_open(ctx, "/home/fftest/t.flv", NULL, 0);
    SAVEDPkt *pkt = saved_create_pkt();
    saved_get_pkt(ctx,pkt);
    return 0;
}