#ifndef __DIP_TASK_H__
#define __DIP_TASK_H__

#include "gpio.h"

typedef enum SysMode_{
	SysMode_0 = 0x00,
	SysMode_1 = 0x01,
	SysMode_2 = 0x10,
	SysMode_3 = 0x11,
	SysMode_NULL = 0xFF
}SysMode;

extern SysMode gSysMode;
extern int gSysModeIsChange;

void * GetSysModeEntry(void * argument);

#endif //__DIP_TASK_H__

