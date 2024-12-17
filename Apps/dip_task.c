#include "dip_task.h"
#include "gpio.h"
#include "stm32f4xx_hal.h"

GPIO_PinState DIP1;
GPIO_PinState DIP2;

SysMode gSysMode = SysMode_NULL;
int gSysModeIsChange = 0;

void * GetSysModeEntry(void * argument){
	DIP1 = HAL_GPIO_ReadPin(DIP1_GPIO_Port, DIP1_Pin);
	DIP2 = HAL_GPIO_ReadPin(DIP2_GPIO_Port, DIP2_Pin);
	
	if(gSysMode != (SysMode)((DIP2 << 4) + DIP1)){
		gSysMode = (SysMode)((DIP2 << 4) + DIP1);
		gSysModeIsChange = 1;
	}
	
	return 0;
}

