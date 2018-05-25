#ifndef __RC522_FUNCTION_H
#define	__RC522_FUNCTION_H

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
#define          macDummy_Data              0x00

/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
void   PcdReset             ( void );										//��λ
void   M500PcdConfigISOType ( unsigned char type );								//������ʽ
char   PcdRequest           ( unsigned char req_code, unsigned char * pTagType );		//Ѱ��
char   PcdAnticoll          ( unsigned char * pSnr);								//������
char   PcdAuthState         ( unsigned char ucAuth_mode, unsigned char ucAddr, unsigned char * pKey, unsigned char * pSnr );//��֤��Ƭ����
char   PcdWrite							( unsigned char ucAddr, unsigned char * pData );				// д��
char   PcdRead              ( unsigned char ucAddr, unsigned char * pData );			//����
char   PcdSelect            ( unsigned char * pSnr );								//ѡ��      
char	 PcdHalt( void );
#endif /* __RC522_FUNCTION_H */

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/

