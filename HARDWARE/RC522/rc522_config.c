/**
  ******************************************************************************
  * �ļ�����: main.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2015-10-04
  * ��    ��: RC522��������
  ******************************************************************************
  * ˵����
  * ����������Ӳʯstm32������YS-F1Proʹ�á�
  * 
  * �Ա���
  * ��̳��http://www.ing10bbs.com
  * ��Ȩ��ӲʯǶ��ʽ�����Ŷ����У��������á�
  ******************************************************************************
  */  
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "rc522_config.h"
#include "stm32f4xx_hal.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
SPI_HandleTypeDef hspiflash;
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

/**
  * ��������: ����FLASH��ʼ��
  * �������: huart�����ھ������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
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
//    /* SPI����ʱ��ʹ�� */
//    macRC522_SPIx_RCC_CLK_ENABLE();
//    /* GPIO����ʱ��ʹ�� */
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

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/


