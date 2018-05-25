#include "touch.h" 
#include "gui.h"
#include "spi.h"
#include "math.h"
#include "spi_flash.h"

/*校准的时候屏幕尺寸的参数*/
#define LCD_width			320
#define LCD_height		240

uint8_t NOE_CMD	= 0xff;

uint8_t ReadX_CMD=0X90;
uint8_t ReadY_CMD=0Xd0;
_m_tp_dev tp_dev=
{
	TP_Init,
	TP_Scan,
	TP_Adjust,
	0,
	0, 
	0,
	0,
	0,
	0,	  	 		
	0,
	0,	  	 		
};					
//默认为touchtype=0的数据.

/**********************
函数名称：TOUCH_ReadData
功能：根据指令读取12位AD值
参数：cmd -----------命令
返回值：totalValue ------读取到的AD值
*********************/
uint16_t TOUCH_ReadData(uint8_t cmd)
{
	uint8_t temp[2]={0};
	uint32_t totalValue=0;
	TP_CS_L();	//打开片选
	//发送命令
	HAL_SPI_Transmit(&hspi2,&cmd,1,0xffff);
	//接下来输出12为的转换结果 我们读取16位 后面三位最后忽略就可以了
	HAL_SPI_TransmitReceive(&hspi2,&NOE_CMD,(uint8_t *)&temp,2,0xffff);
	TP_CS_H();	//关闭片选
	totalValue = temp[0];
	totalValue = (totalValue<<8)+temp[1];
	//去掉4位无用数据
	totalValue >>= 3;	
	return  totalValue;
}

//读取一个坐标值(x或者y)
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值 
//xy:指令（ReadX_CMD/ReadY_CMD）
//返回值:读到的数据
#define READ_TIMES 5 	//读取次数
#define LOST_VAL 1	  	//丢弃值
uint16_t TP_Read_XOY(uint8_t xy)
{
	uint16_t i, j;
	uint16_t buf[READ_TIMES];
	uint16_t sum=0;
	uint16_t temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TOUCH_ReadData(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
//读取x,y坐标
//最小值不能少于100.
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
uint8_t TP_Read_XY(uint16_t *x,uint16_t *y)
{
	uint16_t xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(ReadX_CMD);
	ytemp=TP_Read_XOY(ReadY_CMD);	  												   
	//if(xtemp<100||ytemp<100)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;//读数成功
}
//连续2次读取触摸屏IC,且这两次的偏差不能超过
//ERR_RANGE,满足条件,则认为读数正确,否则读数错误.	   
//该函数能大大提高准确度
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
#define ERR_RANGE 50 //误差范围 
uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y) 
{
	uint16_t x1,y1;
 	uint16_t x2,y2;
 	uint8_t flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
} 

 
//////////////////////////////////////////////////////////////////////////////////		  
//与LCD部分有关的函数  
//画一个触摸点
//用来校准用的
//x,y:坐标
//color:颜色
void TP_Drow_Touch_Point(uint16_t x,uint16_t y,GUI_COLOR color)
{
	GUI_SetColor(color);
	GUI_DrawLine(x-12,y,x+13,y);
	GUI_DrawLine(x,y-12,x,y+13);//竖线
	GUI_DrawPixel(x+1,y+1);
	GUI_DrawPixel(x-1,y+1);
	GUI_DrawPixel(x+1,y-1);
	GUI_DrawPixel(x-1,y-1);
	GUI_DrawCircle(x,y,6);//画中心圈
}	  
//画一个大点(2*2的点)		   
//x,y:坐标
//color:颜色
void TP_Draw_Big_Point(uint16_t x,uint16_t y,GUI_COLOR color)
{	    
	GUI_SetColor(color);
	GUI_DrawPixel(x,y);//中心点 
	GUI_DrawPixel(x+1,y);
	GUI_DrawPixel(x,y+1);
	GUI_DrawPixel(x+1,y+1);	 	  	
}	

//////////////////////////////////////////////////////////////////////////////////		  
//触摸按键扫描
//tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
uint8_t TP_Scan(uint8_t tp)
{			   
	if(TP_PEN==0)//有按键按下
	{
		if(tp)TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//读取物理坐标
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//读取屏幕坐标
		{
	 		tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//将结果转换为屏幕坐标
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff; 
//			GUI_SetColor(GUI_GREEN);
//			GUI_DrawPixel(tp_dev.x[0],tp_dev.y[0]);
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//之前没有被按下
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//按键按下  
			tp_dev.x[4]=tp_dev.x[0];//记录第一次按下时的坐标
			tp_dev.y[4]=tp_dev.y[0];  	   			 
		}	
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//之前是被按下的
		{
			tp_dev.sta&=~(1<<7);//标记按键松开	
		}else//之前就没有被按下
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//返回当前的触屏状态
}	  

//提示字符串
uint8_t* const TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
 					  
//提示校准结果(各个参数)
void TP_Adj_Info_Show(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t x3,uint16_t y3,uint16_t fac)
{	  
	GUI_SetColor(GUI_RED);
	GUI_DispStringAt("x1:",40,160);
 	GUI_DispStringAt("y1:",40+80,160);
 	GUI_DispStringAt("x2:",40,180);
 	GUI_DispStringAt("y2:",40+80,180);
	GUI_DispStringAt("x3:",40,200);
 	GUI_DispStringAt("y3:",40+80,200);
	GUI_DispStringAt("x4:",40,220);
 	GUI_DispStringAt("y4:",40+80,220);  
 	GUI_DispStringAt("fac is:",40,240);     
	GUI_DispDecAt(x0,40+24,160,4);		//显示数值
	GUI_DispDecAt(y0,40+24+80,160,4);	//显示数值
	GUI_DispDecAt(x1,40+24,180,4);		//显示数值
	GUI_DispDecAt(y1,40+24+80,180,4);	//显示数值
	GUI_DispDecAt(x2,40+24,200,4);		//显示数值
	GUI_DispDecAt(y2,40+24+80,200,4);	//显示数值
	GUI_DispDecAt(x3,40+24,220,4);		//显示数值
	GUI_DispDecAt(y3,40+24+80,220,4);	//显示数值
 	GUI_DispDecAt(fac,40+56,240,3); 	//显示数值,该数值必须在95~105范围之内.
}

//触摸屏校准代码
//得到四个校准参数
void TP_Adjust(void)
{								 
	uint16_t pos_temp[4][2];//坐标缓存值
	uint8_t  cnt=0;	
	uint16_t d1,d2;
	uint32_t tem1,tem2;
	double fac; 	
	uint16_t outtime=0;
 	cnt=0;	
	
	GUI_SetBkColor(GUI_WHITE);
	GUI_SetColor(GUI_BLUE);
	GUI_Clear();//清屏      
	GUI_DispStringAt((const char*)TP_REMIND_MSG_TBL,40,40);//显示提示信息
	TP_Drow_Touch_Point(20,20,GUI_RED);//画点1 
	tp_dev.sta=0;//消除触发信号 
	tp_dev.xfac=0;//xfac用来标记是否校准过,所以校准之前必须清掉!以免错误	 
	while(1)//如果连续10秒钟没有按下,则自动退出
	{
		tp_dev.scan(1);//扫描物理坐标
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//按键按下了一次(此时按键松开了.)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);//标记按键已经被处理过了.
						   			   
			pos_temp[cnt][0]=tp_dev.x[0];
			pos_temp[cnt][1]=tp_dev.y[0];
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,GUI_WHITE);				//清除点1 
					TP_Drow_Touch_Point(LCD_width-20,20,GUI_RED);	//画点2
					break;
				case 2:
 					TP_Drow_Touch_Point(LCD_width-20,20,GUI_WHITE);	//清除点2
					TP_Drow_Touch_Point(20,LCD_height-20,GUI_RED);	//画点3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,LCD_height-20,GUI_WHITE);			//清除点3
 					TP_Drow_Touch_Point(LCD_width-20,LCD_height-20,GUI_RED);	//画点4
					break;
				case 4:	 //全部四个点已经得到
	    		    //对边相等
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,2的距离
					printf("1,2的距离为:%d \r\n",d1);
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到3,4的距离
					printf("3,4的距离为:%d \r\n",d2);
					fac=(float)d1/d2;
					printf("fac:%f \r\n",fac);
					if(fac<0.95||fac>1.05||d1==0||d2==0)//不合格
					{
						printf("1不合格\r\n");
						cnt=0;
 				    	TP_Drow_Touch_Point(LCD_width-20,LCD_height-20,GUI_WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,GUI_RED);								//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,3的距离
					printf("1,3的距离为:%d \r\n",d1);
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,4的距离
					printf("2,4的距离为:%d \r\n",d2);
					fac=(float)d1/d2;
					printf("fac:%f \r\n",fac);
					if(fac<0.95||fac>1.05)//不合格
					{
						printf("2不合格\r\n");
						cnt=0;
 				    	TP_Drow_Touch_Point(LCD_width-20,LCD_height-20,GUI_WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,GUI_RED);								//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
						continue;
					}//正确了
								   
					//对角线相等
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//得到1,4的距离
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//得到2,3的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(LCD_width-20,LCD_height-20,GUI_WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,GUI_RED);								//画点1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
						continue;
					}//正确了
					//计算结果
					tp_dev.xfac=(float)(LCD_width-40)/(pos_temp[1][0]-pos_temp[0][0]);//得到xfac		 
					tp_dev.xoff=(LCD_width-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//得到xoff
						  
					tp_dev.yfac=(float)(LCD_height-40)/(pos_temp[2][1]-pos_temp[0][1]);//得到yfac
					tp_dev.yoff=(LCD_height-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//得到yoff  
					
					printf("xfac：%f,yfac:%f \r\n",tp_dev.xfac,tp_dev.yfac);
					printf("xoff：%d,yfac:%d \r\n",tp_dev.xoff,tp_dev.yoff);
					
					if(abs((int)tp_dev.xfac)>2||abs((int)tp_dev.yfac)>2)//触屏和预设的相反了.
					{
						printf("触屏和预设的相反了.\r\n");
						cnt=0;
 				    	TP_Drow_Touch_Point(LCD_width-20,LCD_height-20,GUI_WHITE);	//清除点4
   	 					TP_Drow_Touch_Point(20,20,GUI_RED);								//画点1
						GUI_DispStringAt("TP Need readjust!",40,26);
						tp_dev.touchtype=!tp_dev.touchtype;//修改触屏类型.
						if(tp_dev.touchtype)//X,Y方向与屏幕相反
						{
							ReadX_CMD=0X90;
							ReadY_CMD=0XD0;	 
						}else				   //X,Y方向与屏幕相同
						{
							ReadX_CMD=0XD0;
							ReadY_CMD=0X90;	 
						}			    
						continue;
					}		
					GUI_SetColor(GUI_BLUE);
					GUI_Clear();//清屏
					GUI_DispStringAt("Touch Screen Adjust OK!",35,110);//校正完成
					HAL_Delay(1000);
					TP_Save_Adjdata();  		//保存参数
 					GUI_Clear();//清屏   
					return;//校正完成				 
			}
		}
		HAL_Delay(10);
		outtime++;
		if(outtime>1000)
		{
			TP_Get_Adjdata();
			break;
	 	} 
 	}
}	


//触摸屏初始化  		    
//返回值:0,没有进行校准
//       1,进行过校准
uint8_t TP_Init(void)
{	
	/*端口的上下拉 需要配合输出寄存器才能实现 但是用cubemx生成的代码中 没有设置输出寄存器*/
	/*这里我们用到一个TP_PEN端口 为上拉输入 所以在这里 设置一下输出寄存器 以实现上拉输入*/
	HAL_GPIO_WritePin(TP_CS_GPIO_Port,TP_CS_Pin,GPIO_PIN_SET);
	TP_Read_XY(&tp_dev.x[0],&tp_dev.y[0]);//第一次读取初始化	 
	//AT24CXX_Init();			//初始化24CXX
	if(TP_Get_Adjdata())return 0;//已经校准
	else			  		//未校准?
	{ 										    
		GUI_Clear();	//清屏
		TP_Adjust();  		//屏幕校准  
	}			
	TP_Get_Adjdata();	
	return 1; 									 
}


uint8_t my_TP_Adjust(void)
{
	uint8_t cnt = 0;
	uint16_t pos_temp[5][2];//坐标缓存值
	
	tp_dev.sta=0;//消除触发信号 
	tp_dev.xfac=0;//xfac用来标记是否校准过,所以校准之前必须清掉!以免错误	
	tp_dev.yfac=0;//xfac用来标记是否校准过,所以校准之前必须清掉!以免错误	
	
	TP_Drow_Touch_Point(20,20,GUI_RED);//画点1 
	
	while(1)//如果连续10秒钟没有按下,则自动退出
	{
		tp_dev.scan(1);//扫描物理坐标
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//按键按下了一次(此时按键松开了.)
		{		
			printf("x:%d,y:%d\r\n",tp_dev.x[0],tp_dev.y[0]);
			pos_temp[cnt][0]=tp_dev.y[0];
			pos_temp[cnt][1]=tp_dev.x[0];
			tp_dev.sta&=~(1<<6);//标记按键已经被处理过了.
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,GUI_WHITE);				//清除点1 
					TP_Drow_Touch_Point(LCD_width-20,20,GUI_RED);	//画点2
					break;
				case 2:
 					TP_Drow_Touch_Point(LCD_width-20,20,GUI_WHITE);	//清除点2
					TP_Drow_Touch_Point(20,LCD_height-20,GUI_RED);	//画点3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,LCD_height-20,GUI_WHITE);			//清除点3
 					TP_Drow_Touch_Point(LCD_width-20,LCD_height-20,GUI_RED);	//画点4
					break;
				case 4:	 //全部四个点已经得到
					TP_Drow_Touch_Point(20,LCD_height-20,GUI_WHITE);			//清除点4
 					TP_Drow_Touch_Point(LCD_width/2,LCD_height/2,GUI_RED);	//画点5
					break;
				case 5:	 //全部四个点已经得到
							/*开始计数*/
					tp_dev.xfac = ((float)((pos_temp[1][0]-pos_temp[0][0])+(pos_temp[3][0]-pos_temp[2][0]))/440);
					tp_dev.yfac = ((float)((pos_temp[1][1]-pos_temp[3][1])+(pos_temp[0][1]-pos_temp[2][1]))/560);
					tp_dev.xoff = pos_temp[4][0];
					tp_dev.yoff = pos_temp[4][1];
					printf("xfac:%f,yfac:%f\r\n",tp_dev.xfac,tp_dev.yfac);
					printf("xoff:%d,y0ff:%d\r\n",tp_dev.xoff,tp_dev.yoff);
					GUI_Clear();
					GUI_DispStringAt("TP_Adjust OK!",LCD_width/2,LCD_height/2);
					return 1;	//成功
					break;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////		  
//触摸按键扫描
//tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
uint8_t my_TP_Scan(uint8_t tp)
{			   
	if(TP_PEN==0)//有按键按下
	{
		if(tp)
		{
			TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//读取物理坐标
			printf("物理坐标x:%d,y:%d\r\n",tp_dev.x[0],tp_dev.y[0]);
		}
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//读取屏幕坐标
		{
	 		tp_dev.x[0]=((tp_dev.x[0]-tp_dev.xoff)/tp_dev.xfac + 120);//将结果转换为屏幕坐标
			tp_dev.y[0]=((tp_dev.y[0]-tp_dev.yoff)/tp_dev.yfac + 160);  
			printf("屏幕坐标x:%d,y:%d\r\n",tp_dev.y[0],tp_dev.x[0]);
			GUI_SetColor(GUI_GREEN);
			GUI_DrawPixel(tp_dev.y[0],240 - tp_dev.x[0]);
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//之前没有被按下
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//按键按下  
			tp_dev.x[4]=tp_dev.x[0];//记录第一次按下时的坐标
			tp_dev.y[4]=tp_dev.y[0];  	   			 
		}	
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//之前是被按下的
		{
			tp_dev.sta&=~(1<<7);//标记按键松开	
		}else//之前就没有被按下
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//返回当前的触屏状态
}	 

////保存在EEPROM里面的地址区间基址,占用14个字节(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+13)
#define  SAVE_ADDR_BASE     0x00040
#define  FLASH_ReadAddress      SAVE_ADDR_BASE
#define  FLASH_SectorToErase    SAVE_ADDR_BASE
uint8_t flash_crc = 0x0A;
////保存校准参数										    
void TP_Save_Adjdata(void)
{
//	W25Q16_Write(SAVE_ADDR_BASE,(uint8_t*)&tp_dev.xfac,14);	//强制保存&tp_dev.xfac地址开始的14个字节数据，即保存到tp_dev.touchtype
// 	W25Q16_WriteOneByte(SAVE_ADDR_BASE+14,0X0A);		//在最后，写0X0A标记校准过了
		SPI_FLASH_SectorErase(FLASH_SectorToErase);			//写入之前需要擦除
		SPI_FLASH_BufferWrite((u8*)&tp_dev.xfac, SAVE_ADDR_BASE, 14);//强制保存&tp_dev.xfac地址开始的14个字节数据，即保存到tp_dev.touchtype
		SPI_FLASH_BufferWrite(&flash_crc, SAVE_ADDR_BASE+14, 1);		//在最后，写0X0A标记校准过了
}
//得到保存在EEPROM里面的校准值
//返回值：1，成功获取数据
//        0，获取失败，要重新校准
uint8_t TP_Get_Adjdata(void)
{					  
	uint8_t temp;
	SPI_FLASH_BufferRead(&temp,SAVE_ADDR_BASE+14,1);//读取标记字,看是否校准过！ 		 
	if(temp==0X0A)//触摸屏已经校准过了			   
 	{ 
		SPI_FLASH_BufferRead((u8*)&tp_dev.xfac,SAVE_ADDR_BASE,14);//读取之前保存的校准数据 
		if(tp_dev.touchtype)//X,Y方向与屏幕相反
		{
			ReadX_CMD=0XD0;
			ReadY_CMD=0X90;	 
		}else				   //X,Y方向与屏幕相同
		{
			ReadX_CMD=0X90;
			ReadY_CMD=0XD0;	 
		}		 
		return 1;	 
	}
	return 0;
}







