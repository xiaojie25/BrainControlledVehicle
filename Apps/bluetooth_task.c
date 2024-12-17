#include "bluetooth_task.h"
#include "getstimulation_task.h"
#include "dip_task.h"
#include "usart.h"
#include <stdlib.h>
#include <stdio.h>

void BluetoothInit(void){
	BluetoothPrintf("AT+ORGL\r\n");
	BluetoothPrintf("AT+PSWD=\"0000\"\r\n");
	BluetoothPrintf("AT+ROLE=1\r\n");
	BluetoothPrintf("AT+CMODE=0\r\n");
	BluetoothPrintf("AT+BIND=9A,B2,BD,FE,E8,77\r\n");
	//BluetoothPrintf("AT+BIND=0024,07,00710B\r\n");
	BluetoothPrintf("AT+BIND?\r\n");
}

int BluetoothPrintf(const char *fmt, ...){
	int len;
	va_list args;
	va_start(args, fmt);
	len = BluetoothVPrintf(fmt, args);
	va_end(args);
	return len;
}

int BluetoothVPrintf(const char * fmt, va_list args){
	int len;
	char buffer[64];
	len = vsnprintf(buffer, sizeof(buffer), fmt, args);
	HAL_UART_Transmit_DMA(&huart3, (uint8_t *)fmt, len);
	return len;
}

extern void * BluetoothSendEntry(void * argument){
	if(gSysMode != SysMode_2){
		return NULL;
	}
	BluetoothPrintf(&gFFTRes);
	gFFTRes = 0;
	return NULL;
}
