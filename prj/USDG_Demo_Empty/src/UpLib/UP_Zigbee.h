/******************************************************************/
//	Copyright (C), 2011-2013, 北京博创 
//  Author   	  : 陈中元  
//  Update Date   : 2011/06/12
//  Version   	  : 1.2            
//  Description   :  
/******************************************************************/

#ifndef ZIGBEE_H
#define ZIGBEE_H

#include "STM32Lib\\stm32f10x.h"
extern u8 UP_Zigbee_RecBuf[80];
/*************************************************************
*			 配置zigbee设备相关参数	   需要用户根据自身需求修改
*************************************************************/
#define  PANID_L			 0x07		 /**zigbee设备的网络名称 低字节 **/
#define	 PANID_H	         0x02		 /**zigbee设备的网络名称 高字节  卓越之星测试采用 12 **/

#define	 DEVICE_ID			  0       /******设置设备 ID 设置时必须注意 ：0x00地址对应zigbee的设备类型为协调器，路由器可设置为除 0x00 外的其他数*****/

#define  DEST_ID_1			  1		 /******传送数据的目的地址*****/
#define  DEST_ID_2			  2	
#define  DEST_ID_3			  3	
#define  DEST_ID_4			  4	
#define  DEST_ID_5			  5	
#define  DEST_ID_6			  6	
#define  DEST_ID_7			  7	

/**+++++++++++++++++++++++++++++++++++++++**/
#if(DEVICE_ID==0x00)
	#define 	DEVICE_TYPE   0x00	/**设备类型为协调器**/
#else 
  #define 	    DEVICE_TYPE   0x01	/**设备类型为路由器**/
#endif


typedef struct _UP_ZIGBEE_FRAME
{
	u8 frame_type;
	u8 frame_len;
	u8 *frame_data;
	u8 rec_sign;
	u32 rec_count;
}UP_ZIGBEE_FRAME;



extern u8 zigbee_rec_count;
extern u8 zigbee_receive_num;
extern UP_ZIGBEE_FRAME UP_Zigbee_RecFrame;
extern u8 zigbee_tx_buffer[5]; /***zigbee 发送数据缓冲区**/

//Zigbee相关函数
void zigbee_init(void);
void UP_Zigbee_EnableIT(void);					//使能Zigbee数据接收中断
void UP_SetZigbeeHadler(void (*ApplicationAddress)(u32));		//传递Zigbee数据接收中断入口函数
void UP_Zigbee_Putc(unsigned char c);		//Zigbee发送一个字符
void UP_Zigbee_Puts(char * str);				//Zigbee发送一串字符	
void UP_Zigbee_Rec(u32 data);
void UP_Zigbee_SetID(u8 ID);
void UP_Zigbee_SetDeviceType(u8 type);
extern void UP_Zigbee_RequireSensor(u8 S_ID,u8 D_ID);
void UP_Zigbee_SendData(u8 D_ID,u8 len,u8 *data);
void UP_Zigbee_GetConfig(void);
void UP_Zigbee_GetID(void);
extern void Zigbee_TIMER_handler(u32 null);
extern void UP_Push_TxData(u8 S_ID,u8 D_ID,u8 len,u8 cmd,u8 *data);
#endif
