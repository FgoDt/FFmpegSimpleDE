

#include"log.h"
#include<stdio.h>
#include<libavutil/time.h>
#include<libavutil/avutil.h>
#include<time.h>
#include <string.h>

#if __ANDROID_NDK__

#include <android/log.h>
#endif

#define MAXLOGSTRLEN  1024

static int saved_log_level;

void saved_logp(void *ctx, int level,const char *file, const char *func, const char *fmtstr, ...) {
    int filelen = strlen(file);
    int funclen = strlen(func);
    int fmtlen = strlen(fmtstr);
    // "file -> func : " + 4 + 3
    int totallen = filelen+4 + funclen+3 + fmtlen +2;
    char *fmt = (char*)malloc(totallen);
    memset(fmt,' ',totallen);
    fmt[totallen-1]='\0';
    memcpy(fmt,file,filelen);
    memcpy(fmt+filelen," -> ", 4);
    memcpy(fmt+filelen+4,func,funclen);
    memcpy(fmt+filelen+4+funclen," : ", 3);

    memcpy(fmt+funclen+filelen+7,fmtstr,fmtlen);

  if (level > saved_log_level)
    {
        return;
    }

    char str[MAXLOGSTRLEN] = "";

    int64_t now = av_gettime();

    time_t t;
    time(&t);
    struct tm * timeinfo = localtime(&t);
    static char timestr[100];
    now /= 1000;
    now %= 1000;
    sprintf(timestr, "[%.2d-%.2d %.2d:%.2d:%.2d:%4ld] ", timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, now);
    timestr[99] = '\0';

    size_t tstrLen = strlen(timestr);
    tstrLen = tstrLen > MAXLOGSTRLEN - tstrLen ? MAXLOGSTRLEN - tstrLen : tstrLen;
    memcpy(str, timestr, tstrLen);
#if __ANDROID_NDK__
    int android_leve = 0;
#endif
    switch (level)
    {
    case 0:
        memcpy(str + tstrLen, "[E] ", 4);
#if __ANDROID_NDK__
        android_leve = ANDROID_LOG_ERROR;
#endif
        break;
    case 1:
        memcpy(str + tstrLen, "[W] ", 4);
#if __ANDROID_NDK__
            android_leve = ANDROID_LOG_WARN;
#endif
        break;
    case 2:
        memcpy(str + tstrLen, "[D] ", 4);
#if __ANDROID_NDK__
            android_leve = ANDROID_LOG_DEBUG;
#endif
        break;
    default:
        memcpy(str + tstrLen, "[D] ", 4);
#if __ANDROID_NDK__
            android_leve = ANDROID_LOG_INFO;
#endif
        break;
    }

    fmtstr = fmt;
    va_list args;
    va_start(args, fmtstr);
    vsnprintf(str + tstrLen + 4, +MAXLOGSTRLEN - 1, fmt, args);
    va_end(args);

    printf("%s\n", str);

#ifdef WIN32


#endif // windows

#if __ANDROID_NDK__
    __android_log_print(android_leve,"SAVED4A","*********SAVED4A*********\r\n%s\n",str);
#endif

free(fmt);

}

void saved_set_log_level(int level) {
    saved_log_level = level;
}
