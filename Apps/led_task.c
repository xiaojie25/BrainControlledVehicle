#include "led_task.h"
#include "main.h"
#include "tim.h"
#include "dip_task.h"

void * LedTaskEntry(void * argument){
	if(gSysMode == SysMode_2 || gSysMode == SysMode_3){
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		return NULL;
	}
	return NULL;
}
