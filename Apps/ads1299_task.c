#include "ads1299_task.h"
#include "ADS1299.h"
#include "gpio.h"
#include "ring_buffer.h"
#include "uart_ext.h"
#include <stdlib.h>
#include "dip_task.h"
#include "usart.h"
#include <stdio.h>
#include "fdacoefs.h"

#define ADS1299_DATA_BUFFER_SIZE (8000u)
#define ADS1299_DATA_LENGTH      (24u)

#define TAG_PACKAGE_LEN 10
#define TAG_FREQ 0
#define TAG_LATENCY_START 1
#define TAG_LATENCY_LEN 8
#define TAG_UREVENT 9

char gDataReady = 0;
RingBuffer mADS1299DataRingBuffer;
ADS1299Configure mADS1299Configure;
uint64_t ADS1299DataCnt = 0;
uint8_t PACKAGE_HAND = 0XAA;
uint8_t PACKAGE_TAIL = 0XBB;
uint64_t Data_cnt = 0;
uint8_t TagPackage[TAG_PACKAGE_LEN];
uint8_t DataPackage[8];
uint8_t Freq = 0;
uint8_t urevent = 0;
IIRFilter iir[8];

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == ADS1299_DRDY_Pin){
		
		ADS1299_CS_ENABLE;
		
		gDataReady = 1;
	}
}

int ADS1299TaskInit(void){
	if(RingBufferInit(&mADS1299DataRingBuffer, ADS1299_DATA_BUFFER_SIZE) != 0){
		return -1;
	}
	for(int i=0; i<8; i++){
		IIRFilter_Init(&iir[i], NUM_250, DEN_250);
	}

	ADS1299WakeUp();

	/* Normal Common Reference Collection Mode */
	mADS1299Configure.config1.val = 0xD6;
	mADS1299Configure.config2.val = 0xC0;
	mADS1299Configure.config3.val = 0xEC;
	mADS1299Configure.config4.val = 0x00;
	mADS1299Configure.loff.val = 0x00;
	mADS1299Configure.loffsensn.val = 0x00;
	mADS1299Configure.loffsensp.val = 0x00;
	mADS1299Configure.biassensn.val = 0xFF;
	mADS1299Configure.biassensp.val = 0xFF;
	mADS1299Configure.loffflip.val = 0x00;
	mADS1299Configure.misc1.val = 0x20;
	mADS1299Configure.chnset.val = 0x60;
	
	/* Co-referenced impedance detection mode */
//	mADS1299Configure.config1.val = 0xD6;
//	mADS1299Configure.config2.val = 0xC0;
//	mADS1299Configure.config3.val = 0xEC;
//	mADS1299Configure.config4.val = 0x00;
//	mADS1299Configure.loff.val = 0x06;
//	mADS1299Configure.loffsensn.val = 0x00;
//	mADS1299Configure.loffsensp.val = 0x02;
//	mADS1299Configure.biassensn.val = 0xFF;
//	mADS1299Configure.biassensp.val = 0xFF;
//	mADS1299Configure.loffflip.val = 0x00;
//	mADS1299Configure.misc1.val = 0x20;
//	mADS1299Configure.chnset.val = 0x60;
	
	ADS1299WriteREG(ADS1299_REG_CONFIG1, mADS1299Configure.config1.val);
	ADS1299WriteREG(ADS1299_REG_CONFIG2, mADS1299Configure.config2.val);
	ADS1299WriteREG(ADS1299_REG_CONFIG3, mADS1299Configure.config3.val);
	ADS1299WriteREG(ADS1299_REG_CONFIG4, mADS1299Configure.config4.val);
	ADS1299WriteREG(ADS1299_REG_LOFF, mADS1299Configure.loff.val);
	ADS1299WriteREG(ADS1299_REG_LOFFSENSN, mADS1299Configure.loffsensn.val);
	ADS1299WriteREG(ADS1299_REG_LOFFSENSP, mADS1299Configure.loffsensp.val);
	ADS1299WriteREG(ADS1299_REG_BIASSENSN, mADS1299Configure.biassensn.val);
	ADS1299WriteREG(ADS1299_REG_BIASSENSP, mADS1299Configure.biassensp.val);
	ADS1299WriteREG(ADS1299_REG_LOFFFLIP, mADS1299Configure.loffflip.val);
	ADS1299WriteREG(ADS1299_REG_MISC1, mADS1299Configure.misc1.val);
	
	int CHnSET_move = 8;
	while(CHnSET_move){
		ADS1299WriteREG(ADS1299_REG_CH1SET + CHnSET_move -1, mADS1299Configure.chnset.val);
		if(mADS1299Configure.chnset.val != ADS1299ReadREG(ADS1299_REG_CH1SET + CHnSET_move -1)){
			return -1;
		}
		--CHnSET_move;
	}
	
	
	return ADS1299TaskStart();
}

int ADS1299TaskStart(void){
	ADS1299SendCommand(ADS1299_CMD_WAKEUP);
	int i = 4;
	while(i--);
	ADS1299_CS_ENABLE;
	ADS1299SendCommand(ADS1299_CMD_RDATAC);
	
	ADS1299_DRDY_ENABLE;
	HAL_GPIO_WritePin(ADS1299_START_GPIO_Port, ADS1299_START_Pin, GPIO_PIN_SET);
	
	return 0;
}

int ADS1299TaskUpdateFruq(ADS1299Configure * mADS1299Configure,ADS1299_CONFIG1 * config1, uint8_t bit){
	if(config1 == NULL || mADS1299Configure == NULL){
		return -1;
	}
	mADS1299Configure->config1.control_bit.dr = bit;
	ADS1299WriteREG(ADS1299_REG_CONFIG1, mADS1299Configure->config1.val);
	if(mADS1299Configure->config1.val != ADS1299ReadREG(ADS1299_REG_CONFIG1)){
		return -1;
	}
	
	return ADS1299TaskStart();
}

void * ADS1299TaskInterruptEntry(void * argument){
	if(gSysModeIsChange){
		RingBufferDeinit(&mADS1299DataRingBuffer);
		if(RingBufferInit(&mADS1299DataRingBuffer, ADS1299_DATA_BUFFER_SIZE) != 0){
		return NULL;
	}
		if(gSysMode == SysMode_1 || gSysMode == SysMode_0){
			gSysModeIsChange = 0;
			ADS1299SendCommand(ADS1299_CMD_STOP);
			mADS1299Configure.config1.control_bit.dr = CONFIG1_DR_250SPS;
			mADS1299Configure.chnset.val = 0x60;
			int CHnSET_move = 8;
			while(CHnSET_move){
				ADS1299WriteREG(ADS1299_REG_CH1SET + CHnSET_move -1, mADS1299Configure.chnset.val);
				--CHnSET_move;
			}
			ADS1299WriteREG(ADS1299_REG_CONFIG1, mADS1299Configure.config1.val);
			mADS1299Configure.biassensn.val = 0xFF;
			mADS1299Configure.biassensp.val = 0xFF;
			ADS1299WriteREG(ADS1299_REG_BIASSENSN, mADS1299Configure.biassensn.val);
			ADS1299WriteREG(ADS1299_REG_BIASSENSP, mADS1299Configure.biassensp.val);
			ADS1299TaskStart();

		}//SysMode_1
		if(gSysMode == SysMode_2 || gSysMode == SysMode_3){
			gSysModeIsChange = 0;
			
			ADS1299SendCommand(ADS1299_CMD_STOP);
			mADS1299Configure.config1.control_bit.dr = CONFIG1_DR_250SPS;
			mADS1299Configure.chnset.val = 0xE1;
			int CHnSET_move = 8;
			while(CHnSET_move){
				ADS1299WriteREG(ADS1299_REG_CH1SET + CHnSET_move -1, mADS1299Configure.chnset.val);
				--CHnSET_move;
			}
			mADS1299Configure.chnset.val = 0x60;
			CHnSET_move = 1;
			while(CHnSET_move){
				ADS1299WriteREG(ADS1299_REG_CH1SET + CHnSET_move -1, mADS1299Configure.chnset.val);
				--CHnSET_move;
			}
			ADS1299WriteREG(ADS1299_REG_CONFIG1, mADS1299Configure.config1.val);
			mADS1299Configure.biassensn.val = 0x01;
			mADS1299Configure.biassensp.val = 0x01;
			ADS1299WriteREG(ADS1299_REG_BIASSENSN, mADS1299Configure.biassensn.val);
			ADS1299WriteREG(ADS1299_REG_BIASSENSP, mADS1299Configure.biassensp.val);
			ADS1299TaskStart();
		}//SysMode_2
		
	} //gSysModeIsChange
		if(gDataReady){
		gDataReady = 0;
		++ADS1299DataCnt;
		//TODO TAG
		if(StartFlag){
			Freq = RxFreq[0];
			TagPackage[TAG_FREQ] = Freq;
			TagPackage[TAG_UREVENT] = ++urevent;
			for(int i = 0; i<TAG_LATENCY_LEN; i++){
				TagPackage[TAG_LATENCY_START + i] = (ADS1299DataCnt>>(56-8*i)) & 0xFF;
			}
			StartFlag = 0;
			Freq = 0;
		}
		for(int i = 0; i<8; i++){
			DataPackage[i] = (ADS1299DataCnt>>(56-8*i)) & 0xFF;
		}
		uint8_t * ADS1299Data = malloc(ADS1299_TOTAL_LENGTH);
		uint8_t * ADS1299DataCut50Hz = (uint8_t *)malloc(ADS1299_DATA_LENGTH);
		ADS1299ReadByte(ADS1299Data);
		if(gSysMode == SysMode_1){
			RingBufferPush(&mADS1299DataRingBuffer, &PACKAGE_HAND, sizeof(PACKAGE_HAND));
			RingBufferPush(&mADS1299DataRingBuffer, ADS1299Data+ADS1299_TOTAL_LENGTH-ADS1299_DATA_LENGTH, ADS1299_DATA_LENGTH);
			RingBufferPush(&mADS1299DataRingBuffer, DataPackage, 8);
			RingBufferPush(&mADS1299DataRingBuffer, &TagPackage, TAG_PACKAGE_LEN);
			RingBufferPush(&mADS1299DataRingBuffer, &PACKAGE_TAIL, sizeof(PACKAGE_TAIL));
		}else if(gSysMode == SysMode_0){
			for(int i=0; i<8; i++){
				int Output = 0;
				Output = Decoder(&ADS1299Data[i*3+3], 3);
				Output = (int)IIRFilter_Process(&iir[i], (double)Output);
				Encoding(&Output, &ADS1299DataCut50Hz[i*3]);
			}//filter
			RingBufferPush(&mADS1299DataRingBuffer, &PACKAGE_HAND, sizeof(PACKAGE_HAND));
			RingBufferPush(&mADS1299DataRingBuffer, ADS1299DataCut50Hz, ADS1299_DATA_LENGTH);
			RingBufferPush(&mADS1299DataRingBuffer, DataPackage, 8);
			RingBufferPush(&mADS1299DataRingBuffer, &TagPackage, TAG_PACKAGE_LEN);
			RingBufferPush(&mADS1299DataRingBuffer, &PACKAGE_TAIL, sizeof(PACKAGE_TAIL));
		}else if(gSysMode == SysMode_2 || gSysMode == SysMode_3){
			RingBufferPush(&mADS1299DataRingBuffer, ADS1299Data+ADS1299_TOTAL_LENGTH-ADS1299_DATA_LENGTH, ADS1299_DATA_LENGTH);
		}
		free(ADS1299Data);
		free(ADS1299DataCut50Hz);
		}
	
		return NULL;
}

