#include "touch.h" 
#include "gui.h"
#include "spi.h"
#include "math.h"
#include "spi_flash.h"

/*У׼��ʱ����Ļ�ߴ�Ĳ���*/
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
//Ĭ��Ϊtouchtype=0������.

/**********************
�������ƣ�TOUCH_ReadData
���ܣ�����ָ���ȡ12λADֵ
������cmd -----------����
����ֵ��totalValue ------��ȡ����ADֵ
*********************/
uint16_t TOUCH_ReadData(uint8_t cmd)
{
	uint8_t temp[2]={0};
	uint32_t totalValue=0;
	TP_CS_L();	//��Ƭѡ
	//��������
	HAL_SPI_Transmit(&hspi2,&cmd,1,0xffff);
	//���������12Ϊ��ת����� ���Ƕ�ȡ16λ ������λ�����ԾͿ�����
	HAL_SPI_TransmitReceive(&hspi2,&NOE_CMD,(uint8_t *)&temp,2,0xffff);
	TP_CS_H();	//�ر�Ƭѡ
	totalValue = temp[0];
	totalValue = (totalValue<<8)+temp[1];
	//ȥ��4λ��������
	totalValue >>= 3;	
	return  totalValue;
}

//��ȡһ������ֵ(x����y)
//������ȡREAD_TIMES������,����Щ������������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
//xy:ָ�ReadX_CMD/ReadY_CMD��
//����ֵ:����������
#define READ_TIMES 5 	//��ȡ����
#define LOST_VAL 1	  	//����ֵ
uint16_t TP_Read_XOY(uint8_t xy)
{
	uint16_t i, j;
	uint16_t buf[READ_TIMES];
	uint16_t sum=0;
	uint16_t temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TOUCH_ReadData(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
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
//��ȡx,y����
//��Сֵ��������100.
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
uint8_t TP_Read_XY(uint16_t *x,uint16_t *y)
{
	uint16_t xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(ReadX_CMD);
	ytemp=TP_Read_XOY(ReadY_CMD);	  												   
	//if(xtemp<100||ytemp<100)return 0;//����ʧ��
	*x=xtemp;
	*y=ytemp;
	return 1;//�����ɹ�
}
//����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���
//ERR_RANGE,��������,����Ϊ������ȷ,�����������.	   
//�ú����ܴ�����׼ȷ��
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
#define ERR_RANGE 50 //��Χ 
uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y) 
{
	uint16_t x1,y1;
 	uint16_t x2,y2;
 	uint8_t flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//ǰ�����β�����+-50��
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
} 

 
//////////////////////////////////////////////////////////////////////////////////		  
//��LCD�����йصĺ���  
//��һ��������
//����У׼�õ�
//x,y:����
//color:��ɫ
void TP_Drow_Touch_Point(uint16_t x,uint16_t y,GUI_COLOR color)
{
	GUI_SetColor(color);
	GUI_DrawLine(x-12,y,x+13,y);
	GUI_DrawLine(x,y-12,x,y+13);//����
	GUI_DrawPixel(x+1,y+1);
	GUI_DrawPixel(x-1,y+1);
	GUI_DrawPixel(x+1,y-1);
	GUI_DrawPixel(x-1,y-1);
	GUI_DrawCircle(x,y,6);//������Ȧ
}	  
//��һ�����(2*2�ĵ�)		   
//x,y:����
//color:��ɫ
void TP_Draw_Big_Point(uint16_t x,uint16_t y,GUI_COLOR color)
{	    
	GUI_SetColor(color);
	GUI_DrawPixel(x,y);//���ĵ� 
	GUI_DrawPixel(x+1,y);
	GUI_DrawPixel(x,y+1);
	GUI_DrawPixel(x+1,y+1);	 	  	
}	

//////////////////////////////////////////////////////////////////////////////////		  
//��������ɨ��
//tp:0,��Ļ����;1,��������(У׼�����ⳡ����)
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
uint8_t TP_Scan(uint8_t tp)
{			   
	if(TP_PEN==0)//�а�������
	{
		if(tp)TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//��ȡ��������
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//��ȡ��Ļ����
		{
	 		tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//�����ת��Ϊ��Ļ����
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff; 
//			GUI_SetColor(GUI_GREEN);
//			GUI_DrawPixel(tp_dev.x[0],tp_dev.y[0]);
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//֮ǰû�б�����
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//��������  
			tp_dev.x[4]=tp_dev.x[0];//��¼��һ�ΰ���ʱ������
			tp_dev.y[4]=tp_dev.y[0];  	   			 
		}	
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta&=~(1<<7);//��ǰ����ɿ�	
		}else//֮ǰ��û�б�����
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//���ص�ǰ�Ĵ���״̬
}	  

//��ʾ�ַ���
uint8_t* const TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
 					  
//��ʾУ׼���(��������)
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
	GUI_DispDecAt(x0,40+24,160,4);		//��ʾ��ֵ
	GUI_DispDecAt(y0,40+24+80,160,4);	//��ʾ��ֵ
	GUI_DispDecAt(x1,40+24,180,4);		//��ʾ��ֵ
	GUI_DispDecAt(y1,40+24+80,180,4);	//��ʾ��ֵ
	GUI_DispDecAt(x2,40+24,200,4);		//��ʾ��ֵ
	GUI_DispDecAt(y2,40+24+80,200,4);	//��ʾ��ֵ
	GUI_DispDecAt(x3,40+24,220,4);		//��ʾ��ֵ
	GUI_DispDecAt(y3,40+24+80,220,4);	//��ʾ��ֵ
 	GUI_DispDecAt(fac,40+56,240,3); 	//��ʾ��ֵ,����ֵ������95~105��Χ֮��.
}

//������У׼����
//�õ��ĸ�У׼����
void TP_Adjust(void)
{								 
	uint16_t pos_temp[4][2];//���껺��ֵ
	uint8_t  cnt=0;	
	uint16_t d1,d2;
	uint32_t tem1,tem2;
	double fac; 	
	uint16_t outtime=0;
 	cnt=0;	
	
	GUI_SetBkColor(GUI_WHITE);
	GUI_SetColor(GUI_BLUE);
	GUI_Clear();//����      
	GUI_DispStringAt((const char*)TP_REMIND_MSG_TBL,40,40);//��ʾ��ʾ��Ϣ
	TP_Drow_Touch_Point(20,20,GUI_RED);//����1 
	tp_dev.sta=0;//���������ź� 
	tp_dev.xfac=0;//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	 
	while(1)//�������10����û�а���,���Զ��˳�
	{
		tp_dev.scan(1);//ɨ����������
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//����������һ��(��ʱ�����ɿ���.)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);//��ǰ����Ѿ����������.
						   			   
			pos_temp[cnt][0]=tp_dev.x[0];
			pos_temp[cnt][1]=tp_dev.y[0];
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,GUI_WHITE);				//�����1 
					TP_Drow_Touch_Point(LCD_width-20,20,GUI_RED);	//����2
					break;
				case 2:
 					TP_Drow_Touch_Point(LCD_width-20,20,GUI_WHITE);	//�����2
					TP_Drow_Touch_Point(20,LCD_height-20,GUI_RED);	//����3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,LCD_height-20,GUI_WHITE);			//�����3
 					TP_Drow_Touch_Point(LCD_width-20,LCD_height-20,GUI_RED);	//����4
					break;
				case 4:	 //ȫ���ĸ����Ѿ��õ�
	    		    //�Ա����
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,2�ľ���
					printf("1,2�ľ���Ϊ:%d \r\n",d1);
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�3,4�ľ���
					printf("3,4�ľ���Ϊ:%d \r\n",d2);
					fac=(float)d1/d2;
					printf("fac:%f \r\n",fac);
					if(fac<0.95||fac>1.05||d1==0||d2==0)//���ϸ�
					{
						printf("1���ϸ�\r\n");
						cnt=0;
 				    	TP_Drow_Touch_Point(LCD_width-20,LCD_height-20,GUI_WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,GUI_RED);								//����1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,3�ľ���
					printf("1,3�ľ���Ϊ:%d \r\n",d1);
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,4�ľ���
					printf("2,4�ľ���Ϊ:%d \r\n",d2);
					fac=(float)d1/d2;
					printf("fac:%f \r\n",fac);
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						printf("2���ϸ�\r\n");
						cnt=0;
 				    	TP_Drow_Touch_Point(LCD_width-20,LCD_height-20,GUI_WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,GUI_RED);								//����1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//��ȷ��
								   
					//�Խ������
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,4�ľ���
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,3�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(LCD_width-20,LCD_height-20,GUI_WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,GUI_RED);								//����1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//��ȷ��
					//������
					tp_dev.xfac=(float)(LCD_width-40)/(pos_temp[1][0]-pos_temp[0][0]);//�õ�xfac		 
					tp_dev.xoff=(LCD_width-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//�õ�xoff
						  
					tp_dev.yfac=(float)(LCD_height-40)/(pos_temp[2][1]-pos_temp[0][1]);//�õ�yfac
					tp_dev.yoff=(LCD_height-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//�õ�yoff  
					
					printf("xfac��%f,yfac:%f \r\n",tp_dev.xfac,tp_dev.yfac);
					printf("xoff��%d,yfac:%d \r\n",tp_dev.xoff,tp_dev.yoff);
					
					if(abs((int)tp_dev.xfac)>2||abs((int)tp_dev.yfac)>2)//������Ԥ����෴��.
					{
						printf("������Ԥ����෴��.\r\n");
						cnt=0;
 				    	TP_Drow_Touch_Point(LCD_width-20,LCD_height-20,GUI_WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,GUI_RED);								//����1
						GUI_DispStringAt("TP Need readjust!",40,26);
						tp_dev.touchtype=!tp_dev.touchtype;//�޸Ĵ�������.
						if(tp_dev.touchtype)//X,Y��������Ļ�෴
						{
							ReadX_CMD=0X90;
							ReadY_CMD=0XD0;	 
						}else				   //X,Y��������Ļ��ͬ
						{
							ReadX_CMD=0XD0;
							ReadY_CMD=0X90;	 
						}			    
						continue;
					}		
					GUI_SetColor(GUI_BLUE);
					GUI_Clear();//����
					GUI_DispStringAt("Touch Screen Adjust OK!",35,110);//У�����
					HAL_Delay(1000);
					TP_Save_Adjdata();  		//�������
 					GUI_Clear();//����   
					return;//У�����				 
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


//��������ʼ��  		    
//����ֵ:0,û�н���У׼
//       1,���й�У׼
uint8_t TP_Init(void)
{	
	/*�˿ڵ������� ��Ҫ�������Ĵ�������ʵ�� ������cubemx���ɵĴ����� û����������Ĵ���*/
	/*���������õ�һ��TP_PEN�˿� Ϊ�������� ���������� ����һ������Ĵ��� ��ʵ����������*/
	HAL_GPIO_WritePin(TP_CS_GPIO_Port,TP_CS_Pin,GPIO_PIN_SET);
	TP_Read_XY(&tp_dev.x[0],&tp_dev.y[0]);//��һ�ζ�ȡ��ʼ��	 
	//AT24CXX_Init();			//��ʼ��24CXX
	if(TP_Get_Adjdata())return 0;//�Ѿ�У׼
	else			  		//δУ׼?
	{ 										    
		GUI_Clear();	//����
		TP_Adjust();  		//��ĻУ׼  
	}			
	TP_Get_Adjdata();	
	return 1; 									 
}


uint8_t my_TP_Adjust(void)
{
	uint8_t cnt = 0;
	uint16_t pos_temp[5][2];//���껺��ֵ
	
	tp_dev.sta=0;//���������ź� 
	tp_dev.xfac=0;//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	
	tp_dev.yfac=0;//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	
	
	TP_Drow_Touch_Point(20,20,GUI_RED);//����1 
	
	while(1)//�������10����û�а���,���Զ��˳�
	{
		tp_dev.scan(1);//ɨ����������
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//����������һ��(��ʱ�����ɿ���.)
		{		
			printf("x:%d,y:%d\r\n",tp_dev.x[0],tp_dev.y[0]);
			pos_temp[cnt][0]=tp_dev.y[0];
			pos_temp[cnt][1]=tp_dev.x[0];
			tp_dev.sta&=~(1<<6);//��ǰ����Ѿ����������.
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,GUI_WHITE);				//�����1 
					TP_Drow_Touch_Point(LCD_width-20,20,GUI_RED);	//����2
					break;
				case 2:
 					TP_Drow_Touch_Point(LCD_width-20,20,GUI_WHITE);	//�����2
					TP_Drow_Touch_Point(20,LCD_height-20,GUI_RED);	//����3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,LCD_height-20,GUI_WHITE);			//�����3
 					TP_Drow_Touch_Point(LCD_width-20,LCD_height-20,GUI_RED);	//����4
					break;
				case 4:	 //ȫ���ĸ����Ѿ��õ�
					TP_Drow_Touch_Point(20,LCD_height-20,GUI_WHITE);			//�����4
 					TP_Drow_Touch_Point(LCD_width/2,LCD_height/2,GUI_RED);	//����5
					break;
				case 5:	 //ȫ���ĸ����Ѿ��õ�
							/*��ʼ����*/
					tp_dev.xfac = ((float)((pos_temp[1][0]-pos_temp[0][0])+(pos_temp[3][0]-pos_temp[2][0]))/440);
					tp_dev.yfac = ((float)((pos_temp[1][1]-pos_temp[3][1])+(pos_temp[0][1]-pos_temp[2][1]))/560);
					tp_dev.xoff = pos_temp[4][0];
					tp_dev.yoff = pos_temp[4][1];
					printf("xfac:%f,yfac:%f\r\n",tp_dev.xfac,tp_dev.yfac);
					printf("xoff:%d,y0ff:%d\r\n",tp_dev.xoff,tp_dev.yoff);
					GUI_Clear();
					GUI_DispStringAt("TP_Adjust OK!",LCD_width/2,LCD_height/2);
					return 1;	//�ɹ�
					break;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////		  
//��������ɨ��
//tp:0,��Ļ����;1,��������(У׼�����ⳡ����)
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
uint8_t my_TP_Scan(uint8_t tp)
{			   
	if(TP_PEN==0)//�а�������
	{
		if(tp)
		{
			TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//��ȡ��������
			printf("��������x:%d,y:%d\r\n",tp_dev.x[0],tp_dev.y[0]);
		}
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//��ȡ��Ļ����
		{
	 		tp_dev.x[0]=((tp_dev.x[0]-tp_dev.xoff)/tp_dev.xfac + 120);//�����ת��Ϊ��Ļ����
			tp_dev.y[0]=((tp_dev.y[0]-tp_dev.yoff)/tp_dev.yfac + 160);  
			printf("��Ļ����x:%d,y:%d\r\n",tp_dev.y[0],tp_dev.x[0]);
			GUI_SetColor(GUI_GREEN);
			GUI_DrawPixel(tp_dev.y[0],240 - tp_dev.x[0]);
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//֮ǰû�б�����
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//��������  
			tp_dev.x[4]=tp_dev.x[0];//��¼��һ�ΰ���ʱ������
			tp_dev.y[4]=tp_dev.y[0];  	   			 
		}	
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta&=~(1<<7);//��ǰ����ɿ�	
		}else//֮ǰ��û�б�����
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//���ص�ǰ�Ĵ���״̬
}	 

////������EEPROM����ĵ�ַ�����ַ,ռ��14���ֽ�(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+13)
#define  SAVE_ADDR_BASE     0x00040
#define  FLASH_ReadAddress      SAVE_ADDR_BASE
#define  FLASH_SectorToErase    SAVE_ADDR_BASE
uint8_t flash_crc = 0x0A;
////����У׼����										    
void TP_Save_Adjdata(void)
{
//	W25Q16_Write(SAVE_ADDR_BASE,(uint8_t*)&tp_dev.xfac,14);	//ǿ�Ʊ���&tp_dev.xfac��ַ��ʼ��14���ֽ����ݣ������浽tp_dev.touchtype
// 	W25Q16_WriteOneByte(SAVE_ADDR_BASE+14,0X0A);		//�����д0X0A���У׼����
		SPI_FLASH_SectorErase(FLASH_SectorToErase);			//д��֮ǰ��Ҫ����
		SPI_FLASH_BufferWrite((u8*)&tp_dev.xfac, SAVE_ADDR_BASE, 14);//ǿ�Ʊ���&tp_dev.xfac��ַ��ʼ��14���ֽ����ݣ������浽tp_dev.touchtype
		SPI_FLASH_BufferWrite(&flash_crc, SAVE_ADDR_BASE+14, 1);		//�����д0X0A���У׼����
}
//�õ�������EEPROM�����У׼ֵ
//����ֵ��1���ɹ���ȡ����
//        0����ȡʧ�ܣ�Ҫ����У׼
uint8_t TP_Get_Adjdata(void)
{					  
	uint8_t temp;
	SPI_FLASH_BufferRead(&temp,SAVE_ADDR_BASE+14,1);//��ȡ�����,���Ƿ�У׼���� 		 
	if(temp==0X0A)//�������Ѿ�У׼����			   
 	{ 
		SPI_FLASH_BufferRead((u8*)&tp_dev.xfac,SAVE_ADDR_BASE,14);//��ȡ֮ǰ�����У׼���� 
		if(tp_dev.touchtype)//X,Y��������Ļ�෴
		{
			ReadX_CMD=0XD0;
			ReadY_CMD=0X90;	 
		}else				   //X,Y��������Ļ��ͬ
		{
			ReadX_CMD=0X90;
			ReadY_CMD=0XD0;	 
		}		 
		return 1;	 
	}
	return 0;
}







