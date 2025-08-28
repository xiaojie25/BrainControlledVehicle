#include "ADS1299.h"
#include "main.h"
#include "gpio.h"
#include "spi.h"
#include "delay.h"

static void ADS1299Reset(void);
static void ADS1299PowerOn(void);

static void ADS1299Reset(void){
	ADS1299_RESET_DOWN;
	
	DelayUs(6);
	
	ADS1299_RESET_PULL;
	ADS1299_CS_DISABLE;
	
	DelayUs(200);		      	  // 18 tclk
}

static void ADS1299PowerOn(void){
	ADS1299_PWDN_PULL;
	ADS1299_RESET_PULL;
	
	DelayUs(200);		      	  // 18 tclk
}


void ADS1299WakeUp(void){
	ADS1299_DRDY_DISABLE;
	
	ADS1299PowerOn();
	ADS1299Reset();	
	
	ADS1299_START_PULL;
	while(HAL_GPIO_ReadPin(ADS1299_DRDY_GPIO_Port, ADS1299_DRDY_Pin) == GPIO_PIN_SET);
	ADS1299_START_DOWN;
	
	ADS1299Reset();
	ADS1299SendCommand(ADS1299_CMD_RESET);
	DelayUs(10);		
	ADS1299SendCommand(ADS1299_CMD_SDATAC);	
}

void ADS1299SendCommand(uint8_t command){
	//__disable_irq();
	
	ADS1299_CS_ENABLE;
	DelayUs(4);
	while(HAL_SPI_GetState(&hspi1)!= HAL_SPI_STATE_READY);
	HAL_SPI_Transmit(&hspi1, &command, sizeof(command),400);
	
	while(HAL_SPI_GetState(&hspi1)== HAL_SPI_STATE_BUSY);
	
	DelayUs(4);												
	ADS1299_CS_DISABLE;

	//__enable_irq();
	DelayUs(4);													
}

void ADS1299WriteREG (uint8_t address, uint8_t value){
	
	address += 0x40;			
	//__disable_irq();
	uint8_t DummyData = 0;
	
	ADS1299_CS_ENABLE;
	DelayUs(4);
	/* byte1 */
	while(HAL_SPI_GetState(&hspi1)!= HAL_SPI_STATE_READY);
	HAL_SPI_Transmit(&hspi1, &address, sizeof(address), 400);

	DelayUs(4);																						
	
	/* byte2 */
	while(HAL_SPI_GetState(&hspi1)!= HAL_SPI_STATE_READY);
	HAL_SPI_Transmit(&hspi1, &DummyData,sizeof(DummyData), 400);

	DelayUs(4);																							
	
	/* reg data */
	while(HAL_SPI_GetState(&hspi1)!= HAL_SPI_STATE_READY);
	HAL_SPI_Transmit(&hspi1, &value,sizeof(value), 400);
	DelayUs(4);																							

	while(HAL_SPI_GetState(&hspi1)== HAL_SPI_STATE_BUSY);
	
	DelayUs(5);																							// Delay time, final SCLK falling edge to CS high
	ADS1299_CS_DISABLE;
	//__enable_irq();
}

uint8_t ADS1299ReadREG (uint8_t address){
	address += 0x20;
	
	uint8_t DRchar;		
	uint8_t DummyData = 0;
	
	//__disable_irq();
	
	ADS1299_CS_ENABLE;
	
	DelayUs(5);
	while(HAL_SPI_GetState(&hspi1)!= HAL_SPI_STATE_READY);
	HAL_SPI_Transmit(&hspi1, &address,sizeof(address), 400);

	DelayUs(4);																								// 2us Command decode time
	
	while(HAL_SPI_GetState(&hspi1)!= HAL_SPI_STATE_READY);
	HAL_SPI_Transmit(&hspi1, &DummyData,sizeof(DummyData), 400);

	DelayUs(4);																								// 2us Command decode time
	
	while(HAL_SPI_GetState(&hspi1)!= HAL_SPI_STATE_READY);
	
	while(HAL_SPI_GetState(&hspi1)== HAL_SPI_STATE_BUSY);
	HAL_SPI_Receive(&hspi1, &DRchar, sizeof(DRchar), 400);
	

	DelayUs(5);																								// Delay time, final SCLK falling edge to CS high
	ADS1299_CS_DISABLE;
	
	//__enable_irq();

  return DRchar;
}

void ADS1299ReadByte(uint8_t *rp)
{
	unsigned char i=4;
	unsigned char tp[27]={0};	

	ADS1299_CS_ENABLE;
											
	while(i--);
	i = 4;

	HAL_SPI_TransmitReceive(&hspi1,tp,rp,27,100);
	while(i--);																								// 2us Command decode time
	//ADS1299_CS_DISABLE;

}


