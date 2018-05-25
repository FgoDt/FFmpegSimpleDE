

#include"log.h"
#include<stdio.h>
#include<libavutil/time.h>
#include<libavutil/avutil.h>
#include<time.h>


#define MAXLOGSTRLEN  1024

static int saved_log_level;
void saved_log(void *ctx, int level, const char *fmt, ...) {

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
    sprintf(timestr, "[%.2d-%.2d %.2d:%.2d:%.2d:%4d] ", timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, now);
    timestr[99] = '\0';

    size_t tstrLen = strlen(timestr);
    tstrLen = tstrLen > MAXLOGSTRLEN - tstrLen ? MAXLOGSTRLEN - tstrLen : tstrLen;
    memcpy(str, timestr, tstrLen);
    switch (level)
    {
    case 0:
        memcpy(str + tstrLen, "[E] ", 4);
        break;
    case 1:
        memcpy(str + tstrLen, "[W] ", 4);
        break;
    case 2:
        memcpy(str + tstrLen, "[D] ", 4);
        break;
    default:
        memcpy(str + tstrLen, "[D] ", 4);
        break;
    }

    va_list args;
    va_start(args, fmt);
    int len;
    len = vsnprintf(str + tstrLen + 4, +MAXLOGSTRLEN - 1, fmt, args);
    va_end(args);

    printf("%s\n", str);

#ifdef WIN32


#endif // windows

}

void saved_set_log_level(int level) {
    saved_log_level = level;
}