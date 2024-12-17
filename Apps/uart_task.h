#ifndef __UART_TASK_H__
#define __UART_TASK_H__

#include "uart_ext.h"
#include <stdarg.h>

int UartTaskInit(void);
extern void * UartDataFlushTaskEntry(void * argument);

#endif //__UART_TASK_H__
