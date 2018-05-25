#include "motor.h"

#define microstep	16	//固定细分


uint8_t motor_flag;

/* 调用pluse之前先赋值通道 */
uint8_t channel;

unsigned int times;
unsigned int pulse_times;

void pulse(unsigned int degrees)
{
	pulse_times = degrees / 1.8 * microstep;
	HAL_TIM_PWM_Start_IT(&htim2, channel);
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (++times == pulse_times)
	{
		times = 0;
		motor_flag = 0;	// 标志位置0表示完成
		HAL_TIM_PWM_Stop_IT(&htim2, channel);
	}
}


