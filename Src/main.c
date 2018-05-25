/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "crc.h"
#include "fatfs.h"
#include "rtc.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* USER CODE BEGIN Includes */
#include "GUI.h"
#include "string.h"
#include "sccb.h"
#include "ov7725.h"
#include "ILI93xx.h"
#include "touch.h"
#include "spi_flash.h"
#include "debug.h"
//#include "GUIDemo.h"

/* libunko add */
#include "string.h"
#include "rc522_function.h"
#include "rc522_config.h"
#include "manager.h"
#include "motor.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_NVIC_Init(void);

extern uint8_t motor_flag;		// 电机完成标志位
extern uint8_t channel;				// 定时器通道

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/*摄像头配置结构体*/
extern OV7725_MODE_PARAM cam_mode;  

void Mytouch_MainTask(void);
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
	/*文件系统*/
	FATFS fs;
	int res = -1;
	
	/*falsh id*/
	uint32_t FLASH_ID = 0;
	
	/* debug add */
	int16_t x = 0, y = 0, z = 0, r = 0;
	
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  MX_USART1_UART_Init();
  MX_FSMC_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_RTC_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_SPI2_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();

  /* USER CODE BEGIN 2 */
	printf("系统正在初始化...\r\n");
	GUI_Init();
	GUI_Clear();
	GUI_SetFont(&GUI_Font8x12_ASCII);
	GUI_DispString("The system is initializing ...");
	
	/* RFID Init */
	MX_SPIFlash_Init();
	PcdReset();
	M500PcdConfigISOType ( 'A' );// 设置工作方式
	
//	Set_Date_Time();

	/*挂载sd文件系统*/
	res = f_mount(&fs,"0:",1);
	if(res != FR_OK)
	{
		printf("请给开发板插入已格式化成fat格式的SD卡 !\r\n\n\n");
	}

	OV7725_Start();
	
	
	//这里拉高f_cs 原因是避免因为在CUBemx中没设置默认输出高 导致芯片在上电的时候就被选中 而不能正常通信
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 ,GPIO_PIN_SET);
	/*读取flash ID */
	FLASH_ID = SPI_FLASH_ReadID();
	printf("FLASH_ID is 0x%x\r\n", FLASH_ID);
	
	/*初始化触屏*/
	tp_dev.init();
	GUI_SetColor(GUI_RED);
	GUI_DrawRect(0, 0, 320-1, 240-1);
	GUI_DrawLine(0, 120, 320, 120);
	GUI_DrawLine(80, 0, 80, 240);
	GUI_DrawLine(160, 0, 160, 240);
	GUI_DrawLine(240, 0, 240, 240);
//	while(1)
//	{
//		TP_Scan(0);
//	}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		while(1)
			manager();
//		car2_park();
//		debug();			
		
		
	while(1)
	{
		
		while (!TP_Scan(0));
		GUI_SetColor(GUI_YELLOW);
		GUI_DispStringAt("x:",1,16);GUI_DispDec(tp_dev.x[0],3);
		GUI_DispStringAt("y:",1,32);GUI_DispDec(tp_dev.y[0],3);
		GUI_SetColor(GUI_WHITE);
		motor_flag = 1;		// 标志位置1表示未完成
		
		// x轴
		if (tp_dev.x[0] < 80 && tp_dev.y[0] < 120)
		{
			x++;
			channel = CHANNEL_X;
			HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
		}
		else if (tp_dev.x[0] < 80 && tp_dev.y[0] > 120)
		{
			x--;
			channel = CHANNEL_X;
			HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
		}
		// y轴
		else if (tp_dev.x[0] > 80 && tp_dev.x[0] < 160 && tp_dev.y[0] < 120)
		{
			y++;
			channel = CHANNEL_Y;
			HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
		}
		else if (tp_dev.x[0] > 80 && tp_dev.x[0] < 160 && tp_dev.y[0] > 120)
		{
			y--;
			channel = CHANNEL_Y;
			HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
		}
		// z轴
		else if (tp_dev.x[0] > 160 && tp_dev.x[0] < 240 && tp_dev.y[0] < 120)
		{
			z++;
			channel = CHANNEL_Z;
			HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
		}
		else if (tp_dev.x[0] > 160 && tp_dev.x[0] < 240 && tp_dev.y[0] > 120)
		{
			z--;
			channel = CHANNEL_Z;
			HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
		}
		// 分度盘
		else if (tp_dev.x[0] > 240 && tp_dev.x[0] < 320 && tp_dev.y[0] < 120)
		{
			r++;
			channel = CHANNEL_R;
			HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_UP_DIRECTION);
		}
		else if (tp_dev.x[0] > 240 && tp_dev.x[0] < 320 && tp_dev.y[0] > 120)
		{
			r--;
			channel = CHANNEL_R;
			HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
		}
		
		pulse(15);
		// 等待电机到位
		
		// 等待电机到位
		while (motor_flag)
		{
			HAL_Delay(10);
		}
		GUI_SetColor(GUI_GREEN);
		GUI_DispStringAt("x:", 1, 16*4);GUI_DispDec(x, 4);
		GUI_DispStringAt("y:", 1, 16*5);GUI_DispDec(y, 4);
		GUI_DispStringAt("z:", 1, 16*6);GUI_DispDec(z, 4);
		GUI_DispStringAt("r:", 1, 16*7);GUI_DispDec(r, 4);
		GUI_SetColor(GUI_WHITE);
		
		}
		
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/** NVIC Configuration
*/
static void MX_NVIC_Init(void)
{
  /* TIM2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM2_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

/* USER CODE BEGIN 4 */
void Mytouch_MainTask(void)
{
GUI_PID_STATE TouchState;
int xPhys;
int yPhys;
GUI_Init();
GUI_SetFont(&GUI_Font20_ASCII);
GUI_CURSOR_Show();
GUI_CURSOR_Select(&GUI_CursorCrossL);
GUI_SetBkColor(GUI_WHITE);
GUI_SetColor(GUI_BLACK);
GUI_Clear();
GUI_DispString("Measurement of\nA/D converter values");
while (1)
{
GUI_TOUCH_GetState(&TouchState); // Get the touch position in pixel
//xPhys = GUI_TOUCH_GetxPhys(); // Get the A/D mesurement result in x
//yPhys = GUI_TOUCH_GetyPhys(); // Get the A/D mesurement result in y
xPhys = GUI_TOUCH_X_MeasureX(); // Get the A/D mesurement result in x
yPhys = GUI_TOUCH_X_MeasureY(); // Get the A/D mesurement result in y	
GUI_SetColor(GUI_BLUE);
GUI_DispStringAt("Analog input:\n", 0, 40);
GUI_GotoY(GUI_GetDispPosY() + 2);
	GUI_DispString("x:");
GUI_DispDec(xPhys, 4);
GUI_DispString(", y:");
GUI_DispDec(yPhys, 4);
GUI_SetColor(GUI_RED);
GUI_GotoY(GUI_GetDispPosY() + 4);
GUI_DispString("\nPosition:\n");
GUI_GotoY(GUI_GetDispPosY() + 2);
GUI_DispString("x:");
GUI_DispDec(TouchState.x,4);
GUI_DispString(", y:");
GUI_DispDec(TouchState.y,4);
HAL_Delay(50);
};
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
