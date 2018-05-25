#ifndef __MOTOR_H
#define __MOTOR_H


#include "stm32f4xx_hal.h"
#include "tim.h"


/* PWM Í¨µÀ */
#define CHANNEL_X				TIM_CHANNEL_1
#define CHANNEL_Y				TIM_CHANNEL_2
#define CHANNEL_Z				TIM_CHANNEL_3
#define CHANNEL_R				TIM_CHANNEL_4



void pulse(unsigned int degrees);




#endif
