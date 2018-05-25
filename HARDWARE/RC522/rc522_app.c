#include "rc522_app.h"

uint8_t KEY[6]={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

/*
 * ��������: ��ӡ�Ѵ���ṹ����ĳһ������4������
 * �������: sector: ����
 *           *card : id_struct�ṹ��ָ��
 * �� �� ֵ: ��
 * ˵    ��: ��
 */
void Print_Sector(uint8_t sector, uint8_t *key, struct id_struct *card)
{
	uint8_t i;
//	uint8_t j;
	uint8_t ret;
	printf("���ţ�%s\n", card->card_id);
	
	ret = Read_Sector(sector, key, card);
	if (ret == MI_OK)
	{
		for (i=0; i<4; i++)
		{
			printf("��%d������%d��: ", sector, i);
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
 * ��������: ��ȡĳһ������4�����ݲ�����ṹ����
 * �������: sector: ����
 *           *card : id_struct�ṹ��ָ��
 * �� �� ֵ: MI_OK : ok
 *					 MI_ERR: error
 * ˵    ��: ��
 */
uint8_t Read_Sector(uint8_t sector, uint8_t *key, struct id_struct *card)
{
	uint8_t ucStatusReturn;
	uint8_t i;
	
	ucStatusReturn = Read_ID(card);
	
	/*ѡ��*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdSelect(card->ucArray_ID);
	}
	
	/*ѡ���ɹ�*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdAuthState(PICC_AUTHENT1B, sector * 4, key, card->ucArray_ID);
	}
	/*��֤�ɹ�*/
	if( ucStatusReturn == MI_OK)
	{
		for (i=0; i<4; i++)
		{
			ucStatusReturn = PcdRead(sector * 4 + i, card->sector[sector].block[i].data);
		}
		if (ucStatusReturn == MI_OK)
		{
			/*�ɹ�*/
			return ucStatusReturn;
		}
	}
	/*ʧ��*/
	printf("��%d������ȡʧ��\n",sector);
	return MI_ERR;
}


/*
 * ��������: д��ĳһ����ĳһ��
 * �������: sector: ����
 *					 block : ��
 *           *card : id_struct�ṹ��ָ��
 * �� �� ֵ: MI_OK : ok
 *					 MI_ERR: error
 * ˵    ��: ��
 */
uint8_t Write_Block(uint8_t sector, uint8_t block, uint8_t *key, uint8_t *pdata, struct id_struct *card)
{
	uint8_t ucStatusReturn;
	
	if (block > 2)
	{
		printf("���ƿ�3����д��\n");
		return MI_ERR;
	}
	
	/* ��ȡ���� */
	ucStatusReturn = Read_ID(card);
	
	/*ѡ��*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdSelect(card->ucArray_ID);
	}
	
	/*ѡ���ɹ�*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdAuthState(PICC_AUTHENT1A, sector * 4 + block, key, card->ucArray_ID);
	}
	/*��֤�ɹ�*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdWrite(sector * 4 + block, pdata);
		
		if (ucStatusReturn == MI_OK)
		{
//			printf("��%d������%d��д��ɹ�\n",sector , block);
			/*�ɹ�*/
			return ucStatusReturn;
		}
	}
	/*ʧ��*/
	printf("��%d������%d��д��ʧ��\n",sector , block);
	return MI_ERR;
}

/*
 * ��������: ��֤����
 * �������: sector: ����
 *					 block : ��
 *           *card : id_struct�ṹ��ָ��
 * �� �� ֵ: MI_OK : ok
 *					 MI_ERR: error
 * ˵    ��: ��
 */
uint8_t Check_PW(uint8_t sector, uint8_t block, uint8_t *key, struct id_struct *card)
{
	uint8_t ucStatusReturn;
	
	/* ��ȡ���� */
	ucStatusReturn = Read_ID(card);
	
	/*ѡ��*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdSelect(card->ucArray_ID);
	}
	
	/*ѡ���ɹ�*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdAuthState(PICC_AUTHENT1A, sector * 4 + block, key, card->ucArray_ID);
	}
	/*��֤�ɹ�*/      
	if( ucStatusReturn == MI_OK)
	{
		/*�ɹ�*/
		return ucStatusReturn;
	}
	/*ʧ��*/
	printf("��%d����%d����֤����ʧ��\n",sector , block);
	return MI_ERR;
}


/*
 * ��������: ��ȡĳһ����ĳһ��
 * �������: sector: ����
 *					 block : ��
 *           *card : id_struct�ṹ��ָ��
 * �� �� ֵ: MI_OK : ok
 *					 MI_ERR: error
 * ˵    ��: ��
 */
uint8_t Read_Block(uint8_t sector, uint8_t block, uint8_t *key, struct id_struct *card)
{
	uint8_t ucStatusReturn;
	
	/* ��ȡ���� */
	ucStatusReturn = Read_ID(card);
	
	/*ѡ��*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdSelect(card->ucArray_ID);
	}
	
	/*ѡ���ɹ�*/
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdAuthState(PICC_AUTHENT1A, sector * 4 + block, key, card->ucArray_ID);
	}
	/*��֤�ɹ�*/      
	if( ucStatusReturn == MI_OK)
	{
		ucStatusReturn = PcdRead(sector * 4 + block, card->sector[sector].block[block].data);
		
		if (ucStatusReturn == MI_OK)
		{
			/*�ɹ�*/
			return ucStatusReturn;
		}
	}
	/*ʧ��*/
	printf("��%d������%d���ȡʧ��\n",sector , block);
	return MI_ERR;
}

/*
 * ��������: ��ȡ����
* �������: *card: id_struct�ṹ��ָ��
 * �� �� ֵ: ��
 * ˵    ��: ��
 */
uint8_t Read_ID(struct id_struct *card)
{
	uint8_t ucStatusReturn;				//����״̬

	/*Ѱ��*/
	ucStatusReturn = PcdRequest(PICC_REQIDL, card->ucArray_ID);
	if(ucStatusReturn != MI_OK)
	{
		/*��ʧ���ٴ�Ѱ��*/
		ucStatusReturn = PcdRequest(PICC_REQIDL, card->ucArray_ID);
	}
	if(ucStatusReturn == MI_OK)
	{
		/*����ײ�����ж��ſ������д��������Χʱ������ͻ���ƻ������ѡ��һ�Ž��в�����*/
		if(PcdAnticoll(card->ucArray_ID) == MI_OK)
		{
//			memset(card->card_id, 0, sizeof(card->card_id));
			/* ���濨�� */
			sprintf((char *)card->card_id, "%02X%02X%02X%02X", card->ucArray_ID[0], card->ucArray_ID[1], card->ucArray_ID[2], card->ucArray_ID[3]);
			
			return ucStatusReturn;		// ��ȡ����ֱ�ӷ���
		}
	}
	/*ʧ��*/
	return MI_ERR;
}

