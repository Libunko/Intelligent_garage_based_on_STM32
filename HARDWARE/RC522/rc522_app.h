#ifndef __RC522_APP_H
#define __RC522_APP_H

#include "stm32f4xx_hal.h"
#include "string.h"
#include "rc522_function.h"
#include "rc522_config.h"
#include "rtc.h"
#include <stdlib.h>





struct block_struct
{
	uint8_t data[16];
};

struct sector_struct
{
	struct block_struct block[4];
};

struct id_struct
{
	struct sector_struct sector[16];
	uint8_t ucArray_ID[4];
	uint8_t card_id[9];		// 最后个元素存放'\0'
};



uint8_t Read_ID(struct id_struct *card);
uint8_t Check_PW(uint8_t sector, uint8_t block, uint8_t *key, struct id_struct *card);
void Print_Sector(uint8_t sector, uint8_t *key, struct id_struct *card);
uint8_t Read_Sector(uint8_t sector, uint8_t *key, struct id_struct *card);
uint8_t Write_Block(uint8_t sector, uint8_t block, uint8_t *key, uint8_t *pdata, struct id_struct *card);
uint8_t Read_Block(uint8_t sector, uint8_t block, uint8_t *key, struct id_struct *card);






#endif


