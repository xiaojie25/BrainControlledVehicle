#ifndef __ADS1299_TASK_H__
#define __ADS1299_TASK_H__

#include "ADS1299.h"
#include "ring_buffer.h"

typedef struct ADS1299Configure_{
	ADS1299_CONFIG1 config1;
	ADS1299_CONFIG2 config2;
	ADS1299_CONFIG3 config3;
	ADS1299_LOFF loff;
	ADS1299_LOFFSENSN loffsensn;
	ADS1299_LOFFSENSP loffsensp;
	ADS1299_LOFFFLIP loffflip;
	ADS1299_CHnSET chnset;
	ADS1299_BIASSENSP biassensp;
	ADS1299_BIASSENSN biassensn;
	ADS1299_MISC1 misc1;
	ADS1299_CONFIG4 config4;
}ADS1299Configure;

#define CONFIG1_DR_DO_NOT_USE 					(0x7u)
#define CONFIG1_DR_250SPS 							(0x6u)
#define CONFIG1_DR_500SPS 							(0x5u)
#define CONFIG1_DR_1KSPS 								(0x4u)
#define CONFIG1_DR_2KSPS 								(0x3u)
#define CONFIG1_DR_4KSPS 								(0x2u)
#define CONFIG1_DR_8KSPS 								(0x1u)
#define CONFIG1_DR_16KSPS 							(0x0u)
#define CONFIG1_RESERVED4								(0B10u)
#define CONFIG1_CLK_EN_ENABLE						(0B1u)
#define CONFIG1_CLK_EN_DISABLE					(0B0u)
#define CONFIG1_DAISTEN_ENABLE					(0B0u)
#define CONFIG1_DAISTEN_DISABLE					(0B1u)
#define CONFIG1_RESERVED7								(0B1u)

#define CONFIG2_CAL_FREQ_DC							(0B11u)
#define CONFIG2_CAL_FREQ_DO_NOT_USE			(0B10u)
#define CONFIG2_CAL_FREQ_X20						(0B01u)
#define CONFIG2_CAL_FREQ_X21						(0B00u)
#define CONFIG2_CAL_AMP_X2							(0B1u)
#define CONFIG2_CAL_AMP_X1							(0B0u)
#define CONFIG2_CAL_RESERVED3						(0B0u)
#define CONFIG2_INT_CAL_TEST_INTER			(0B1u)
#define CONFIG2_INT_CAL_TEST_EXTER			(0B0u)
#define CONFIG2_CAL_RESERVED7						(0B110u)

#define CONFIG3_BIAS_STAT_DISABLE				(0B1u)
#define CONFIG3_BIAS_STAT_ENABLE				(0B0u)
#define CONFIG3_BIAS_LOFF_SENS_ENABLE		(0B1u)
#define CONFIG3_BIAS_LOFF_SENS_DISABLE	(0B0u)
#define CONFIG3_PD_BIAS_ENABLE					(0B1u)
#define CONFIG3_PD_BIAS_DISABLE					(0B0u)
#define CONFIG3_BIAS_MEAS_VREF					(0B1u)
#define CONFIG3_BIAS_MEAS_OPEN					(0B0u)
#define CONFIG3_RESERVED6								(0B11u)
#define CONFIG3_PD_REFBUF_ENABLE				(0B1u)
#define CONFIG3_PD_REFBUF_DISABLE				(0B0u)

#define CHnSET_MUXn_BIAS_DRN						(0B111u)
#define CHnSET_MUXn_BIAS_DRP						(0B110u)
#define CHnSET_MUXn_TEST								(0B101u)
#define CHnSET_MUXn_TEMP								(0B100u)
#define CHnSET_MUXn_BIAS								(0B010u)
#define CHnSET_MUXn_INPUT_SHORT					(0B001u)
#define CHnSET_MUXn_NORMAL							(0B000u)

#define BIASSENSP_BIASP1_ENABLE					(0B1u)
#define BIASSENSP_BIASP1_DISABLE				(0B0u)
#define BIASSENSP_BIASP2_ENABLE					(0B1u)
#define BIASSENSP_BIASP2_DISABLE				(0B0u)
#define BIASSENSP_BIASP3_ENABLE					(0B1u)
#define BIASSENSP_BIASP3_DISABLE				(0B0u)
#define BIASSENSP_BIASP4_ENABLE					(0B1u)
#define BIASSENSP_BIASP4_DISABLE				(0B0u)
#define BIASSENSP_BIASP5_ENABLE					(0B1u)
#define BIASSENSP_BIASP5_DISABLE				(0B0u)
#define BIASSENSP_BIASP6_ENABLE					(0B1u)
#define BIASSENSP_BIASP6_DISABLE				(0B0u)
#define BIASSENSP_BIASP7_ENABLE					(0B1u)
#define BIASSENSP_BIASP7_DISABLE				(0B0u)
#define BIASSENSP_BIASP8_ENABLE					(0B1u)
#define BIASSENSP_BIASP8_DISABLE				(0B0u)

#define BIASSENSN_BIASP1_ENABLE					(0B1u)
#define BIASSENSN_BIASP1_DISABLE				(0B0u)
#define BIASSENSN_BIASP2_ENABLE					(0B1u)
#define BIASSENSN_BIASP2_DISABLE				(0B0u)
#define BIASSENSN_BIASP3_ENABLE					(0B1u)
#define BIASSENSN_BIASP3_DISABLE				(0B0u)
#define BIASSENSN_BIASP4_ENABLE					(0B1u)
#define BIASSENSN_BIASP4_DISABLE				(0B0u)
#define BIASSENSN_BIASP5_ENABLE					(0B1u)
#define BIASSENSN_BIASP5_DISABLE				(0B0u)
#define BIASSENSN_BIASP6_ENABLE					(0B1u)
#define BIASSENSN_BIASP6_DISABLE				(0B0u)
#define BIASSENSN_BIASP7_ENABLE					(0B1u)
#define BIASSENSN_BIASP7_DISABLE				(0B0u)
#define BIASSENSN_BIASP8_ENABLE					(0B1u)
#define BIASSENSN_BIASP8_DISABLE				(0B0u)

#define MISC1_RESERVED4									(0B00000u)
#define MISC1_SRB1_CLOSE								(0B1u)
#define MISC1_SRB1_OPEN									(0B0u)
#define MISC1_RESERVED7									(0B00u)

#define MISC2_RESERVED4									(0B00000000u)

#define CONFIG4_RESERVED0								(0B0u) 
#define CONFIG4_PD_LOFF_COMP_ENABLE			(0B1u)
#define CONFIG4_PD_LOFF_COMP_DISABLE		(0B0u)
#define CONFIG4_RESERVED2								(0B0u)
#define CONFIG4_SINGLE_SHOT_SHOT_MODE		(0B1u)
#define CONFIG4_SINGLE_SHOT_CONTINUOUS	(0B0u)
#define CONFIG4_RESERVED7								(0B0000u)

#define ADS1299_DATA_LENGTH      				(24u)
#define ADS1299_TOTAL_LENGTH						(27u)

extern char DataReadly;
extern RingBuffer mADS1299DataRingBuffer;

extern void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
int ADS1299TaskInit(void);
int ADS1299TaskCigFlush(ADS1299Configure * mADS1299Configure);
int ADS1299TaskStart(void);
int ADS1299TaskUpdateFruq(ADS1299Configure * mADS1299Configure,ADS1299_CONFIG1 * config1, uint8_t bit);
void * ADS1299TaskInterruptEntry(void * argument);

#endif //__ADS1299_TASK_H__













