#pragma once

void saved_set_log_level(int level);

void saved_log(void *ctx,int level,const char *file, const char *fmt,...);
