#include "uart_task.h"
#include "usart.h"
#include <stdlib.h>
#include <stdio.h>
#include "ads1299_task.h"
#include "ring_buffer.h"
#include "dip_task.h"

UartExt mDataUartExt;

int UartTaskInit(){
    int result = UartExtInit(&mDataUartExt, &huart1, UART_EXT_TX_ONLY, mADS1299DataRingBuffer.size);
    if(result != 0) {
      return -1;
    }
		if(mDataUartExt.mode == UART_EXT_TX_ONLY){
			mDataUartExt.tx_buffer = &mADS1299DataRingBuffer;
			return 0;
		}else if(mDataUartExt.mode == UART_EXT_RX_ONLY){
			mDataUartExt.rx_buffer = &mADS1299DataRingBuffer;
			return 0;
		}else{
			mDataUartExt.tx_buffer = &mADS1299DataRingBuffer;
			mDataUartExt.rx_buffer = &mADS1299DataRingBuffer;
			return 0;
		}
}

extern void * UartDataFlushTaskEntry(void * argument){
	if(gSysMode != SysMode_1){
		return NULL;
	}
	UartExtFlush(&mDataUartExt);
	return NULL;
	
//	return NULL;
}
