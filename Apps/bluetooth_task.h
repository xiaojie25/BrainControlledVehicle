#ifndef __BLUETOOTH_TASK_H__
#define __BLUETOOTH_TASK_H__

#include <stdarg.h>

void BluetoothInit(void);
extern int BluetoothVPrintf(const char * fmt, va_list args);
extern int BluetoothPrintf(const char *fmt, ...);
extern void * BluetoothSendEntry(void * argument);

#endif //__BLUETOOTH_TASK_H__
