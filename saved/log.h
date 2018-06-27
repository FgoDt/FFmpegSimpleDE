#ifndef __HOG_H__
#define __HOG_H__

void saved_set_log_level(int level);

void saved_logp(void *ctx, int level,const char *file, const char *func, const char *fmtstr, ...);

#endif // !__HOG_H__
