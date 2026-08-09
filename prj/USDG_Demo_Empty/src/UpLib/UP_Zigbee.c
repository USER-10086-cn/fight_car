/******************************************************************/
//	Copyright (C), 2011-2013, 北京博创 
//  Author   	  : 乔潇楠  
//  Update Date   : 2013/07/15
//  Version   	  : 1.0            
//  Description   : 
/******************************************************************/ 

#include "UP_Zigbee.h" 
#include "UP_UART.h"
#include "UP_Globle.h"
#include "UP_UART_Parse.h"
#include "UP_Variable.h"
#include "UP_Timer.h"
 u8 zigbee_tx_buffer[5]={0}; /***zigbee 发送数据缓冲区**/
/*******************************************/
u8   const	zigbee_conf[3][8]=
		{
		 {7,0xfe,0xcc,0x02,0x27,0x02,PANID_L,PANID_H},  /********设置zigbee的 PANID    *******/
		 {6,0xfe,0xcc,0x01,0x27,0x04,DEVICE_TYPE},	     /********设置zigbee的 设备类型 *******/
		 {6,0xfe,0xcc,0x01,0x21,0x09,DEVICE_ID}			/*********设置zigbee的 ID 号 *********/
		}	;

/*******************************************************
*		 通过串口来配置 zigbee设备
*
*
*******************************************************/

void zigbee_init(void)
{
  u8 count=0,i=0;
	do{
			while(count++<zigbee_conf[i][0])			 
			{
		  	UP_UART4_Putc(zigbee_conf[i][count]);				
			}	
		  count=0;
		  UP_delay_ms(100);
	  }
 	while(++i<3);

}

extern void TimeHadler(u32 timechannel);
//UP_UART_FRAME
//使能Zigbee数据接收中断
void UP_Zigbee_EnableIT(void)
{
	g_UP_bZigbeeIT = TRUE;	
	UP_SetZigbeeHadler(UP_Zigbee_Rec);
	UP_Timer_SetHadler(TIMER_CHANNEL1,TimeHadler);			 			 //传递计时器中断入口函数指针 	
}

//传递Zigbee数据接收中断入口函数
void UP_SetZigbeeHadler(void (*ApplicationAddress)(u32))
{
	if(ApplicationAddress != 0)
		g_UP_ZigbeeITAddress = (u32)ApplicationAddress;
}

//Zigbee发送一个字符
void UP_Zigbee_Putc(unsigned char c)
{
	UP_UART4_Putc(c);
} 

//Zigbee发送一串字符	
void UP_Zigbee_Puts(char * str)
{
	UP_UART4_Puts(str);
}		





u8 UP_Zigbee_RecBuf[80];
u8 UP_Zigbee_RecSign;
u32 UP_Zigbee_RecNum;
u32 UP_Zigbee_TxNum;
u8 zigbee_receive_num=0; /**其他函数中用此变量提取数据**/
u8 zigbee_rec_count=0;

u8 Zigbee_RecTime_Count = 0;
UP_UART_FRAME Zigbee_Frame;
u8 zigbee_buf[9];

//  			UP_Zigbee_RecBuf[0] = 0xFE;
//  			UP_Zigbee_RecBuf[1] = data;
// 			Zigbee_RecTime_Count = 5;
// 		}
// 		else
// 		{
// 			last_data = data;	
// 		}
// 	}
// 	else
// 	{
// 		if(Zigbee_RecTime_Count!=0)
// 		{
// 			Zigbee_RecTime_Count = 5;
// 			if(2 == index)	
// 			{	
// 				len = UP_Zigbee_RecBuf[2] = data;
// 				sum = 0xd8+len;
// 				index = 3;
// 			}
// 			else
// 			{
// 				if(index == (len-1))
// 				{
// 					if(sum==data)
// 					{
// 						UP_Zigbee_RecBuf[index] = data;
// 						Zigbee_Frame.DataBuf[Zigbee_Frame.Head].len = UP_Zigbee_RecBuf[2];
// 						Zigbee_Frame.DataBuf[Zigbee_Frame.Head].cmd = UP_Zigbee_RecBuf[5];	
// 						for(i=0;i<len;i++)
// 						{
// 							Zigbee_Frame.DataBuf[Zigbee_Frame.Head].data[i]=UP_Zigbee_RecBuf[i];	
// 							UP_Zigbee_RecBuf[i]=0;
// 						}					
// 						
// 						Zigbee_Frame.Head+=1;	
// 						if(Zigbee_Frame.Head > UP_UART_FRAMENUM_MAX)
// 							Zigbee_Frame.Head = 0;		
// 					}
// 					len=0;
// 					index=0;
// 					FrameStart = 0;	
// 					sum = 0;					
// 					//UP_Zigbee_RecNum++;
// 				}
// 				else
// 				{
// 					UP_Zigbee_RecBuf[index++] = data;
// 					sum+=data;
// 				}
// 			}
// 		}
// 		else
// 		{
// 			len=0;
// 			index=0;
// 			FrameStart = 0;	
// 			sum = 0;				
// 		}
// 	}
// }
void UP_Push_TxData(u8 S_ID,u8 D_ID,u8 len,u8 cmd,u8 *data)
{
	u8 i,sum;
	//Zigbee_Frame.DataBuf[Zigbee_Frame.Head].data
// 	UP_Zigbee_Putc(0xfe);
// 	UP_Zigbee_Putc(0xda);
// 	UP_Zigbee_Putc(len+7);
// 	UP_Zigbee_Putc(S_ID);
// 	UP_Zigbee_Putc(D_ID);
// 	UP_Zigbee_Putc(cmd);
// 	sum = 0xfe+0xda+7+len+S_ID+D_ID+cmd;
// 	for(i=0;i<len;i++)
// 	{
// 		UP_Zigbee_Putc(data[i]);	
// 		sum+=data[i];
// 	}
// 	UP_Zigbee_Putc(sum);
	UP_Zigbee_TxNum++;
	Zigbee_Frame.DataBuf[Zigbee_Frame.Head].data[0] = 0xfe;
	Zigbee_Frame.DataBuf[Zigbee_Frame.Head].data[1] = 0xda;
	Zigbee_Frame.DataBuf[Zigbee_Frame.Head].data[2] = len+7;
	Zigbee_Frame.DataBuf[Zigbee_Frame.Head].data[3] = S_ID;
	Zigbee_Frame.DataBuf[Zigbee_Frame.Head].data[4] = D_ID;
	Zigbee_Frame.DataBuf[Zigbee_Frame.Head].data[5] = cmd;
	sum = 0xfe;
	sum+=0xda+7+len+S_ID+D_ID+cmd;
	for(i=0;i<len;i++)
	{
		Zigbee_Frame.DataBuf[Zigbee_Frame.Head].data[6+i] = data[i];	
		sum+=data[i];
	}
	Zigbee_Frame.DataBuf[Zigbee_Frame.Head].data[len+6] = sum;
	
	Zigbee_Frame.Head++;
	if(Zigbee_Frame.Head == UP_UART_FRAMENUM_MAX)
		Zigbee_Frame.Head = 0;
    


	
}
void Check_Tx_Frame(UP_UART_FRAME *Tx_Frame)
{
	u8 i,len;
	
	if(Tx_Frame->Tail != Tx_Frame->Head)
	{
		len = Tx_Frame->DataBuf[Tx_Frame->Tail].data[2];
		for(i=0;i<len;i++)
			UP_Zigbee_Putc(Tx_Frame->DataBuf[Tx_Frame->Tail].data[i]);
		Tx_Frame->Tail++;
		if(Tx_Frame->Tail == UP_UART_FRAMENUM_MAX)
			Tx_Frame->Tail = 0;
		
	}
		
}
u8 Sensor[8];

void UP_Zigbee_Rec(u32 data)
{
   	
	
	UP_Zigbee_RecBuf[zigbee_rec_count++] = (u8)data;
	UP_Timer_EnableIT(TIMER_CHANNEL1,9000);		 	 //使能计时器2，计时时间9ms
	UP_Timer_Init();  
}
void Zigbee_TIMER_handler(u32 null)//
{
	static u8 count;
	if(Zigbee_RecTime_Count>0) Zigbee_RecTime_Count--;
	if(count == 5)
	{
		Check_Tx_Frame(&Zigbee_Frame);		
		count = 0;
	}
	else
		count++;
}

void UP_Zigbee_SetID(u8 ID)
{
	UP_Zigbee_Putc(0xfe);
	UP_Zigbee_Putc(0xcc);
	UP_Zigbee_Putc(0x01);
	UP_Zigbee_Putc(0x21);
	UP_Zigbee_Putc(0x09);
	UP_Zigbee_Putc(ID);
}
void UP_Zigbee_SetDeviceType(u8 type)//0-协调器     1-路由器
{
	UP_Zigbee_Putc(0xfe);
	UP_Zigbee_Putc(0xcc);
	UP_Zigbee_Putc(0x01);
	UP_Zigbee_Putc(0x27);
	UP_Zigbee_Putc(0x04);
	UP_Zigbee_Putc(type);	
}
void UP_Zigbee_RequireSensor(u8 S_ID,u8 D_ID)
{
	UP_Zigbee_Putc(0xfe);
	UP_Zigbee_Putc(0xed);
	UP_Zigbee_Putc(0x06);
	UP_Zigbee_Putc(S_ID);
	UP_Zigbee_Putc(D_ID);	
	UP_Zigbee_Putc(0x01);
}
/*************************************************************
*  函数目的：  向其他zigbee设备传送数据
*  入口参数：  D_ID目的zigbee设备地址  len需要传送数据长度
*  			   data 待传数据的数组首地址
**************************************************************/
void UP_Zigbee_SendData(u8 D_ID,u8 len,u8 *data)	   
{
	u8 i;
	u8 sum;
//	if(len>(sizeof(data) / sizeof(data[0])))
//	{
//		return ;
//	}
	UP_Zigbee_Putc(0xfe);
	UP_Zigbee_Putc(0xda);
	UP_Zigbee_Putc(len+6);
	UP_Zigbee_Putc(DEVICE_ID);
	UP_Zigbee_Putc(D_ID);
	sum = 0xfe;
	sum+= 0xda;
	sum+=6+len+DEVICE_ID+D_ID;
	for(i=0;i<len;i++)
	{
		UP_Zigbee_Putc(data[i]);	
		sum+=data[i];
	}
	UP_Zigbee_Putc(sum);
}
void UP_Zigbee_GetConfig(void)
{
	UP_Zigbee_Putc(0xfe);
	UP_Zigbee_Putc(0xcc);
	UP_Zigbee_Putc(0x00);
	UP_Zigbee_Putc(0x27);	
	UP_Zigbee_Putc(0x00);
}
void UP_Zigbee_GetID(void)
{
	UP_Zigbee_Putc(0xfe);
	UP_Zigbee_Putc(0xcc);
	UP_Zigbee_Putc(0x03);
	UP_Zigbee_Putc(0x21);	
	UP_Zigbee_Putc(0x08);	
	UP_Zigbee_Putc(0x77);
	UP_Zigbee_Putc(0x07);
	UP_Zigbee_Putc(0x00);
	
}


