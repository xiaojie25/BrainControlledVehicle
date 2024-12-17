#include "stimulation_task.h"
#include "tim.h"

void * StimulationTaskEntry(void * argument){

	if(gSysMode == SysMode_2){
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
		HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);
	}else if(gSysMode == SysMode_1){
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(&htim12, TIM_CHANNEL_1);
	}
	
	return NULL;
}
