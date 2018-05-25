#include "manager.h"

#define READ							1
#define WRITE							2
#define MAX_CAR_NUM				4
#define PARK_PICK_SECTOR	4
#define PARK_BLOCK				0
#define PICK_BLOCK				1


uint8_t date_time[16];				// ͣȡ��ʱ��
extern uint8_t KEY[6];				// ��������

extern uint8_t motor_flag;		// �����ɱ�־λ
extern uint8_t channel;				// ��ʱ��ͨ��

extern int16_t x, y, z, r;

char temp_buf[100];
	
uint8_t rn_num;								// ��ǰ����ĳ�λ

struct id_struct card[MAX_CAR_NUM + 1];		// card[MAX_CAR_NUM] ��Ϊ��ʱ�ṹ��洢��ʱ����

void manager(void)
{
	uint8_t i;
	
	GUI_Clear();
	printf("\n");
	
	Print_Database();
	
	// Ѱ��
	while (MI_ERR == Read_ID(&card[MAX_CAR_NUM]));
	
	// ��֤����
	if (MI_ERR == Check_PW(PARK_PICK_SECTOR, PARK_BLOCK, KEY, &card[MAX_CAR_NUM]))
		return;
	
	// ��ѯcard���Ƿ��иÿ����ţ���������ȡ�����޿�����ͣ��
	for (i=0; i<MAX_CAR_NUM; i++)
	{
		if (!memcmp(card[MAX_CAR_NUM].card_id, card[i].card_id, sizeof(card[MAX_CAR_NUM].card_id)))
		{
			// ���ҵ����ź�
			printf("***************ȡ��**************\n");
			
			// д��ȡ��ʱ��
			Get_Date_Time();
			if (MI_ERR == Write_Block(PARK_PICK_SECTOR, PICK_BLOCK, KEY, date_time, &card[i]))
			{
				// ɾ������
				memset(&card[i], 0, sizeof(card[i]));
			}
			
			// ��ȡ�������ݵ��ṹ�岢��ӡ
//			Print_Sector(PARK_PICK_SECTOR, KEY, &card[i]);
			Read_Sector(PARK_PICK_SECTOR, KEY, &card[i]);
			
			// ɾ������ĳ�����Ϣ
			Write_Block(PARK_PICK_SECTOR, PICK_BLOCK, KEY, (uint8_t *)"", &card[i]);
			Write_Block(PARK_PICK_SECTOR, PARK_BLOCK, KEY, (uint8_t *)"", &card[i]);
				
			// ���߿�
			PcdHalt();
			
			// �������
			printf("��ȴ���ȡ����...\n");
			car2_pick();
//			switch (rn_num)
//			{
//				case 0: car1_pick();break;
//				case 1: car2_pick();break;
//				case 2: car3_pick();break;
//				case 3: car4_pick();break;
//			}

			// ����
			Take_pic(card[i].card_id, card[i].sector[PARK_PICK_SECTOR].block[PICK_BLOCK].data);
			
			// ��ʾͣ���շ�
			change(&card[i]);
			
			// ɾ�������ṹ��
//			Print_Sector(PARK_PICK_SECTOR, KEY, &card[i]);
//			Read_Sector(PARK_PICK_SECTOR, KEY, &card[i]);

			// ɾ������
			memset(&card[i], 0, sizeof(card[i]));
			
			// �ȴ������λ
			while (motor_flag)
			{
				HAL_Delay(10);
			}
			
			// ��ʾȡ���ɹ�
			printf("*************ȡ���ɹ�************\n\n\n");
			Print_Database();
			
			return;
		}
	}	
	// δ�ҵ�������Ϣ������ͣ��
	printf("***************ͣ��**************\n");
	
	// ���䳵λ
	for (i=0; i<MAX_CAR_NUM; i++)
	{
		if (!memcmp(card[i].card_id, "", sizeof("")))
		{
			rn_num = i;
			printf("���䳵λΪ: %d\n", rn_num + 1);
			break;
		}
	}
	if (i == MAX_CAR_NUM)
	{
		printf("��λ����\n");
		return;
	}
	
	// ������ʱ�ṹ����Ϣ����ǰ��λ�ṹ����
	strcpy((char *)card[rn_num].card_id, (char *)card[MAX_CAR_NUM].card_id);
	strcpy((char *)card[rn_num].ucArray_ID, (char *)card[MAX_CAR_NUM].ucArray_ID);
	
	printf("����: %s\n", card[rn_num].card_id);
	
	// д��ͣ��ʱ��
	Get_Date_Time();
	if (MI_ERR == Write_Block(PARK_PICK_SECTOR, PARK_BLOCK, KEY, date_time, &card[rn_num]))
	{
		// ɾ������
		memset(&card[rn_num], 0, sizeof(card[rn_num]));
		printf("���ʧ�ܣ�������ˢ��....\n");
		return;
	}
	
	// ��ȡ�������ݵ��ṹ��
//	Print_Sector(PARK_PICK_SECTOR, KEY, &card[rn_num]);
	if (MI_ERR == Read_Sector(PARK_PICK_SECTOR, KEY, &card[rn_num]))
	{
		// ɾ������ĳ�����Ϣ
		Write_Block(PARK_PICK_SECTOR, PICK_BLOCK, KEY, (uint8_t *)"", &card[rn_num]);
		Write_Block(PARK_PICK_SECTOR, PARK_BLOCK, KEY, (uint8_t *)"", &card[rn_num]);
		// ɾ������
		memset(&card[rn_num], 0, sizeof(card[rn_num]));
		printf("���ʧ�ܣ�������ˢ��....\n");
		return;
	}
	// ���߿�
	PcdHalt();
		
	// ����
	Take_pic(card[rn_num].card_id, card[i].sector[PARK_PICK_SECTOR].block[PARK_BLOCK].data);
	
	// �������
	printf("��ȴ���ų���...\n");
	car2_park();
//	switch (rn_num)
//	{
//		case 0: car1_park();break;
//		case 1: car2_park();break;
//		case 2: car3_park();break;
//		case 3: car4_park();break;
//	}

	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
		
	// ��ʾͣ���ɹ�
	printf("*************ͣ���ɹ�************\n\n\n");
	Print_Database();
	
	return;
}

// �շ�
void change(struct id_struct *card)
{
	uint8_t i;
	char date_park[9] = {0};		// 8�����ݣ����鳤��Ϊ9����Ϊ���һλ�洢'\0'
	char date_pick[9] = {0};
	char time_park[7] = {0};
	char time_pick[7] = {0};
	
	uint32_t park_time;
	uint32_t pick_time;
	uint32_t park_date;
	uint32_t pick_date;
	
	uint8_t park_temp[6];		// ������ͣ������127��
	uint8_t pick_temp[6];
	uint8_t value[6];		// ������ͣ������127��
	
	// ���ṹ���е�ͣ��ʱ���ȡ��ȡ������	
	for (i=0; i<8; i++)
	{
		date_park[i] = card->sector[PARK_PICK_SECTOR].block[PARK_BLOCK].data[i];
		date_pick[i] = card->sector[PARK_PICK_SECTOR].block[PICK_BLOCK].data[i];
	}
	
	for (i=0; i<6; i++)
	{
		// ��8�ַ��ǿ��ַ�
		time_park[i] = card->sector[PARK_PICK_SECTOR].block[PARK_BLOCK].data[i + 9];
		time_pick[i] = card->sector[PARK_PICK_SECTOR].block[PICK_BLOCK].data[i + 9];
	}
	
	// ���ַ���תΪ����
	park_date = atoll(date_park);
	park_time = atoll(time_park);
	pick_date = atoll(date_pick);
	pick_time = atoll(time_pick);
	
	printf("ͣ��ʱ��: %08d ", park_date);
	printf("%06d\n", park_time);
	printf("ȡ��ʱ��: %08d ", pick_date);
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
	
	// ��������ʱ���ֵ
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
	
	// ���ڴ�ӡ��TFT����ʾ
	printf("*********************************\n");
	for (i=5; i>0; i--)
	{
		if (value[i] != 0)
			break;
	}
	printf("       ��ͣ��  ");
	memset(temp_buf, 0, sizeof(temp_buf));
	GUI_DispNextLine();
	GUI_DispNextLine();
	GUI_DispNextLine();
	GUI_DispNextLine();
	GUI_DispString("     Length of Parking :  ");
	// switch ����ֻ��ʾ��Чʱ��
	switch (i)
	{
		case 5: printf("%d��", value[i]); sprintf(temp_buf, "%d year ", value[i]); GUI_DispString(temp_buf);
		case 4: printf("%d��", value[i]); sprintf(temp_buf, "%d month ", value[i]);GUI_DispString(temp_buf);
		case 3: printf("%d��", value[i]); sprintf(temp_buf, "%d day ", value[i]);GUI_DispString(temp_buf);
		case 2: printf("%dСʱ", value[i]); sprintf(temp_buf, "%d hour ", value[i]); GUI_DispString(temp_buf);
		case 1: printf("%d����", value[i]); sprintf(temp_buf, "%d min ", value[i]); GUI_DispString(temp_buf);
		case 0: printf("%d��", value[i]); sprintf(temp_buf, "%d sec", value[i]); GUI_DispString(temp_buf);
	}
	printf("\n");
	printf("*********************************\n");
	
//	// ����SD������ֹ�γ�SD������
//	HAL_SD_MspDeInit(&hsd);
	
	// ���㹻ʱ��۲�ͣ��ʱ��
	HAL_Delay(10000);
}

	// ��ӡ��������Ϣ
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
	
	
	printf("***********��������Ϣ**********\n\n");
	printf("         ����        ���ʱ��\n");
	GUI_SetColor(GUI_RED);
	for (i=0; i<MAX_CAR_NUM; i++)
	{
		sprintf(temp_buf, "No %d:   %s   |   %s", i+1, (char *)card[i].card_id, (char *)card[i].sector[PARK_PICK_SECTOR].block[PARK_BLOCK].data);
		GUI_DispNextLine();
		GUI_DispNextLine();
		GUI_DispString(temp_buf);
		printf("%d��λ: %s | %s\n", i +1 , (char *)card[i].card_id, (char *)card[i].sector[PARK_PICK_SECTOR].block[PARK_BLOCK].data);
	}
	return;
}
// �������ں�ʱ��
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
	
	printf("�����ʱ������\n");	
	Get_Date_Time();
}

// ��ȡ���ں�ʱ��
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
	
	printf("����ʱ��: %s\n", date_time);
}

void car1_park(void)
{
	// Z ����
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*78);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y �ջ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*92);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*12);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// �ֶ��� ˳ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_X_Pin, R_UP_DIRECTION);
	pulse(15*93);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	
	// z ����
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*177);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
/**********************************/
	// 1�ų���
	// r ��ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
	pulse(15*(89+4));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x �ұ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*(12-1));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(91+3+1));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z �½�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*137);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y �ջ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*(91+3));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*12);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// r ˳ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_UP_DIRECTION);
	pulse(15*(89+4));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
/*********************************/
	// ͨ�û�
	// r ��ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
	pulse(15*(89+4));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x �ұ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*12);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(92));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z �½�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*(118+2));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
}

void car2_park(void)
{
	// Z ����
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*70);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y �ջ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*92);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*12);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// �ֶ��� ˳ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_X_Pin, R_UP_DIRECTION);
	pulse(15*93);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z ����
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*160);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
/**********************************/
// 2�ų���
	// r ��ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
	pulse(15*(94));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*140);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(95-3-1));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z �½�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*(160-30));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y �ջ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*(92));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x �ұ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*140);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// r ˳ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_UP_DIRECTION);
	pulse(15*(1));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x �ұ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*12);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(92));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z �½�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*(70+30));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
}

void car3_park(void)
{
	// Z ����
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*78);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y �ջ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*93);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*12);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// �ֶ��� ˳ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_X_Pin, R_UP_DIRECTION);
	pulse(15*93);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z ����
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*177);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
/**********************************/
// 3�ų���
	// r ˳ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_UP_DIRECTION);
	pulse(15*(87));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*(113+3+4+3-2-1));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(95));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z �½�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*137);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y �ջ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*(95));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x �ұ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*(113+3+6+3-2-1));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// r ��ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
	pulse(15*(87));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
/*********************************/
	// ͨ�û�
	// r ��ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
	pulse(15*(89+4));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x �ұ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*12);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(93));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z �½�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*(118+2));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
}

void car4_park(void)
{
	// Z ����
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*78);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y �ջ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*93);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*12);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// �ֶ��� ˳ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_X_Pin, R_UP_DIRECTION);
	pulse(15*93);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z ����
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*177);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
/**********************************/
// 4�ų���
	// r ˳ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_UP_DIRECTION);
	pulse(15*(86+1-1));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x �ұ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
//	pulse(15*(27+1-3-1));
	pulse(15*(24-3));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(95-2));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z �½�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*137);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y �ջ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*(95-2));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
//	pulse(15*(27+1-3-1));
	pulse(15*(24-3));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// r ��ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
	pulse(15*(86+1-1));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
/*********************************/
	// ͨ�û�
	// r ��ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
	pulse(15*(89+4));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x �ұ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*(12-6+2));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(93-1));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z �½�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*(118+2));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
}

void debug(void)
{
// z �½�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*137);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y �ջ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*(95-3+1));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x �ұ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*140);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
//	// r ˳ʱ��
//	motor_flag = 1;		// ��־λ��1��ʾδ���
//	channel = CHANNEL_R;
//	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_UP_DIRECTION);
//	pulse(15*(94));
//	// �ȴ������λ
//	while (motor_flag)
//	{
//		HAL_Delay(10);
//	}
//	
//	// r ��ʱ��
//	motor_flag = 1;		// ��־λ��1��ʾδ���
//	channel = CHANNEL_R;
//	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
//	pulse(15*(89+4));
//	// �ȴ������λ
//	while (motor_flag)
//	{
//		HAL_Delay(10);
//	}

//	// r ˳ʱ��
//	motor_flag = 1;		// ��־λ��1��ʾδ���
//	channel = CHANNEL_R;
//	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_UP_DIRECTION);
//	pulse(15*(1));
//	// �ȴ������λ
//	while (motor_flag)
//	{
//		HAL_Delay(10);
//	}
	// x �ұ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*12);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(92));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z �½�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*(118+2));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
}

void car2_pick(void)
{
	// Z ����
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*70);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y �ջ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*92);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*12);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_LEFT_DIRECTION);
	pulse(15*140);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(95-3-1));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z ����
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_UP_DIRECTION);
	pulse(15*160);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// y �ջ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_DOWN_DIRECTION);
	pulse(15*(92));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x �ұ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*140);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// �ֶ��� ˳ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_X_Pin, R_UP_DIRECTION);
	pulse(15*93);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z �½�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*160);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// r ��ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_DOWN_DIRECTION);
	pulse(15*(94));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}	
	// r ˳ʱ��
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_R;
	HAL_GPIO_WritePin(Direction_R_GPIO_Port, Direction_R_Pin, R_UP_DIRECTION);
	pulse(15*(1));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// x �ұ�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_X;
	HAL_GPIO_WritePin(Direction_X_GPIO_Port, Direction_X_Pin, X_RIGHT_DIRECTION);
	pulse(15*12);
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// Y ���
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Y;
	HAL_GPIO_WritePin(Direction_Y_GPIO_Port, Direction_Y_Pin, Y_UP_DIRECTION);
	pulse(15*(92));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
	// z �½�
	motor_flag = 1;		// ��־λ��1��ʾδ���
	channel = CHANNEL_Z;
	HAL_GPIO_WritePin(Direction_Z_GPIO_Port, Direction_Z_Pin, Z_DOWN_DIRECTION);
	pulse(15*(70));
	// �ȴ������λ
	while (motor_flag)
	{
		HAL_Delay(10);
	}
}
