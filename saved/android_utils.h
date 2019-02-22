//
// Created by rt-zl on 2019/2/19.
//

#ifndef SAVED_ANDROID_UTILS_H
#define SAVED_ANDROID_UTILS_H

#include <jni.h>

typedef struct SAVEDAndroidData{
    JavaVM *vm;
    jobject gsurface;
}SAVEDAndroidData;

SAVEDAndroidData *saved_android_data_alloc();
void saved_set_jvm(JavaVM *vm);
JavaVM* saved_get_jvm();
void saved_set_gsurface(jobject);
jobject saved_get_gsurface();
void saved_android_data_free(SAVEDAndroidData* ctx);

#endif //SAVED_ANDROID_UTILS_H
