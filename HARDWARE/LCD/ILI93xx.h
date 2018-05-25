/*******************************/
//移植要注意 PD11和PE1 分别为RST和背光 是要初始化的
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

/***************** ILI934 显示屏全屏默认（扫描方向为1时）最大宽度和最大高度 *********************/
#define LCD_Default_Max_Width		  240     //起始点的X坐标
#define LCD_Default_Max_Heigth		320     //起始点的Y坐标

#define 			ILI9341_LESS_PIXEL	  							240			//液晶屏较短方向的像素宽度
#define 			ILI9341_MORE_PIXEL	 								320			//液晶屏较长方向的像素宽度

/******************************* 定义 ILI934 显示屏常用颜色 ********************************/
#define 	BACKGROUND		                BLACK   //默认背景颜色

#define 		WHITE			0xFFFF	   //白色
#define 		BLACK			0x0000	   //黑色 
#define 		GREY			0xF7DE	   //灰色 
#define 		BLUE			0x001F	   //蓝色 
#define 		BLUE2			0x051F	   //浅蓝色 
#define 		RED				0xF800	   //红色 
#define 		MAGENTA		0xF81F	   //红紫色，洋红色 
#define		 	GREEN			0x07E0	   //绿色 
#define 		CYAN			0x7FFF	   //蓝绿色，青色 
#define 		YELLOW		0xFFE0	   //黄色 
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

