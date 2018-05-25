/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define FIFO_WRST_Pin GPIO_PIN_2
#define FIFO_WRST_GPIO_Port GPIOE
#define FIFO_RRST_Pin GPIO_PIN_3
#define FIFO_RRST_GPIO_Port GPIOE
#define FIFO_RRSTE4_Pin GPIO_PIN_4
#define FIFO_RRSTE4_GPIO_Port GPIOE
#define VSYNC_Pin GPIO_PIN_5
#define VSYNC_GPIO_Port GPIOE
#define TP_IRQ_Pin GPIO_PIN_13
#define TP_IRQ_GPIO_Port GPIOC
#define Camra_D0_Pin GPIO_PIN_0
#define Camra_D0_GPIO_Port GPIOC
#define Camra_D1_Pin GPIO_PIN_1
#define Camra_D1_GPIO_Port GPIOC
#define Camra_D2_Pin GPIO_PIN_2
#define Camra_D2_GPIO_Port GPIOC
#define Camra_D3_Pin GPIO_PIN_3
#define Camra_D3_GPIO_Port GPIOC
#define Direction_X_Pin GPIO_PIN_4
#define Direction_X_GPIO_Port GPIOA
#define Direction_Y_Pin GPIO_PIN_6
#define Direction_Y_GPIO_Port GPIOA
#define Direction_Z_Pin GPIO_PIN_7
#define Direction_Z_GPIO_Port GPIOA
#define Camra_D4_Pin GPIO_PIN_4
#define Camra_D4_GPIO_Port GPIOC
#define Camra_D5_Pin GPIO_PIN_5
#define Camra_D5_GPIO_Port GPIOC
#define FLASH_CS_Pin GPIO_PIN_0
#define FLASH_CS_GPIO_Port GPIOB
#define LCD_BL_Pin GPIO_PIN_1
#define LCD_BL_GPIO_Port GPIOB
#define TP_CS_Pin GPIO_PIN_12
#define TP_CS_GPIO_Port GPIOB
#define Camra_D6_Pin GPIO_PIN_6
#define Camra_D6_GPIO_Port GPIOC
#define Camra_D7_Pin GPIO_PIN_7
#define Camra_D7_GPIO_Port GPIOC
#define Direction_R_Pin GPIO_PIN_15
#define Direction_R_GPIO_Port GPIOA
#define SCCB_SCK_Pin GPIO_PIN_8
#define SCCB_SCK_GPIO_Port GPIOB
#define SCCB_SDA_Pin GPIO_PIN_9
#define SCCB_SDA_GPIO_Port GPIOB
#define FIFO_OE_Pin GPIO_PIN_0
#define FIFO_OE_GPIO_Port GPIOE
#define FIFO_WE_Pin GPIO_PIN_1
#define FIFO_WE_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
