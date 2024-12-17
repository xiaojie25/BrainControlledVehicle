/**
 ****************************************************************************************************
 * @file        ADS1299.C
 * @author      XJIE
 * @version     V1.00
 * @date        2023-11-21
 * @brief       This file is the driver for the ADS1299 and its main user functions.
 *           
 ****************************************************************************************************
 * @attention		NULL
 *
 * CSDN:				https://blog.csdn.net/NANY_ying.com
 * Github:			https://github.com/xiaojie25
 *
 * 
 *
 ****************************************************************************************************
 */

#ifndef __ADS1299_H__
#define __ADS1299_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef union{
	uint8_t val;
	struct{
		uint8_t nu_ch 							: 2;
		uint8_t dev_id 							: 2;
		uint8_t	reserved4 					: 1;
		uint8_t rev_id 							: 3;
	}control_bit;
}ADS1299_ID;

typedef union{
	uint8_t val;
	struct{
		uint8_t dr 									: 3;
		uint8_t reserved4 					: 2;
    uint8_t clk_en 							: 1;
    uint8_t daisy_en 						: 1;
    uint8_t reserved7 					: 1;
	}control_bit;
}ADS1299_CONFIG1;

typedef union{
	uint8_t val;
  struct{
		uint8_t cal_freq 						: 2;
    uint8_t cal_amp 						: 1;
    uint8_t reserved3 					: 1;
    uint8_t int_cal 						: 1;
    uint8_t reserved7 					: 3;
	}control_bit;
}ADS1299_CONFIG2;

typedef union{
  uint8_t val;
  struct{  
		uint8_t bias_stat          	 : 1;
		uint8_t bias_loff_sens       : 1;
		uint8_t pd_bias           	 : 1;
		uint8_t biasref_int        	 : 1;
		uint8_t bias_meas          	 : 1;
		uint8_t reserved6            : 2;
		uint8_t pd_brefbuf         	 : 1;
   }control_bit;
} ADS1299_CONFIG3;

typedef union{
	uint8_t val;
  struct{
		uint8_t flead_off          	 : 2;
		uint8_t ilead_off          	 : 2;
		uint8_t reserved4            : 1;
		uint8_t comp_th            	 : 3;
	}control_bit;
}ADS1299_LOFF;

typedef union{
	uint8_t val;
	struct{
		uint8_t muxn                 : 3;
		uint8_t srb2               	 : 1;
		uint8_t gainn                : 3;
		uint8_t pdn                  : 1;
	}control_bit;
}ADS1299_CHnSET;

typedef union{
	uint8_t val;
	struct{
		uint8_t BIAS1p             	: 1;
		uint8_t BIAS2p            	: 1;
		uint8_t BIAS3p             	: 1;
		uint8_t BIAS4p             	: 1;
		uint8_t BIAS5p             	: 1;
		uint8_t BIAS6p             	: 1;
		uint8_t BIAS7p             	: 1;
		uint8_t BIAS8p             	: 1;
	}control_bit;
}ADS1299_BIASSENSP;

typedef union{
	uint8_t val;
	struct{
		uint8_t BIAS1n             	: 1;
		uint8_t BIAS2n             	: 1;
		uint8_t BIAS3n             	: 1;
		uint8_t BIAS4n             	: 1;
		uint8_t BIAS5n             	: 1;
		uint8_t BIAS6n             	: 1;
		uint8_t BIAS7n             	: 1;
		uint8_t BIAS8n             	: 1;
	}control_bit;
}ADS1299_BIASSENSN;

typedef union{
	uint8_t val;
	struct{
		uint8_t loff1p             	: 1;
		uint8_t loff2p             	: 1;
		uint8_t loff3p             	: 1;
		uint8_t loff4p             	: 1;
		uint8_t loff5p             	: 1;
		uint8_t loff6p             	: 1;
		uint8_t loff7p             	: 1;
		uint8_t loff8p             	: 1;
	}control_bit;
}ADS1299_LOFFSENSP;

typedef union{
	uint8_t val;
	struct{
		uint8_t loff1n             	: 1;
		uint8_t loff2n             	: 1;
		uint8_t loff3n             	: 1;
		uint8_t loff4n             	: 1;
		uint8_t loff5n             	: 1;
		uint8_t loff6n             	: 1;
		uint8_t loff7n             	: 1;
		uint8_t loff8n             	: 1;
	}control_bit;
}ADS1299_LOFFSENSN;

typedef union{
	uint8_t val;
	struct{
		uint8_t loffflip1          	: 1;
		uint8_t loffflip2          	: 1;
		uint8_t loffflip3          	: 1;
		uint8_t loffflip4          	: 1;
		uint8_t loffflip5          	: 1;
		uint8_t loffflip6          	: 1;
		uint8_t loffflip7          	: 1;
		uint8_t loffflip8          	: 1;
	}control_bit;
}ADS1299_LOFFFLIP;

typedef union{
	uint8_t val;
	struct{
		uint8_t in1poff            	: 1;
		uint8_t in2poff            	: 1;
		uint8_t in3poff            	: 1;
		uint8_t in4poff            	: 1;
		uint8_t in5poff            	: 1;
		uint8_t in6poff            	: 1;
		uint8_t in7poff            	: 1;
		uint8_t in8poff            	: 1;
	}control_bit;
}ADS1299_LOFFSTATP;

typedef union{
	uint8_t val;
	struct{
		uint8_t in1noff            	: 1;
		uint8_t in2noff            	: 1;
		uint8_t in3noff            	: 1;
		uint8_t in4noff            	: 1;
		uint8_t in5noff            	: 1;
		uint8_t in6noff            	: 1;
		uint8_t in7noff            	: 1;
		uint8_t in8noff            	: 1;
	}control_bit;
}ADS1299_LOFFSTATN;

typedef union{
	uint8_t val;
	struct{
		uint8_t gpioc1             	: 1;
		uint8_t gpioc2             	: 1;
		uint8_t gpioc3             	: 1;
		uint8_t gpioc4             	: 1;
		uint8_t gpiod1             	: 1;
		uint8_t gpiod2             	: 1;
		uint8_t gpiod3             	: 1;
		uint8_t gpiod4             	: 1;
	}control_bit;
}ADS1299_GPIO;

typedef union{
	uint8_t val;
	struct{
		uint8_t reserved4           : 5;
		uint8_t srb1               	: 1;
		uint8_t reserved7           : 2;
	}control_bit;
}ADS1299_MISC1;

typedef union{
	uint8_t val;
	struct{
		uint8_t reserved            : 8;
	}control_bit;
}ADS1299_MISC2;

typedef union{
	uint8_t val;
	struct{
		uint8_t rsv0               	: 1;
		uint8_t pdbloffcomp        	: 1;
		uint8_t rsv2               	: 1;
		uint8_t singleshot         	: 1;
		uint8_t rsv7                : 4;
	}control_bit;
}ADS1299_CONFIG4;

#define ADS1299_CMD_RREG                        (0x20u)
#define ADS1299_CMD_WREG                        (0x40u)
#define ADS1299_CMD_WAKEUP                      (0x02u)
#define ADS1299_CMD_STANDBY                     (0x04u)
#define ADS1299_CMD_RESET                       (0x06u)
#define ADS1299_CMD_START                       (0x08u)
#define ADS1299_CMD_STOP                        (0x0Au)
#define ADS1299_CMD_RDATAC                      (0x10u)
#define ADS1299_CMD_RDATA												(0x12u)
#define ADS1299_CMD_SDATAC                      (0x11u)
#define ADS1299_CMD_INITDEVICE                  (0x0100u)

#define ADS1299_REG_DEVID         							(0x0000u)
#define ADS1299_REG_CONFIG1      					 			(0x0001u)
#define ADS1299_REG_CONFIG2       							(0x0002u)
#define ADS1299_REG_CONFIG3       							(0x0003u)
#define ADS1299_REG_LOFF          							(0x0004u)
#define ADS1299_REG_CH1SET        							(0x0005u)
#define ADS1299_REG_CH2SET        							(0x0006u)
#define ADS1299_REG_CH3SET        							(0x0007u)
#define ADS1299_REG_CH4SET        							(0x08u)
#define ADS1299_REG_CH5SET        							(0x09u)
#define ADS1299_REG_CH6SET        							(0x0Au)
#define ADS1299_REG_CH7SET        							(0x0Bu)
#define ADS1299_REG_CH8SET        							(0x0Cu)
#define ADS1299_REG_BIASSENSP     							(0x0Du)
#define ADS1299_REG_BIASSENSN     							(0x0Eu)
#define ADS1299_REG_LOFFSENSP     							(0x0Fu)
#define ADS1299_REG_LOFFSENSN     							(0x10u)
#define ADS1299_REG_LOFFFLIP      							(0x11u)
#define ADS1299_REG_LOFFSTATP     							(0x12u)
#define ADS1299_REG_LOFFSTATN     							(0x13u)
#define ADS1299_REG_GPIO          							(0x14u)
#define ADS1299_REG_MISC1         							(0x15u)
#define ADS1299_REG_MISC2         							(0x16u)
#define ADS1299_REG_CONFIG4       							(0x17u)

#define ADS1299_RESET_DOWN  		HAL_GPIO_WritePin(ADS1299_RESET_GPIO_Port, ADS1299_RESET_Pin, GPIO_PIN_RESET)
#define ADS1299_RESET_PULL  		HAL_GPIO_WritePin(ADS1299_RESET_GPIO_Port, ADS1299_RESET_Pin, GPIO_PIN_SET)
#define ADS1299_CS_DISABLE 			HAL_GPIO_WritePin(ADS1299_CS1_GPIO_Port, ADS1299_CS1_Pin, GPIO_PIN_SET)
#define ADS1299_CS_ENABLE 			HAL_GPIO_WritePin(ADS1299_CS1_GPIO_Port, ADS1299_CS1_Pin, GPIO_PIN_RESET)
#define ADS1299_PWDN_DOWN				HAL_GPIO_WritePin(ADS1299_PWDN_GPIO_Port, ADS1299_PWDN_Pin, GPIO_PIN_RESET)
#define ADS1299_PWDN_PULL				HAL_GPIO_WritePin(ADS1299_PWDN_GPIO_Port, ADS1299_PWDN_Pin, GPIO_PIN_SET)
#define ADS1299_DRDY_ENABLE			HAL_NVIC_EnableIRQ(EXTI1_IRQn);
#define ADS1299_DRDY_DISABLE		HAL_NVIC_DisableIRQ(EXTI1_IRQn);
#define ADS1299_START_DOWN			HAL_GPIO_WritePin(ADS1299_START_GPIO_Port, ADS1299_START_Pin, GPIO_PIN_RESET)
#define ADS1299_START_PULL			HAL_GPIO_WritePin(ADS1299_START_GPIO_Port, ADS1299_START_Pin, GPIO_PIN_SET)

static void ADS1299Reset(void);
static void ADS1299PowerOn(void);
void ADS1299WakeUp(void);
void ADS1299SendCommand(uint8_t command);
void ADS1299WriteREG (uint8_t address, uint8_t value);
uint8_t ADS1299ReadREG (uint8_t address);
void ADS1299ReadByte(uint8_t *rp);
uint8_t ADS1299ModeConfig(uint8_t Mode);

#ifdef __cplusplus
}
#endif

#endif //__ADS1299_H__
