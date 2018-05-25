#include "ILI93xx.h"
#include "ascii.h"

//����Һ��ɨ�跽����仯��XY���ؿ��
//����ILI9341_GramScan�������÷���ʱ���Զ�����
uint16_t LCD_X_LENGTH = ILI9341_LESS_PIXEL;
uint16_t LCD_Y_LENGTH = ILI9341_MORE_PIXEL;

//Һ����ɨ��ģʽ����������Ҫ���ڷ���ѡ�������ļ������
//������ѡֵΪ0-7
//����ILI9341_GramScan�������÷���ʱ���Զ�����
//LCD�ճ�ʼ�����ʱ��ʹ�ñ�Ĭ��ֵ
uint8_t LCD_SCAN_MODE = 6;

/**
  * @brief  ���� ILI9341 ����ʱ����
  * @param  nCount ����ʱ����ֵ
  * @retval ��
  */	
static void ILI9341_Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}
void ILI9341_Rst ( void )
{			
	//GPIO_ResetBits ( macILI9341_RST_PORT, macILI9341_RST_PIN );	 //�͵�ƽ��λ
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_1,GPIO_PIN_RESET);
	ILI9341_Delay ( 0xAFFf<<2 ); 					   
	//GPIO_SetBits ( macILI9341_RST_PORT, macILI9341_RST_PIN );		
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_1,GPIO_PIN_SET);	
	ILI9341_Delay ( 0xAFFf<<2 ); 	
}
static void ILI9341_REG_Config ( void )
{
	/*  Power control B (CFh)  */
	LCD_WRITE_CMD(0xCF);
	LCD_WRITE_DATA(0x00);
	LCD_WRITE_DATA(0x81);
	LCD_WRITE_DATA(0x30);
	
	/*  Power on sequence control (EDh) */
	LCD_WRITE_CMD ( 0xED );
	LCD_WRITE_DATA ( 0x64 );
	LCD_WRITE_DATA ( 0x03 );
	LCD_WRITE_DATA ( 0x12 );
	LCD_WRITE_DATA ( 0x81 );
	
	/*  Driver timing control A (E8h) */
	LCD_WRITE_CMD ( 0xE8 );
	LCD_WRITE_DATA ( 0x85 );
	LCD_WRITE_DATA ( 0x10 );
	LCD_WRITE_DATA ( 0x78 );
	
	/*  Power control A (CBh) */
	LCD_WRITE_CMD ( 0xCB );
	LCD_WRITE_DATA ( 0x39 );
	LCD_WRITE_DATA ( 0x2C );
	LCD_WRITE_DATA ( 0x00 );
	LCD_WRITE_DATA ( 0x34 );
	LCD_WRITE_DATA ( 0x02 );
	
	/* Pump ratio control (F7h) */
	LCD_WRITE_CMD ( 0xF7 );
	LCD_WRITE_DATA ( 0x20 );
	
	/* Driver timing control B */
	LCD_WRITE_CMD ( 0xEA );
	LCD_WRITE_DATA ( 0x00 );
	LCD_WRITE_DATA ( 0x00 );
	
	/* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
	LCD_WRITE_CMD ( 0xB1 );
	LCD_WRITE_DATA ( 0x00 );
	LCD_WRITE_DATA ( 0x1B );
	
	/*  Display Function Control (B6h) */
	LCD_WRITE_CMD ( 0xB6 );
	LCD_WRITE_DATA ( 0x0A );
	LCD_WRITE_DATA ( 0xA2 );
	
	/* Power Control 1 (C0h) */
	LCD_WRITE_CMD ( 0xC0 );
	LCD_WRITE_DATA ( 0x35 );
	
	/* Power Control 2 (C1h) */
	LCD_WRITE_CMD ( 0xC1 );
	LCD_WRITE_DATA ( 0x11 );
	
	/* VCOM Control 1 (C5h) */
	LCD_WRITE_CMD ( 0xC5 );
	LCD_WRITE_DATA ( 0x45 );
	LCD_WRITE_DATA ( 0x45 );
	
	/*  VCOM Control 2 (C7h)  */
	LCD_WRITE_CMD ( 0xC7 );
	LCD_WRITE_DATA ( 0xA2 );
	
	/* Enable 3G (F2h) */
	LCD_WRITE_CMD ( 0xF2 );
	LCD_WRITE_DATA ( 0x00 );
	
	/* Gamma Set (26h) */
	LCD_WRITE_CMD ( 0x26 );
	LCD_WRITE_DATA ( 0x01 );

	/* Positive Gamma Correction */
	LCD_WRITE_CMD ( 0xE0 ); //Set Gamma
	LCD_WRITE_DATA ( 0x0F );
	LCD_WRITE_DATA ( 0x26 );
	LCD_WRITE_DATA ( 0x24 );
	LCD_WRITE_DATA ( 0x0B );
	LCD_WRITE_DATA ( 0x0E );
	LCD_WRITE_DATA ( 0x09 );
	LCD_WRITE_DATA ( 0x54 );
	LCD_WRITE_DATA ( 0xA8 );
	LCD_WRITE_DATA ( 0x46 );
	LCD_WRITE_DATA ( 0x0C );
	LCD_WRITE_DATA ( 0x17 );
	LCD_WRITE_DATA ( 0x09 );
	LCD_WRITE_DATA ( 0x0F );
	LCD_WRITE_DATA ( 0x07 );
	LCD_WRITE_DATA ( 0x00 );
	
	/* Negative Gamma Correction (E1h) */
	LCD_WRITE_CMD ( 0XE1 ); //Set Gamma
	LCD_WRITE_DATA ( 0x00 );
	LCD_WRITE_DATA ( 0x19 );
	LCD_WRITE_DATA ( 0x1B );
	LCD_WRITE_DATA ( 0x04 );
	LCD_WRITE_DATA ( 0x10 );
	LCD_WRITE_DATA ( 0x07 );
	LCD_WRITE_DATA ( 0x2A );
	LCD_WRITE_DATA ( 0x47 );
	LCD_WRITE_DATA ( 0x39 );
	LCD_WRITE_DATA ( 0x03 );
	LCD_WRITE_DATA ( 0x06 );
	LCD_WRITE_DATA ( 0x06 );
	LCD_WRITE_DATA ( 0x30 );
	LCD_WRITE_DATA ( 0x38 );
	LCD_WRITE_DATA ( 0x0F );
	
	/* memory access control set */
	LCD_WRITE_CMD ( 0x36 ); 	
	LCD_WRITE_DATA ( 0xC8 );    /*����  ���Ͻǵ� (���)�����½� (�յ�)ɨ�跽ʽ*/
	
	/* column address control set */
	LCD_WRITE_CMD ( 0X2A ); 
	LCD_WRITE_DATA ( 0x00 );
	LCD_WRITE_DATA ( 0x00 );
	LCD_WRITE_DATA ( 0x00 );
	LCD_WRITE_DATA ( 0xEF );
	
	/* page address control set */
	LCD_WRITE_CMD ( 0X2B ); 
	LCD_WRITE_DATA ( 0x00 );
	LCD_WRITE_DATA ( 0x00 );
	LCD_WRITE_DATA ( 0x01 );
	LCD_WRITE_DATA ( 0x3F );
	
	/*  Pixel Format Set (3Ah)  */
	LCD_WRITE_CMD ( 0x3a ); 
	LCD_WRITE_DATA ( 0x55 );
	
	/* Sleep Out (11h)  */
	LCD_WRITE_CMD ( 0x11 );	
	ILI9341_Delay ( 0xAFFf<<2 );
	
	/* Display ON (29h) */
	LCD_WRITE_CMD ( 0x29 ); 
	
	
}
/**
 * @brief  ����ILI9341��GRAM��ɨ�跽�� 
 * @param  ucOption ��ѡ��GRAM��ɨ�跽�� 
 *     @arg 0-7 :������ѡֵΪ0-7��˸�����
 *
 *	����������0��3��5��6 ģʽ�ʺϴ���������ʾ���֣�
 *				���Ƽ�ʹ������ģʽ��ʾ����	����ģʽ��ʾ���ֻ��о���Ч��			
 *		
 *	����0��2��4��6 ģʽ��X��������Ϊ240��Y��������Ϊ320
 *	����1��3��5��7 ģʽ��X��������Ϊ320��Y��������Ϊ240
 *
 *	���� 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����
 *	���� 3 ģʽΪ����ͷ����ʹ�õķ���
 *	���� 0 ģʽΪBMPͼƬ��ʾ����ʹ�õķ���
 *
 * @retval ��
 * @note  ����ͼ����A��ʾ���ϣ�V��ʾ���£�<��ʾ����>��ʾ����
					X��ʾX�ᣬY��ʾY��

------------------------------------------------------------
ģʽ0��				.		ģʽ1��		.	ģʽ2��			.	ģʽ3��					
					A		.					A		.		A					.		A									
					|		.					|		.		|					.		|							
					Y		.					X		.		Y					.		X					
					0		.					1		.		2					.		3					
	<--- X0 o		.	<----Y1	o		.		o 2X--->  .		o 3Y--->	
------------------------------------------------------------	
ģʽ4��				.	ģʽ5��			.	ģʽ6��			.	ģʽ7��					
	<--- X4 o		.	<--- Y5 o		.		o 6X--->  .		o 7Y--->	
					4		.					5		.		6					.		7	
					Y		.					X		.		Y					.		X						
					|		.					|		.		|					.		|							
					V		.					V		.		V					.		V		
---------------------------------------------------------				
											 LCD��ʾ��
								|-----------------|
								|			����Logo		|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|-----------------|
								��Ļ���棨��240����320��

 *******************************************************/
void LCD_GramScan ( uint8_t ucOption )
{	
	//������飬ֻ������0-7
	if(ucOption >7 )
		return;
	
	//����ģʽ����LCD_SCAN_MODE��ֵ����Ҫ���ڴ�����ѡ��������
	LCD_SCAN_MODE = ucOption;
	
	//����ģʽ����XY��������ؿ��
	if(ucOption%2 == 0)	
	{
		//0 2 4 6ģʽ��X�������ؿ��Ϊ240��Y����Ϊ320
		LCD_X_LENGTH = ILI9341_LESS_PIXEL;
		LCD_Y_LENGTH =	ILI9341_MORE_PIXEL;
	}
	else				
	{
		//1 3 5 7ģʽ��X�������ؿ��Ϊ320��Y����Ϊ240
		LCD_X_LENGTH = ILI9341_MORE_PIXEL;
		LCD_Y_LENGTH =	ILI9341_LESS_PIXEL; 
	}

	//0x36��������ĸ�3λ����������GRAMɨ�跽��	
	LCD_WRITE_CMD ( 0x36 ); 
	LCD_WRITE_DATA ( 0x08 |(ucOption<<5));//����ucOption��ֵ����LCD��������0-7��ģʽ
	LCD_WRITE_CMD ( 0x2A ); 
	LCD_WRITE_DATA ( 0x00 );		/* x ��ʼ�����8λ */
	LCD_WRITE_DATA ( 0x00 );		/* x ��ʼ�����8λ */
	LCD_WRITE_DATA ( ((LCD_X_LENGTH-1)>>8)&0xFF ); /* x ���������8λ */	
	LCD_WRITE_DATA ( (LCD_X_LENGTH-1)&0xFF );				/* x ���������8λ */

	LCD_WRITE_CMD ( 0X2B ); 
	LCD_WRITE_DATA ( 0x00 );		/* y ��ʼ�����8λ */
	LCD_WRITE_DATA ( 0x00 );		/* y ��ʼ�����8λ */
	LCD_WRITE_DATA ( ((LCD_Y_LENGTH-1)>>8)&0xFF );	/* y ���������8λ */	 
	LCD_WRITE_DATA ( (LCD_Y_LENGTH-1)&0xFF );				/* y ���������8λ */

	/* write gram start */
	LCD_WRITE_CMD ( 0x2C );	
}
/**
 * @brief  ��ILI9341��ʾ���Ͽ���һ������
 * @param  usX �����ض�ɨ�跽���´��ڵ����X����
 * @param  usY �����ض�ɨ�跽���´��ڵ����Y����
 * @param  usWidth �����ڵĿ��
 * @param  usHeight �����ڵĸ߶�
 * @retval ��
 */

void LCD_OpenWindow(uint16_t usX,uint16_t usY,uint16_t usWidth,uint16_t usHeight)
{
	LCD_WRITE_CMD(0X2A);									//����X����
	LCD_WRITE_DATA(usX>>8);								//������ʼ���ȸ�8λ
	LCD_WRITE_DATA(usX&0xff);							//������ʼ���8λ
	LCD_WRITE_DATA((usX+usWidth-1)>>8);		//���ý����㣺��8λ
	LCD_WRITE_DATA((usX+usWidth-1)&0xff);	//�����㣺��8λ
	
	LCD_WRITE_CMD(0x2B);		//����Y����
	LCD_WRITE_DATA(usY>>8);								//������ʼ���ȸ�8λ
	LCD_WRITE_DATA(usY&0xff);							//������ʼ���8λ
	LCD_WRITE_DATA((usY+usHeight-1)>>8);		//���ý����㣺��8λ
	LCD_WRITE_DATA((usY+usHeight-1)&0xff);	//�����㣺��8λ
}
/**
 * @brief  ILI9341��ʼ�����������Ҫ�õ�lcd��һ��Ҫ�����������
 * @param  ��
 * @retval ��
 */

//���ù��λ��
//Xpos:������
//Ypos:������
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	   
	LCD_OpenWindow ( Xpos, Ypos, 1, 1 );	
} 
/**
 * @brief  ��ILI9341��ʾ������ĳһ��ɫ������ص�
 * @param  ulAmout_Point ��Ҫ�����ɫ�����ص������Ŀ
 * @param  usColor ����ɫ
 * @retval ��
 */
static __inline void LCD_FillColor(uint32_t ulAmout_Point,uint16_t usColor)
{
	uint32_t i = 0;
	/* ��ʼ��GRAMд������ */
	LCD_WRITE_CMD(0X2C);	
	for (i=0;i<ulAmout_Point;i++)
		LCD_WRITE_DATA(usColor);
}
/**
 * @brief  ��ILI9341��ʾ����ĳһ������ĳ����ɫ��������
 * @param  usX �����ض�ɨ�跽���´��ڵ����X����
 * @param  usY �����ض�ɨ�跽���´��ڵ����Y����
 * @param  usWidth �����ڵĿ��
 * @param  usHeight �����ڵĸ߶�
 * @param  usColor ����ɫ
 * @retval ��
 */
void LCD_Clear(uint16_t usX,uint16_t usY,uint16_t usWidth,uint16_t usHeight,uint16_t usColor)
{
	LCD_OpenWindow(usX,usY,usWidth,usHeight );
	LCD_FillColor(usWidth * usHeight,usColor);		
}
/**
 * @brief  ��ILI9341��ʾ����ĳһ����ĳ����ɫ�������
 * @param  usX �����ض�ɨ�跽���¸õ��X����
 * @param  usY �����ض�ɨ�跽���¸õ��Y����
 * @param  usColor ������������ɫ
 * @retval ��
 */
void LCD_SetPointPixel(uint16_t usX,uint16_t usY,uint16_t usColor)	
{	
	if ((usX<LCD_X_LENGTH)&&(usY<LCD_Y_LENGTH))
  {
		LCD_SetCursor ( usX, usY );
		LCD_FillColor(1,usColor);
	}
}

/**
 * @brief  ��ȡILI9341 GRAN ��һ����������
 * @param  ��
 * @retval ��������
 */
static uint16_t LCD_Read_PixelData ( void )	
{	
	uint16_t usR=0, usG=0, usB=0 ;

	
	LCD_WRITE_CMD ( 0x2E );   /* ������ */
	
	usR = LCD_READ_DATA (); 	/*FIRST READ OUT DUMMY DATA*/
	
	usR = LCD_READ_DATA ();  	/*READ OUT RED DATA  */
	usB = LCD_READ_DATA ();  	/*READ OUT BLUE DATA*/
	usG = LCD_READ_DATA ();  	/*READ OUT GREEN DATA*/	
	
  return ( ( ( usR >> 11 ) << 11 ) | ( ( usG >> 10 ) << 5 ) | ( usB >> 11 ) );
	
}

/**
 * @brief  ��ȡ ILI9341 ��ʾ����ĳһ����������������
 * @param  usX �����ض�ɨ�跽���¸õ��X����
 * @param  usY �����ض�ɨ�跽���¸õ��Y����
 * @retval ��������
 */
uint16_t LCD_GetPointPixel ( uint16_t usX, uint16_t usY )
{ 
	uint16_t usPixelData;

	
	LCD_SetCursor ( usX, usY );
	
	usPixelData = LCD_Read_PixelData ();
	
	return usPixelData;
	
}
void LCD_BL_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	LCD_GPIO_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = LCD_BL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_GPIO_PORT, &GPIO_InitStruct);
}
void LCD_Init ( void )
{
	LCD_BL_GPIO_Config();
	LCD_BL_ON();	      //����LCD�����
	ILI9341_Rst ();
	ILI9341_REG_Config ();
}


void LCD_ShowChar(uint16_t usX,uint16_t usY,const char cChar,uint16_t usColor_Foreground,uint16_t usColor_Background)
{
	uint8_t ucPage,ucTemp,usRelativePositon,ucColumn;
	
	usRelativePositon = cChar-' ';
	
	LCD_OpenWindow(usX,usY,8,16);
	LCD_WRITE_CMD(0x2C);	//дGRAM
	
	for(ucPage=0;ucPage<16;ucPage++)
	{
		ucTemp=ucAscii_1608[usRelativePositon][ucPage];
		for(ucColumn=0;ucColumn<8;ucColumn++)
		{
			if(ucTemp&0x01)
				LCD_WRITE_DATA(usColor_Foreground);
			else
				LCD_WRITE_DATA(usColor_Background);
			ucTemp>>=1;
		}
	}
}	
void LCD_ShowString(uint16_t usX,uint16_t usY,const char *pStr,uint16_t usColor_Foreground,uint16_t usColor_Background)
{
	while(*pStr != '\0')
	{
		if((usX+8)>LCD_Default_Max_Width)
		{
			usX=0;
			usY+=16;
		}
		if((usY+16)>LCD_Default_Max_Heigth)
		{
			usX=0;
			usY=0;
		}
		LCD_ShowChar(usX,usY,*pStr,usColor_Foreground,usColor_Background);
		pStr++;
		usX+=8;
	}
}

uint32_t LCD_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}			 
void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint16_t usColor_Foreground,uint16_t usColor_Background)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(16/2)*t,y,' ',usColor_Foreground,usColor_Background);
				LCD_ShowChar(x+(16/2)*t,y,' ',usColor_Foreground,usColor_Background);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(16/2)*t,y,temp+'0',usColor_Foreground,usColor_Background); 
	}
} 



