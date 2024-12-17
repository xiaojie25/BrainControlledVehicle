#ifndef __LOG_TASK_H__
#define __LOG_TASK_H__

#include <stdarg.h>


extern void * LogFlushTaskEntry(void * argument);

extern int LogInit(void);

extern int LogVPrintf ( const char * fmt, va_list args);

extern int LogPrintf(const char *fmt, ...);


#endif // __LOG_TASK_H__
