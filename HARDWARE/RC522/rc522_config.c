/**
  ******************************************************************************
  * 文件名程: main.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: RC522引脚配置
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F1Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */  
/* 包含头文件 ----------------------------------------------------------------*/
#include "rc522_config.h"
#include "stm32f4xx_hal.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
SPI_HandleTypeDef hspiflash;
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 串行FLASH初始化
  * 输入参数: huart：串口句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
*/
void MX_SPIFlash_Init(void)
{
  hspiflash.Instance = macRC522_SPIx;
  hspiflash.Init.Mode = SPI_MODE_MASTER;
  hspiflash.Init.Direction = SPI_DIRECTION_2LINES;
  hspiflash.Init.DataSize = SPI_DATASIZE_8BIT;
  hspiflash.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspiflash.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspiflash.Init.NSS = SPI_NSS_SOFT;
  hspiflash.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspiflash.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspiflash.Init.TIMode = SPI_TIMODE_DISABLE;
  hspiflash.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspiflash.Init.CRCPolynomial = 7;
  HAL_SPI_Init(&hspiflash);

  macRC522_Reset_Disable();
	macRC522_CS_Disable();

}

//void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
//{
//  GPIO_InitTypeDef GPIO_InitStruct;
//  if(hspi->Instance==macRC522_SPIx)
//  {
//    /* SPI外设时钟使能 */
//    macRC522_SPIx_RCC_CLK_ENABLE();
//    /* GPIO外设时钟使能 */
//    macRC522_SPI_RCC_CLK_ENABLE();
//    macRC522_RST_RCC_CLK_ENABLE();   

//    GPIO_InitStruct.Pin = macRC522_GPIO_CS_PIN;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    GPIO_InitStruct.Mode = macRC522_GPIO_CS_Mode;
//    HAL_GPIO_Init(macRC522_GPIO_CS_PORT, &GPIO_InitStruct);
//    
//    GPIO_InitStruct.Pin = macRC522_GPIO_SCK_PIN;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    GPIO_InitStruct.Mode = macRC522_GPIO_SCK_Mode;
//    HAL_GPIO_Init(macRC522_GPIO_SCK_PORT, &GPIO_InitStruct);

//    GPIO_InitStruct.Pin = macRC522_GPIO_MOSI_PIN;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    GPIO_InitStruct.Mode = macRC522_GPIO_MOSI_Mode;
//    HAL_GPIO_Init(macRC522_GPIO_MOSI_PORT, &GPIO_InitStruct);

//    GPIO_InitStruct.Pin = macRC522_GPIO_MISO_PIN;
//    GPIO_InitStruct.Mode = macRC522_GPIO_MISO_Mode;
//    HAL_GPIO_Init(macRC522_GPIO_MISO_PORT, &GPIO_InitStruct);	
//    
//    GPIO_InitStruct.Pin = macRC522_GPIO_RST_PIN;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    GPIO_InitStruct.Mode = macRC522_GPIO_RST_Mode;
//    HAL_GPIO_Init(macRC522_GPIO_RST_PORT, &GPIO_InitStruct);
//  }
//}

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/


