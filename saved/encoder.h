#ifndef __ENCODER_H__
#define __ENCODER_H__


typedef struct SAVEDEncoderContext {
    int use_hw;
    char *hw_name;
    AVBufferRef *hw_bufferref;
}SAVEDEncoderContext;

int saved_encoder_create();

int saved_encoder_send_frame();

int saved_encoder_recive_pkt();

int saved_encoder_close();

#endif // !__ENCODER_H__