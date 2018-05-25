#include "rc522_app.h"

uint8_t KEY[6]={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

/*
 * 函数功能: 打印已存入结构体中某一扇区的4块数据
 * 输入参数: sector: 扇区
 *           *card : id_struct结构体指针
 * 返 回 值: 无
 * 说    明: 无
 */
void Print_Sector(uint8_t sector, uint8_t *key, struct id_struct *card)
{
	uint8_t i;
//	uint8_t j;
	uint8_t ret;
	printf("卡号：%s\n", card->card_id);
	
	ret = Read_Sector(sector, key, card);
	if (ret == MI_OK)
	{
		for (i=0; i<4; i++)
		{
			printf("第%d扇区第%d块: ", sector, i);
//				for (j=0; j<16; j++)
//				{
//					printf("%02x", card->sector[sector].block[i].data[j]);
//				}
			printf("%s", card->sector[sector].block[i].data);
			printf("\n");
		}
	}
}

/*
 * 函数功能: 读取某一扇区共4块数据并存入结构体中
 * 输入参数: sector: 扇区
 *           *card : id_struct结构体指针
 * 返 回 值: MI_OK : ok
 *					 MI_ERR: error
 * 说    明: 无
 */
uint8_t Read_Sector(uint8_t sector, uint8_t *key, struct id_struct *card)
{
	uint8_t ucStatusReturn;
	uint8_t i;
	
	ucStatusReturn = Read_ID(card);
	
	/*选卡*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdSelect(card->ucArray_ID);
	}
	
	/*选卡成功*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdAuthState(PICC_AUTHENT1B, sector * 4, key, card->ucArray_ID);
	}
	/*验证成功*/
	if( ucStatusReturn == MI_OK)
	{
		for (i=0; i<4; i++)
		{
			ucStatusReturn = PcdRead(sector * 4 + i, card->sector[sector].block[i].data);
		}
		if (ucStatusReturn == MI_OK)
		{
			/*成功*/
			return ucStatusReturn;
		}
	}
	/*失败*/
	printf("第%d扇区读取失败\n",sector);
	return MI_ERR;
}


/*
 * 函数功能: 写入某一扇区某一块
 * 输入参数: sector: 扇区
 *					 block : 块
 *           *card : id_struct结构体指针
 * 返 回 值: MI_OK : ok
 *					 MI_ERR: error
 * 说    明: 无
 */
uint8_t Write_Block(uint8_t sector, uint8_t block, uint8_t *key, uint8_t *pdata, struct id_struct *card)
{
	uint8_t ucStatusReturn;
	
	if (block > 2)
	{
		printf("控制块3不能写入\n");
		return MI_ERR;
	}
	
	/* 读取卡号 */
	ucStatusReturn = Read_ID(card);
	
	/*选卡*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdSelect(card->ucArray_ID);
	}
	
	/*选卡成功*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdAuthState(PICC_AUTHENT1A, sector * 4 + block, key, card->ucArray_ID);
	}
	/*验证成功*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdWrite(sector * 4 + block, pdata);
		
		if (ucStatusReturn == MI_OK)
		{
//			printf("第%d扇区第%d块写入成功\n",sector , block);
			/*成功*/
			return ucStatusReturn;
		}
	}
	/*失败*/
	printf("第%d扇区第%d块写入失败\n",sector , block);
	return MI_ERR;
}

/*
 * 函数功能: 验证密码
 * 输入参数: sector: 扇区
 *					 block : 块
 *           *card : id_struct结构体指针
 * 返 回 值: MI_OK : ok
 *					 MI_ERR: error
 * 说    明: 无
 */
uint8_t Check_PW(uint8_t sector, uint8_t block, uint8_t *key, struct id_struct *card)
{
	uint8_t ucStatusReturn;
	
	/* 读取卡号 */
	ucStatusReturn = Read_ID(card);
	
	/*选卡*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdSelect(card->ucArray_ID);
	}
	
	/*选卡成功*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdAuthState(PICC_AUTHENT1A, sector * 4 + block, key, card->ucArray_ID);
	}
	/*验证成功*/      
	if( ucStatusReturn == MI_OK)
	{
		/*成功*/
		return ucStatusReturn;
	}
	/*失败*/
	printf("第%d扇区%d块验证密码失败\n",sector , block);
	return MI_ERR;
}


/*
 * 函数功能: 读取某一扇区某一块
 * 输入参数: sector: 扇区
 *					 block : 块
 *           *card : id_struct结构体指针
 * 返 回 值: MI_OK : ok
 *					 MI_ERR: error
 * 说    明: 无
 */
uint8_t Read_Block(uint8_t sector, uint8_t block, uint8_t *key, struct id_struct *card)
{
	uint8_t ucStatusReturn;
	
	/* 读取卡号 */
	ucStatusReturn = Read_ID(card);
	
	/*选卡*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdSelect(card->ucArray_ID);
	}
	
	/*选卡成功*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdAuthState(PICC_AUTHENT1A, sector * 4 + block, key, card->ucArray_ID);
	}
	/*验证成功*/      
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdRead(sector * 4 + block, card->sector[sector].block[block].data);
		
		if (ucStatusReturn == MI_OK)
		{
			/*成功*/
			return ucStatusReturn;
		}
	}
	/*失败*/
	printf("第%d扇区第%d块读取失败\n",sector , block);
	return MI_ERR;
}

/*
 * 函数功能: 读取卡号
* 输入参数: *card: id_struct结构体指针
 * 返 回 值: 无
 * 说    明: 无
 */
uint8_t Read_ID(struct id_struct *card)
{
	uint8_t ucStatusReturn;				//返回状态

	/*寻卡*/
	ucStatusReturn = PcdRequest(PICC_REQIDL, card->ucArray_ID);
	if(ucStatusReturn != MI_OK)
	{
		/*若失败再次寻卡*/
		ucStatusReturn = PcdRequest(PICC_REQIDL, card->ucArray_ID);
	}
	if(ucStatusReturn == MI_OK)
	{
		/*防冲撞（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）*/
		if(PcdAnticoll(card->ucArray_ID) == MI_OK)
		{
//			memset(card->card_id, 0, sizeof(card->card_id));
			/* 储存卡号 */
			sprintf((char *)card->card_id, "%02X%02X%02X%02X", card->ucArray_ID[0], card->ucArray_ID[1], card->ucArray_ID[2], card->ucArray_ID[3]);
			
			return ucStatusReturn;		// 读取卡号直接返回
		}
	}
	/*失败*/
	return MI_ERR;
}

