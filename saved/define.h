#ifndef __SAVED_DEFINE_H__
#define __SAVED_DEFINE_H__
#include <stdio.h>
#include "log.h"



#define SAVEDLOG_LEVEL_E 0
#define SAVEDLOG_LEVEL_W 1
#define SAVEDLOG_LEVEL_D 2


#define SAVEDLOG(ctx,level,fmt) saved_logp(ctx,level,__FILE__,__FUNCTION__,fmt)
#define SAVEDLOG1(ctx,level,fmt,i) saved_logp(ctx,level,__FILE__,__FUNCTION__,fmt,i)
#define SAVEDLOG2(ctx,level,fmt,i,j) saved_logp(ctx,level,__FILE__,__FUNCTION__,fmt,i,j)
#define SAVEDLOG3(ctx,level,fmt,i,j,k) saved_logp(ctx,level,__FILE__,__FUNCTION__,fmt,i,j,k)

#define SAVEDLOGE(ctx,fmt) SAVEDLOG(ctx,SAVEDLOG_LEVEL_E,fmt)
#define SAVEDLOGW(ctx,fmt) SAVEDLOG(ctx,SAVEDLOG_LEVEL_W,fmt)
#define SAVEDLOGD(ctx,fmt) SAVEDLOG(ctx,SAVEDLOG_LEVEL_D,fmt)

#define SAVEDLOGND(...) saved_logp(NULL,SAVEDLOG_LEVEL_E,__VA_ARGS__)

#define SAVLOGE(fmt) SAVEDLOG(NULL,SAVEDLOG_LEVEL_E,fmt)
#define SAVLOGW(fmt) SAVEDLOG(NULL,SAVEDLOG_LEVEL_W,fmt)
#define SAVLOGD(fmt) SAVEDLOG(NULL,SAVEDLOG_LEVEL_D,fmt)

#define OPENMARK 0x12F3F2
#define SAVEDCTXISNULL(x) ((x)->openmark != OPENMARK)
#define RETIFCTXNULL(x) if(SAVEDCTXISNULL(x)) return
#define RETIFNULL(x) if((x) == NULL) return

#define SAVED_SET_NULL(x) ((x) = NULL)

#define SAVED_E_
#define SAVED_OP_OK 0
#define SAVED_E_AVLIB_ERROR -1
#define SAVED_E_NO_MEM  -2
#define SAVED_E_NO_MEDIAFILE -3
#define SAVED_E_EOF -5
#define SAVED_E_USE_NULL -255
#define SAVED_E_FATAL -0xfff
#define SAVED_E_UNDEFINE -0xffff

#endif // !__DEFINE_H__