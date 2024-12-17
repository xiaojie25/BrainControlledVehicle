#include "led_task.h"
#include "main.h"
#include "tim.h"

void * LedTaskEntry(void * argument){
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	
	return NULL;
}
