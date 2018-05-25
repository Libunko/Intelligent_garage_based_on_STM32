#ifndef __MANAGER_H
#define __MANAGER_H


#include "stm32f4xx_hal.h"
#include "string.h"
#include "rc522_function.h"
#include "rc522_config.h"
#include "rtc.h"
#include "rc522_app.h"
#include <stdlib.h>
#include "motor.h"
#include "math.h"
#include "ov7725.h"
#include "GUI.h"
#include "sdio.h"
#include "ff.h"
#include "main.h"


/* ������� */

// LEFt��ת RIGHT��ת
#define X_LEFT_DIRECTION		GPIO_PIN_SET
#define X_RIGHT_DIRECTION		GPIO_PIN_RESET

// UP���ȥ DOWN�ջ���
#define Y_UP_DIRECTION			GPIO_PIN_RESET
#define Y_DOWN_DIRECTION		GPIO_PIN_SET

// UP���� DOWN�½�
#define Z_UP_DIRECTION			GPIO_PIN_RESET
#define Z_DOWN_DIRECTION		GPIO_PIN_SET

// UP˳ʱ�� DOWN��ʱ��
#define R_UP_DIRECTION			GPIO_PIN_RESET
#define R_DOWN_DIRECTION		GPIO_PIN_SET


void manager(void);
void change(struct id_struct *card);
void Print_Database(void);
void Get_Date_Time(void);
void Set_Date_Time(void);

void car1_park(void);
void car2_park(void);
void car3_park(void);
void car4_park(void);

void debug(void);

void car2_pick(void);

#endif



