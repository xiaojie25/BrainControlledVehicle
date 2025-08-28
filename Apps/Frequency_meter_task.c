#include "Frequency_meter_task.h"
#include "getstimulation_task.h"
#include "stm32f4xx_hal.h"
#include "dip_task.h"
#include "ads1299_task.h"
#include "ring_buffer.h"
#include "usart.h"
#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"
#include "delay.h"

#define LSB 0.0224
#define ALPHA_AVERAGR_MID(arr) ((arr)[9] + (arr)[10] + (arr)[11])/3 
#define ALPHA_AVERAGR_L(arr) ((arr)[5] + (arr)[6] + (arr)[7] + (arr)[8])/4 
#define ALPHA_AVERAGR_R(arr) ((arr)[13] + (arr)[14] + (arr)[15])/3 

const float UserCarRaceWeight = 1.5f;

static char cCarIsRacing(float32_t *pDst){
//	pDst[9] + pDst[10] + pDst[11]
	return (ALPHA_AVERAGR_MID(pDst) > UserCarRaceWeight * (ALPHA_AVERAGR_L(pDst) + ALPHA_AVERAGR_R(pDst))/2) ? 0x1 :0x05;
}

void * FrequencyMeterTaskEntry(void * argument){
	if(gSysMode != SysMode_3){
	return NULL;
	}
	float fft_outputbuf[ROWS];
	float fft_inputbuf[ROWS*2];
	int ADS1299Data[COLS_X][ROWS];
	uint8_t Uint8Data[ROWS * COLS_X * 3];
	uint32_t Combined;
	uint8_t FrequencyMeterHead = 0x2A;
	uint8_t FrequencyMeterTail = 0x2B;
	int FrequencyMeterLine[4];
	FrequencyMeterLine[0] = FrequencyMeterHead;
	FrequencyMeterLine[3] = FrequencyMeterTail;
	
	int Len = ROWS * COLS_X * 3;
	int NowLen = RingBufferGetDistanceToBoundary(&mADS1299DataRingBuffer);
	
	if(Len > NowLen){
		return NULL;
	}

	RingBufferPop(&mADS1299DataRingBuffer, Uint8Data, Len);
	
	int ptr_move = 0;
	for(int j = 0; j < ROWS; j++){
		for(int i = 0; i < COLS_X; i++){//CHANNEL
			Combined = (Uint8Data[ptr_move] << 16)|(Uint8Data[ptr_move+1] << 8)|Uint8Data[ptr_move+2];
			if (Combined & 0x800000){ // sign bit
        ADS1299Data[i][j] = ((int)(Combined | 0xFF000000)); // expand sign bit
			}else{
        ADS1299Data[i][j] =((int) Combined); // keep on
			}
			ptr_move += 3;
		}
	}

	for(int i = 0;i<ROWS;i++){
		fft_inputbuf[i*2] = ((float)ADS1299Data[0][i])*(float)LSB;//uV
		fft_inputbuf[i*2 + 1] = 0; 
	}
	arm_cfft_radix4_instance_f32 scfft;
	arm_cfft_radix4_init_f32(&scfft, ROWS, 0, 1);
	arm_cfft_radix4_f32(&scfft,fft_inputbuf);
	arm_cmplx_mag_f32(fft_inputbuf,fft_outputbuf,ROWS);
	
	gFFTRes = cCarIsRacing(fft_outputbuf);

	for(int i=0; i<FREQUENCYMETERLEN; i++){
		float DataTemp = fft_outputbuf[i];
		FrequencyMeterLine[1] = DataTemp;
		FrequencyMeterLine[2] = i;
		for(int j=0; j<100; j++){
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)FrequencyMeterLine, sizeof(FrequencyMeterLine));
			while(huart1.gState != HAL_UART_STATE_READY);
		}
	}
	mADS1299DataRingBuffer.head_index = 0;
	mADS1299DataRingBuffer.tail_index = 0;
	mADS1299DataRingBuffer.is_full = 0;

	return NULL;
}
