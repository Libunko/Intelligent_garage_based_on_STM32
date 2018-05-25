/*******************************/
//��ֲҪע�� PD11��PE1 �ֱ�ΪRST�ͱ��� ��Ҫ��ʼ����
/*******************************/
#ifndef _LCD_H_
#define _LCD_H_
#include "stm32f4xx_hal.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define FSMC_LCD_CMD		((uint32_t)0x60000000)
#define FSMC_LCD_DATA		((uint32_t)0x60020000)	//A16	 

#define LCD_WRITE_CMD(x)	*(__IO uint16_t *)FSMC_LCD_CMD = x
#define LCD_WRITE_DATA(x)	*(__IO uint16_t *)FSMC_LCD_DATA = x
#define LCD_READ_DATA()		*(__IO uint16_t *)FSMC_LCD_DATA

	 
#define FSMC_LCD_BANKx		FSMC_NORSRAM_BANK1

/***************** ILI934 ��ʾ��ȫ��Ĭ�ϣ�ɨ�跽��Ϊ1ʱ������Ⱥ����߶� *********************/
#define LCD_Default_Max_Width		  240     //��ʼ���X����
#define LCD_Default_Max_Heigth		320     //��ʼ���Y����

#define 			ILI9341_LESS_PIXEL	  							240			//Һ�����϶̷�������ؿ��
#define 			ILI9341_MORE_PIXEL	 								320			//Һ�����ϳ���������ؿ��

/******************************* ���� ILI934 ��ʾ��������ɫ ********************************/
#define 	BACKGROUND		                BLACK   //Ĭ�ϱ�����ɫ

#define 		WHITE			0xFFFF	   //��ɫ
#define 		BLACK			0x0000	   //��ɫ 
#define 		GREY			0xF7DE	   //��ɫ 
#define 		BLUE			0x001F	   //��ɫ 
#define 		BLUE2			0x051F	   //ǳ��ɫ 
#define 		RED				0xF800	   //��ɫ 
#define 		MAGENTA		0xF81F	   //����ɫ�����ɫ 
#define		 	GREEN			0x07E0	   //��ɫ 
#define 		CYAN			0x7FFF	   //����ɫ����ɫ 
#define 		YELLOW		0xFFE0	   //��ɫ 
#define 		BRED			0xF81F
#define 		GRED			0xFFE0
#define 		GBLUE			0x07FF



#define LCD_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOB_CLK_ENABLE()
#define LCD_GPIO_PORT                      GPIOB   
#define LCD_BL_PIN                         GPIO_PIN_0

#define LCD_BL_ON()			HAL_GPIO_WritePin(LCD_GPIO_PORT,LCD_BL_PIN,GPIO_PIN_RESET);
#define LCD_BL_OFF()		HAL_GPIO_WritePin(LCD_GPIO_PORT,LCD_BL_PIN,GPIO_PIN_SET);

void LCD_Init ( void );
void LCD_GramScan ( uint8_t ucOption );
void LCD_OpenWindow(uint16_t usX,uint16_t usY,uint16_t usWidth,uint16_t usHeight);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_Clear(uint16_t usX,uint16_t usY,uint16_t usWidth,uint16_t usHeight,uint16_t usColor);
void LCD_SetPointPixel(uint16_t usX,uint16_t usY,uint16_t usColor);
uint16_t LCD_GetPointPixel ( uint16_t usX, uint16_t usY );
#endif

