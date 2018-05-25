#include "manager.h"

#define READ							1
#define WRITE							2
#define MAX_CAR_NUM				4
#define PARK_PICK_SECTOR	4
#define PARK_BLOCK				0
#define PICK_BLOCK				1


uint8_t date_time[16];				// 停取车时间
extern uint8_t KEY[6];				// 扇区密码

extern uint8_t motor_flag;		// 电机完成标志位
extern uint8_t channel;				// 定时器通道

extern int16_t x, y, z, r;

char temp_buf[100];
	
uint8_t rn_num;								// 当前分配的车位

struct id_struct card[MAX_CAR_NUM + 1];		// card[MAX_CAR_NUM] 作为临时结构体存储临时数据

void manager(void)
{
	uint8_t i;
	
	GUI_Clear();
	printf("\n");
	
	Print_Database();
	
	// 寻卡
	while (MI_ERR == Read_ID(&card[MAX_CAR_NUM]));
	
	// 验证密码
	if (MI_ERR == Check_PW(PARK_PICK_SECTOR, PARK_BLOCK, KEY, &card[MAX_CAR_NUM]))
		return;
	
	// 查询card中是否有该卡卡号，如有则是取车，无卡号是停车
	for (i=0; i<MAX_CAR_NUM; i++)
	{
		if (!memcmp(card[MAX_CAR_NUM].card_id, card[i].card_id, sizeof(card[MAX_CAR_NUM].card_id)))
		{
			// 已找到卡信号
			printf("***************取车**************\n");
			
			// 写入取车时间
			Get_Date_Time();
			if (MI_ERR == Write_Block(PARK_PICK_SECTOR, PICK_BLOCK, KEY, date_time, &card[i]))
			{
				// 删除卡号
				memset(&card[i], 0, sizeof(card[i]));
			}
			
			// 读取卡块数据到结构体并打印
//			Print_Sector(PARK_PICK_SECTOR, KEY, &card[i]);
			Read_Sector(PARK_PICK_SECTOR, KEY, &card[i]);
			
			// 删除卡块的车辆信息
			Write_Block(PARK_PICK_SECTOR, PICK_BLOCK, KEY, (uint8_t *)"", &card[i]);
			Write_Block(PARK_PICK_SECTOR, PARK_BLOCK, KEY, (uint8_t *)"", &card[i]);
				
			// 休眠卡
			PcdHalt();
			
			// 驱动电机
			printf("请等待提取车辆...\n");
			car2_pick();
//			switch (rn_num)
//			{
//				case 0: car1_pick();break;
//				case 1: car2_pick();break;
//				case 2: car3_pick();break;
//				case 3: car4_pick();break;
//			}

			// 拍照
			Take_pic(card[i].card_id, card[i].sector[PARK_PICK_SECTOR].block[PICK_BLOCK].data);
			
			// 显示停车收费
			change(&card[i]);
			
			// 删除后存入结构体
//			Print_Sector(PARK_PICK_SECTOR, KEY, &card[i]);
//			Read_Sector(PARK_PICK_SECTOR, KEY, &card[i]);

			// 删除卡号
			memset(&card[i], 0, sizeof(card[i]));
			
			// 等待电机到位
			while (motor_flag)
			{
				HAL_Delay(10);
			}
			
			// 提示取车成功
			printf("*************取车成功************\n\n\n");
			Print_Database();
			
			return;
		}
	}	
	// 未找到卡号信息，是来停车
	printf("***************停车**************\n");
	
	// 分配车位
	for (i=0; i<MAX_CAR_NUM; i++)
	{
		if (!memcmp(card[i].card_id, "", sizeof("")))
		{
			rn_num = i;
			printf("分配车位为: %d\n", rn_num + 1);
			break;
		}
	}
	if (i == MAX_CAR_NUM)
	{
		printf("车位已满\n");
		return;
	}
	
	// 复制临时结构体信息到当前车位结构体中
	strcpy((char *)card[rn_num].card_id, (char *)card[MAX_CAR_NUM].card_id);
	strcpy((char *)card[rn_num].ucArray_ID, (char *)card[MAX_CAR_NUM].ucArray_ID);
	
	printf("卡号: %s\n", card[rn_num].card_id);
	
	// 写入停车时间
	Get_Date_Time();
	if (MI_ERR == Write_Block(PARK_PICK_SECTOR, PARK_BLOCK, KEY, date_time, &card[rn_num]))
	{
		// 删除卡号
		memset(&card[rn_num], 0, sizeof(card[rn_num]));
		printf("入库失败，请重新刷卡....\n");
		return;
	}
	
	// 读取卡块数据到结构体
//	Print_Sector(PARK_PICK_SECTOR, KEY, &card[rn_num]);
	if (MI_ERR == Read_Sector(PARK_PICK_SECTOR, KEY, &card[rn_num]))
	{
		// 删除卡块的车辆信息
		Write_Block(PARK_PICK_SECTOR, PICK_BLOCK, KEY, (uint8_t *)"", &card[rn_num]);
		Write_Block(PARK_PICK_SECTOR, PARK_BLOCK, KEY, (uint8_t *)"", &card[rn_num]);
		// 删除卡号
		memset(&card[rn_num], 0, sizeof(card[rn_num]));
		printf("入库失败，请重新刷卡....\n");
		return;
	}
	// 休眠卡
	PcdHalt();
		
	// 拍照
	Take_pic(card[rn_num].card_id, card[i].sector[PARK_PICK_SECTOR].block[PARK_BLOCK].data);
	
	// 驱动电机
	printf("请等待存放车辆...\n");
	car2_park();
//	switch (rn_num)
//	{
//		case 0: car1_park();break;
//		case 1: car2_park();break;
//		case 2: car3_park();break;
//		case 3: car4_park();break;
//	}

	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
		
	// 提示停车成功
	printf("*************停车成功************\n\n\n");
	Print_Database();
	
	return;
}

// 收费
void change(struct id_struct *card)
{
	uint8_t i;
	char date_park[9] = {0};		// 8个数据，数组长度为9，因为最后一位存储'\0'
	char date_pick[9] = {0};
	char time_park[7] = {0};
	char time_pick[7] = {0};
	
	uint32_t park_time;
	uint32_t pick_time;
	uint32_t park_date;
	uint32_t pick_date;
	
	uint8_t park_temp[6];		// 不可能停车超过127年
	uint8_t pick_temp[6];
	uint8_t value[6];		// 不可能停车超过127年
	
	// 将结构体中的停车时间截取截取成两段	
	for (i=0; i<8; i++)
	{
		date_park[i] = card->sector[PARK_PICK_SECTOR].block[PARK_BLOCK].data[i];
		date_pick[i] = card->sector[PARK_PICK_SECTOR].block[PICK_BLOCK].data[i];
	}
	
	for (i=0; i<6; i++)
	{
		// 第8字符是空字符
		time_park[i] = card->sector[PARK_PICK_SECTOR].block[PARK_BLOCK].data[i + 9];
		time_pick[i] = card->sector[PARK_PICK_SECTOR].block[PICK_BLOCK].data[i + 9];
	}
	
	// 将字符串转为数字
	park_date = atoll(date_park);
	park_time = atoll(time_park);
	pick_date = atoll(date_pick);
	pick_time = atoll(time_pick);
	
	printf("停车时间: %08d ", park_date);
	printf("%06d\n", park_time);
	printf("取车时间: %08d ", pick_date);
	printf("%06d\n", pick_time);
	
	GUI_Clear();
	
	GUI_DispNextLine();
	GUI_DispNextLine();
	sprintf(temp_buf, "     Card ID   :     %s", card->card_id);
	GUI_DispString(temp_buf);
	sprintf(temp_buf, "     Park Time :  %08d %06d", park_date, park_time);
	GUI_DispNextLine();
	GUI_DispNextLine();
	GUI_DispNextLine();
	GUI_DispNextLine();
	GUI_DispString(temp_buf);
	sprintf(temp_buf, "     Pick Time :  %08d %06d", pick_date, pick_time);
	GUI_DispNextLine();
	GUI_DispNextLine();
	GUI_DispNextLine();
	GUI_DispString(temp_buf);
	
	// 计算两个时间差值
	for (i=0; i<3; i++)
	{
		park_temp[i] = park_time / (uint16_t)pow(10, (i*2)) % 100;
		park_temp[i+3] = park_date / (uint16_t)pow(10, (i*2)) % 100;
		
		pick_temp[i] = pick_time / (uint16_t)pow(10, (i*2)) % 100;
		pick_temp[i+3] = pick_date / (uint16_t)pow(10, (i*2)) % 100;
	}
	for (i=0; i<6; i++)
	{
		if (pick_temp[i] == park_temp[i])
		{
			value[i] = 0;
		}
		else if (pick_temp[i] > park_temp[i])
		{
			value[i] = pick_temp[i] - park_temp[i];
		}
		else if (pick_temp[i] < park_temp[i])
		{
			switch (i)
			{
				case 0 : value[i] = 60 - park_temp[i] + pick_temp[i]; park_temp[i + 1]++; break;
				case 1 : value[i] = 60 - park_temp[i] + pick_temp[i]; park_temp[i + 1]++; break;
				case 2 : value[i] = 24 - park_temp[i] + pick_temp[i]; park_temp[i + 1]++; break;
				case 3 : value[i] = 31 - park_temp[i] + pick_temp[i]; pick_temp[i + 1]--; break;
				case 4 : value[i] = 12 - park_temp[i] + pick_temp[i]; pick_temp[i + 1]--; break;
				case 5 : break;
			}
		}
	}
	
	// 串口打印和TFT屏显示
	printf("*********************************\n");
	for (i=5; i>0; i--)
	{
		if (value[i] != 0)
			break;
	}
	printf("       共停车  ");
	memset(temp_buf, 0, sizeof(temp_buf));
	GUI_DispNextLine();
	GUI_DispNextLine();
	GUI_DispNextLine();
	GUI_DispNextLine();
	GUI_DispString("     Length of Parking :  ");
	// switch 作用只显示有效时间
	switch (i)
	{
		case 5: printf("%d年", value[i]); sprintf(temp_buf, "%d year ", value[i]); GUI_DispString(temp_buf);
		case 4: printf("%d月", value[i]); sprintf(temp_buf, "%d month ", value[i]);GUI_DispString(temp_buf);
		case 3: printf("%d天", value[i]); sprintf(temp_buf, "%d day ", value[i]);GUI_DispString(temp_buf);
		case 2: printf("%d小时", value[i]); sprintf(temp_buf, "%d hour ", value[i]); GUI_DispString(temp_buf);
		case 1: printf("%d分钟", value[i]); sprintf(temp_buf, "%d min ", value[i]); GUI_DispString(temp_buf);
		case 0: printf("%d秒", value[i]); sprintf(temp_buf, "%d sec", value[i]); GUI_DispString(temp_buf);
	}
	printf("\n");
	printf("*********************************\n");
	
//	// 弹出SD卡，防止拔出SD卡出错
//	HAL_SD_MspDeInit(&hsd);
	
	// 有足够时间观察停车时间
	HAL_Delay(10000);
}

	// 打印车库存存信息
void Print_Database(void)
{
	uint8_t i;
	
	
	GUI_Clear();
	
	GUI_GotoXY(0, 0);
	GUI_DispNextLine();
	
	GUI_SetColor(GUI_YELLOW);
	
	GUI_DispString("      Garage Inventory Information");
	GUI_DispNextLine();
	GUI_DispNextLine();
	GUI_DispNextLine();
	GUI_DispString("           ID           Storage Time");
	GUI_DispNextLine();
	
	
	printf("***********车库库存信息**********\n\n");
	printf("         卡号        入库时间\n");
	GUI_SetColor(GUI_RED);
	for (i=0; i<MAX_CAR_NUM; i++)
	{
		sprintf(temp_buf, "No %d:   %s   |   %s", i+1, (char *)card[i].card_id, (char *)card[i].sector[PARK_PICK_SECTOR].block[PARK_BLOCK].data);
		GUI_DispNextLine();
		GUI_DispNextLine();
		GUI_DispString(temp_buf);
		printf("%d车位: %s | %s\n", i +1 , (char *)card[i].card_id, (char *)card[i].sector[PARK_PICK_SECTOR].block[PARK_BLOCK].data);
	}
	return;
}
// 设置日期和时间
void Set_Date_Time()
{
	RTC_DateTypeDef RTC_DateTypeStructure;
	RTC_TimeTypeDef RTC_TimeTypeStructure;
	
	RTC_DateTypeStructure.Year = 0;
	RTC_DateTypeStructure.Month = 5;
	RTC_DateTypeStructure.Date = 12;
	
	RTC_TimeTypeStructure.Hours = 10;
	RTC_TimeTypeStructure.Minutes = 23;
	RTC_TimeTypeStructure.Seconds = 20;
	
	HAL_RTC_SetDate(&hrtc, &RTC_DateTypeStructure, RTC_FORMAT_BIN);
	HAL_RTC_SetTime(&hrtc, &RTC_TimeTypeStructure, RTC_FORMAT_BIN);
	
	printf("已完成时间设置\n");	
	Get_Date_Time();
}

// 获取日期和时间
void Get_Date_Time(void)
{
	RTC_DateTypeDef RTC_DateTypeStructure;
	RTC_TimeTypeDef RTC_TimeTypeStructure;
	
	HAL_RTC_GetDate(&hrtc, &RTC_DateTypeStructure, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&hrtc, &RTC_TimeTypeStructure, RTC_FORMAT_BIN);
	
	sprintf((char *)date_time, "%04d%02d%02d %02d%02d%02d", RTC_DateTypeStructure.Year + 2018 - 3, RTC_DateTypeStructure.Month, \
							RTC_DateTypeStructure.Date, RTC_TimeTypeStructure.Hours, \
							RTC_TimeTypeStructure.Minutes, RTC_TimeTypeStructure.Seconds
	);
	
	printf("现在时间: %s\n", date_time);
}

void car1_park(void)
{
	// Z 上升
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*78);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y 收回
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*92);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 左边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*12);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// 分度盘 顺时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_X_Pin, R_UP_DIRECTION);
	pulse(15*93);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	
	// z 上升
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*177);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
/**********************************/
	// 1号车库
	// r 逆时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
	pulse(15*(89+4));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 右边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*(12-1));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y 伸出
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(91+3+1));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z 下降
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*137);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y 收回
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*(91+3));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 左边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*12);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// r 顺时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_UP_DIRECTION);
	pulse(15*(89+4));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
/*********************************/
	// 通用回
	// r 逆时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
	pulse(15*(89+4));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 右边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*12);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y 伸出
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(92));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z 下降
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*(118+2));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
}

void car2_park(void)
{
	// Z 上升
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*70);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y 收回
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*92);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 左边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*12);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// 分度盘 顺时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_X_Pin, R_UP_DIRECTION);
	pulse(15*93);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z 上升
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*160);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
/**********************************/
// 2号车库
	// r 逆时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
	pulse(15*(94));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 左边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*140);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y 伸出
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(95-3-1));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z 下降
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*(160-30));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y 收回
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*(92));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 右边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*140);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// r 顺时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_UP_DIRECTION);
	pulse(15*(1));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 右边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*12);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y 伸出
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(92));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z 下降
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*(70+30));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
}

void car3_park(void)
{
	// Z 上升
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*78);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y 收回
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*93);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 左边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*12);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// 分度盘 顺时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_X_Pin, R_UP_DIRECTION);
	pulse(15*93);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z 上升
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*177);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
/**********************************/
// 3号车库
	// r 顺时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_UP_DIRECTION);
	pulse(15*(87));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 左边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*(113+3+4+3-2-1));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y 伸出
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(95));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z 下降
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*137);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y 收回
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*(95));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 右边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*(113+3+6+3-2-1));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// r 逆时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
	pulse(15*(87));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
/*********************************/
	// 通用回
	// r 逆时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
	pulse(15*(89+4));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 右边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*12);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y 伸出
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(93));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z 下降
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*(118+2));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
}

void car4_park(void)
{
	// Z 上升
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*78);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y 收回
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*93);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 左边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*12);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// 分度盘 顺时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_X_Pin, R_UP_DIRECTION);
	pulse(15*93);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z 上升
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*177);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
/**********************************/
// 4号车库
	// r 顺时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_UP_DIRECTION);
	pulse(15*(86+1-1));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 右边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
//	pulse(15*(27+1-3-1));
	pulse(15*(24-3));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y 伸出
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(95-2));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z 下降
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*137);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y 收回
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*(95-2));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 左边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
//	pulse(15*(27+1-3-1));
	pulse(15*(24-3));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// r 逆时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
	pulse(15*(86+1-1));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
/*********************************/
	// 通用回
	// r 逆时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
	pulse(15*(89+4));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 右边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*(12-6+2));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y 伸出
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(93-1));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z 下降
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*(118+2));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
}

void debug(void)
{
// z 下降
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*137);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y 收回
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*(95-3+1));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 右边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*140);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
//	// r 顺时针
//	motor_flag = 1;		// 标志位置1表示未完成
//	channel = CHANNEL_R;
//	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_UP_DIRECTION);
//	pulse(15*(94));
//	// 等待电机到位
//	while (motor_flag)
//	{
//		HAL_Delay(10);
//	}
//	
//	// r 逆时针
//	motor_flag = 1;		// 标志位置1表示未完成
//	channel = CHANNEL_R;
//	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
//	pulse(15*(89+4));
//	// 等待电机到位
//	while (motor_flag)
//	{
//		HAL_Delay(10);
//	}

//	// r 顺时针
//	motor_flag = 1;		// 标志位置1表示未完成
//	channel = CHANNEL_R;
//	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_UP_DIRECTION);
//	pulse(15*(1));
//	// 等待电机到位
//	while (motor_flag)
//	{
//		HAL_Delay(10);
//	}
	// x 右边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*12);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y 伸出
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(92));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z 下降
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*(118+2));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
}

void car2_pick(void)
{
	// Z 上升
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*70);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y 收回
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*92);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 左边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*12);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 左边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*140);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y 伸出
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(95-3-1));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z 上升
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*160);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y 收回
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*(92));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 右边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*140);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// 分度盘 顺时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_X_Pin, R_UP_DIRECTION);
	pulse(15*93);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z 下降
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*160);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// r 逆时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
	pulse(15*(94));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}	
	// r 顺时针
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_UP_DIRECTION);
	pulse(15*(1));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x 右边
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*12);
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y 伸出
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(92));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z 下降
	motor_flag = 1;		// 标志位置1表示未完成
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*(70));
	// 等待电机到位
	while (motor_flag)
	{
		HAL_Delay(10);
	}
}
