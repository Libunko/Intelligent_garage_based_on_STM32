#ifndef __SCCB_H
#define __SCCB_H
#include "stm32f4xx_hal.h"

#define u8 uint8_t 
#define u16 uint16_t


/************************** OV7725 �������Ŷ���********************************/
#define I2C_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C_GPIO_PORT                       GPIOB   
#define I2C_SCL_PIN                         GPIO_PIN_8
#define I2C_SDA_PIN                         GPIO_PIN_9

#define SCL_H						                    HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SCL_PIN,GPIO_PIN_SET)    // ����ߵ�ƽ
#define SCL_L                       				HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SCL_PIN,GPIO_PIN_RESET)  // ����͵�ƽ
#define SDA_H                      					HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SDA_PIN,GPIO_PIN_SET)    // ����ߵ�ƽ
#define SDA_L                       				HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SDA_PIN,GPIO_PIN_RESET)  // ����͵�ƽ
#define SDA_read                      			HAL_GPIO_ReadPin(I2C_GPIO_PORT,I2C_SDA_PIN)

#define ADDR_OV7725   0x42



void SCCB_GPIO_Config(void);
int SCCB_WriteByte( u16 WriteAddress , u8 SendByte);
int SCCB_ReadByte(u8* pBuffer,   u16 length,   u8 ReadAddress);



#endif 

