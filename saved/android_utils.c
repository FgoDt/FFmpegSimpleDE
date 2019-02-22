//
// Created by rt-zl on 2019/2/19.
//

#include <libavcodec/jni.h>
#include "android_utils.h"
#include "define.h"
#include <stdlib.h>
#include <string.h>

static JavaVM *SAVED_JAVA_VM;
static jobject SAVED_G_SURFACE_OBJ;
SAVEDAndroidData *saved_android_data_alloc(){
    SAVEDAndroidData *data = (SAVEDAndroidData*)malloc(sizeof(SAVEDAndroidData));
    memset(data,0, sizeof(SAVEDAndroidData));
    return data;
}

void saved_set_jvm(JavaVM *vm){
    SAVED_JAVA_VM = vm;
    av_jni_set_java_vm(vm,NULL);
}
JavaVM* saved_get_jvm(){
    return SAVED_JAVA_VM;
}
void saved_set_gsurface(jobject gsruface){
    SAVED_G_SURFACE_OBJ = gsruface;
}
jobject saved_get_gsurface(){
    return SAVED_G_SURFACE_OBJ;
}
void saved_android_data_free(SAVEDAndroidData* ctx){
    RETIFNULL(ctx);
    if(ctx->vm!=NULL){
        JNIEnv  *env = NULL;
        int attach = 0;
        int ret = (*ctx->vm)->GetEnv(ctx->vm,&env,JNI_VERSION_1_6);
        if(ret<0){
            ret = (*ctx->vm)->AttachCurrentThread(ctx->vm,&env,NULL);
            if(ret==0){
                attach = 1;
                (*env)->DeleteGlobalRef(env,ctx->gsurface);
            }
        }
        if(attach == 1){
            (*ctx->vm)->DetachCurrentThread(ctx->vm);
        }
    }

    free(ctx);
}
