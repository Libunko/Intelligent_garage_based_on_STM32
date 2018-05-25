#include "ILI93xx.h"
#include "ascii.h"

//根据液晶扫描方向而变化的XY像素宽度
//调用ILI9341_GramScan函数设置方向时会自动更改
uint16_t LCD_X_LENGTH = ILI9341_LESS_PIXEL;
uint16_t LCD_Y_LENGTH = ILI9341_MORE_PIXEL;

//液晶屏扫描模式，本变量主要用于方便选择触摸屏的计算参数
//参数可选值为0-7
//调用ILI9341_GramScan函数设置方向时会自动更改
//LCD刚初始化完成时会使用本默认值
uint8_t LCD_SCAN_MODE = 6;

/**
  * @brief  用于 ILI9341 简单延时函数
  * @param  nCount ：延时计数值
  * @retval 无
  */	
static void ILI9341_Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}
void ILI9341_Rst ( void )
{			
	//GPIO_ResetBits ( macILI9341_RST_PORT, macILI9341_RST_PIN );	 //低电平复位
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
	LCD_WRITE_DATA ( 0xC8 );    /*竖屏  左上角到 (起点)到右下角 (终点)扫描方式*/
	
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
 * @brief  设置ILI9341的GRAM的扫描方向 
 * @param  ucOption ：选择GRAM的扫描方向 
 *     @arg 0-7 :参数可选值为0-7这八个方向
 *
 *	！！！其中0、3、5、6 模式适合从左至右显示文字，
 *				不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			
 *		
 *	其中0、2、4、6 模式的X方向像素为240，Y方向像素为320
 *	其中1、3、5、7 模式下X方向像素为320，Y方向像素为240
 *
 *	其中 6 模式为大部分液晶例程的默认显示方向
 *	其中 3 模式为摄像头例程使用的方向
 *	其中 0 模式为BMP图片显示例程使用的方向
 *
 * @retval 无
 * @note  坐标图例：A表示向上，V表示向下，<表示向左，>表示向右
					X表示X轴，Y表示Y轴

------------------------------------------------------------
模式0：				.		模式1：		.	模式2：			.	模式3：					
					A		.					A		.		A					.		A									
					|		.					|		.		|					.		|							
					Y		.					X		.		Y					.		X					
					0		.					1		.		2					.		3					
	<--- X0 o		.	<----Y1	o		.		o 2X--->  .		o 3Y--->	
------------------------------------------------------------	
模式4：				.	模式5：			.	模式6：			.	模式7：					
	<--- X4 o		.	<--- Y5 o		.		o 6X--->  .		o 7Y--->	
					4		.					5		.		6					.		7	
					Y		.					X		.		Y					.		X						
					|		.					|		.		|					.		|							
					V		.					V		.		V					.		V		
---------------------------------------------------------				
											 LCD屏示例
								|-----------------|
								|			秉火Logo		|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|-----------------|
								屏幕正面（宽240，高320）

 *******************************************************/
void LCD_GramScan ( uint8_t ucOption )
{	
	//参数检查，只可输入0-7
	if(ucOption >7 )
		return;
	
	//根据模式更新LCD_SCAN_MODE的值，主要用于触摸屏选择计算参数
	LCD_SCAN_MODE = ucOption;
	
	//根据模式更新XY方向的像素宽度
	if(ucOption%2 == 0)	
	{
		//0 2 4 6模式下X方向像素宽度为240，Y方向为320
		LCD_X_LENGTH = ILI9341_LESS_PIXEL;
		LCD_Y_LENGTH =	ILI9341_MORE_PIXEL;
	}
	else				
	{
		//1 3 5 7模式下X方向像素宽度为320，Y方向为240
		LCD_X_LENGTH = ILI9341_MORE_PIXEL;
		LCD_Y_LENGTH =	ILI9341_LESS_PIXEL; 
	}

	//0x36命令参数的高3位可用于设置GRAM扫描方向	
	LCD_WRITE_CMD ( 0x36 ); 
	LCD_WRITE_DATA ( 0x08 |(ucOption<<5));//根据ucOption的值设置LCD参数，共0-7种模式
	LCD_WRITE_CMD ( 0x2A ); 
	LCD_WRITE_DATA ( 0x00 );		/* x 起始坐标高8位 */
	LCD_WRITE_DATA ( 0x00 );		/* x 起始坐标低8位 */
	LCD_WRITE_DATA ( ((LCD_X_LENGTH-1)>>8)&0xFF ); /* x 结束坐标高8位 */	
	LCD_WRITE_DATA ( (LCD_X_LENGTH-1)&0xFF );				/* x 结束坐标低8位 */

	LCD_WRITE_CMD ( 0X2B ); 
	LCD_WRITE_DATA ( 0x00 );		/* y 起始坐标高8位 */
	LCD_WRITE_DATA ( 0x00 );		/* y 起始坐标低8位 */
	LCD_WRITE_DATA ( ((LCD_Y_LENGTH-1)>>8)&0xFF );	/* y 结束坐标高8位 */	 
	LCD_WRITE_DATA ( (LCD_Y_LENGTH-1)&0xFF );				/* y 结束坐标低8位 */

	/* write gram start */
	LCD_WRITE_CMD ( 0x2C );	
}
/**
 * @brief  在ILI9341显示器上开辟一个窗口
 * @param  usX ：在特定扫描方向下窗口的起点X坐标
 * @param  usY ：在特定扫描方向下窗口的起点Y坐标
 * @param  usWidth ：窗口的宽度
 * @param  usHeight ：窗口的高度
 * @retval 无
 */

void LCD_OpenWindow(uint16_t usX,uint16_t usY,uint16_t usWidth,uint16_t usHeight)
{
	LCD_WRITE_CMD(0X2A);									//设置X坐标
	LCD_WRITE_DATA(usX>>8);								//设置起始点先高8位
	LCD_WRITE_DATA(usX&0xff);							//设置起始点低8位
	LCD_WRITE_DATA((usX+usWidth-1)>>8);		//设置结束点：高8位
	LCD_WRITE_DATA((usX+usWidth-1)&0xff);	//结束点：低8位
	
	LCD_WRITE_CMD(0x2B);		//设置Y坐标
	LCD_WRITE_DATA(usY>>8);								//设置起始点先高8位
	LCD_WRITE_DATA(usY&0xff);							//设置起始点低8位
	LCD_WRITE_DATA((usY+usHeight-1)>>8);		//设置结束点：高8位
	LCD_WRITE_DATA((usY+usHeight-1)&0xff);	//结束点：低8位
}
/**
 * @brief  ILI9341初始化函数，如果要用到lcd，一定要调用这个函数
 * @param  无
 * @retval 无
 */

//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	   
	LCD_OpenWindow ( Xpos, Ypos, 1, 1 );	
} 
/**
 * @brief  在ILI9341显示器上以某一颜色填充像素点
 * @param  ulAmout_Point ：要填充颜色的像素点的总数目
 * @param  usColor ：颜色
 * @retval 无
 */
static __inline void LCD_FillColor(uint32_t ulAmout_Point,uint16_t usColor)
{
	uint32_t i = 0;
	/* 开始向GRAM写入数据 */
	LCD_WRITE_CMD(0X2C);	
	for (i=0;i<ulAmout_Point;i++)
		LCD_WRITE_DATA(usColor);
}
/**
 * @brief  对ILI9341显示器的某一窗口以某种颜色进行清屏
 * @param  usX ：在特定扫描方向下窗口的起点X坐标
 * @param  usY ：在特定扫描方向下窗口的起点Y坐标
 * @param  usWidth ：窗口的宽度
 * @param  usHeight ：窗口的高度
 * @param  usColor ：颜色
 * @retval 无
 */
void LCD_Clear(uint16_t usX,uint16_t usY,uint16_t usWidth,uint16_t usHeight,uint16_t usColor)
{
	LCD_OpenWindow(usX,usY,usWidth,usHeight );
	LCD_FillColor(usWidth * usHeight,usColor);		
}
/**
 * @brief  对ILI9341显示器的某一点以某种颜色进行填充
 * @param  usX ：在特定扫描方向下该点的X坐标
 * @param  usY ：在特定扫描方向下该点的Y坐标
 * @param  usColor ：用于填充的颜色
 * @retval 无
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
 * @brief  读取ILI9341 GRAN 的一个像素数据
 * @param  无
 * @retval 像素数据
 */
static uint16_t LCD_Read_PixelData ( void )	
{	
	uint16_t usR=0, usG=0, usB=0 ;

	
	LCD_WRITE_CMD ( 0x2E );   /* 读数据 */
	
	usR = LCD_READ_DATA (); 	/*FIRST READ OUT DUMMY DATA*/
	
	usR = LCD_READ_DATA ();  	/*READ OUT RED DATA  */
	usB = LCD_READ_DATA ();  	/*READ OUT BLUE DATA*/
	usG = LCD_READ_DATA ();  	/*READ OUT GREEN DATA*/	
	
  return ( ( ( usR >> 11 ) << 11 ) | ( ( usG >> 10 ) << 5 ) | ( usB >> 11 ) );
	
}

/**
 * @brief  获取 ILI9341 显示器上某一个坐标点的像素数据
 * @param  usX ：在特定扫描方向下该点的X坐标
 * @param  usY ：在特定扫描方向下该点的Y坐标
 * @retval 像素数据
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
	LCD_BL_ON();	      //点亮LCD背光灯
	ILI9341_Rst ();
	ILI9341_REG_Config ();
}


void LCD_ShowChar(uint16_t usX,uint16_t usY,const char cChar,uint16_t usColor_Foreground,uint16_t usColor_Background)
{
	uint8_t ucPage,ucTemp,usRelativePositon,ucColumn;
	
	usRelativePositon = cChar-' ';
	
	LCD_OpenWindow(usX,usY,8,16);
	LCD_WRITE_CMD(0x2C);	//写GRAM
	
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



