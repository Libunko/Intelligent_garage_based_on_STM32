#ifndef __RC522_FUNCTION_H
#define	__RC522_FUNCTION_H

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define          macDummy_Data              0x00

/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
void   PcdReset             ( void );										//复位
void   M500PcdConfigISOType ( unsigned char type );								//工作方式
char   PcdRequest           ( unsigned char req_code, unsigned char * pTagType );		//寻卡
char   PcdAnticoll          ( unsigned char * pSnr);								//读卡号
char   PcdAuthState         ( unsigned char ucAuth_mode, unsigned char ucAddr, unsigned char * pKey, unsigned char * pSnr );//验证卡片密码
char   PcdWrite							( unsigned char ucAddr, unsigned char * pData );				// 写卡
char   PcdRead              ( unsigned char ucAddr, unsigned char * pData );			//读卡
char   PcdSelect            ( unsigned char * pSnr );								//选卡      
char	 PcdHalt( void );
#endif /* __RC522_FUNCTION_H */

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/

