#include "delay.h"

void DelayUs(uint16_t nus){
	uint16_t differ = 0xffff - nus -5;
	__HAL_TIM_SET_COUNTER(&htim2, differ);
	HAL_TIM_Base_Start(&htim2);
	
	while(differ < 0xffff - 5){
		differ = __HAL_TIM_GET_COUNTER(&htim2);
	}
	HAL_TIM_Base_Stop(&htim2);
}
