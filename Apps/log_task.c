#include "log_task.h"
#include <stdlib.h>
#include <stdio.h>
#include "usart.h"
#include "uart_ext.h"

#define LOG_PRINT_BUFFER_SIZE (1024u)

UartExt mLogUartExt;
char mLogPrintfBuffer[LOG_PRINT_BUFFER_SIZE];


int LogInit(){
    int result = UartExtInit(&mLogUartExt, &huart1, UART_EXT_TX_ONLY, LOG_PRINT_BUFFER_SIZE);
    if(result != 0) {
        return -1;
    }
    return 0;
}

int LogPrintf(const char *fmt, ...){
    int len;
    va_list args;
    va_start(args, fmt);
    len = LogVPrintf(fmt, args);
    va_end(args);
    return len;
}

int LogVPrintf(const char * fmt, va_list args){
	int len;
	char * buffer_ptr = mLogPrintfBuffer;
	len = vsnprintf(buffer_ptr, LOG_PRINT_BUFFER_SIZE, fmt, args);
	len = UartExtWrite(&mLogUartExt, (const uint8_t *)buffer_ptr, len);
	return len;
}

extern void * LogFlushTaskEntry(void * argument){
	LogPrintf(mLogPrintfBuffer);
  UartExtFlush(&mLogUartExt);
  return NULL;
}
