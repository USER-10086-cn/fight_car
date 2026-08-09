/******************************************************************************** 
 * @file     UP_Woody_SpeechRecognition.c
 * @brief    语音识别功能模块 
 * @details  1：Variable is not standardized；2：some fuction used last version ,
 *this version,We will not delete them,because of i am not sure if they will be use.
 *1:这个版本规范了全局变量的使用2:预留未使用到的函数
 * @语音识别功能使用基本流程：第一步：系统初始化之前调用UP_Woody_Init();初始化Woody模块
 * @语音识别功能使用基本流程：第二步：调用UP_Woody_StartUp(0x01);启动Woody语音播放功能,(一个字节的第一位为1,0000 0001)
 * @语音识别功能使用基本流程：第三步：调用UP_Woody_SpeechRecLetIssue();(允许woody下发识别结果)
 * @语音识别功能的基本使用流程：第四步：判断识别结果UP_VoiceId
 * @author   nealwang
 * @date     18/12/2015 
 * @version  v1.0 
 * @par Copyright (c): UpTech 
 * @par History: version: v0.8 
 ********************************************************************************/  
#include"UP_System.h"

#include "UP_Globle.h"
#include "UP_LCD.h"
#include "UP_UART.h"
u8 UP_VoiceId=0;
u8 Flage = 0;				//接收标志
u8 Tab=0;
u8 Var=0;
static u8 num=0;
static u16 ID;
char UP_Woody_ComparisonTables[7]={0X01,0X02,0X03,0X04,0X05,0X06,0X07};
char TestCommand[5] = {0x55,0xAA,0x05,0x01,0x05};		
char  UP_Woody_StartSpeech[5]={0x55,0xAA,0x05,0x10,0x14};
char  UP_Woody_Read[5]={0x55,0xAA,0x05,0x12,0x16};
char  UP_Woody_Closing[5]={0x55,0xAA,0x05,0x11,0x15};
char  UP_Woody_ReturnData[6]={0x00};

/*************************************************
  函数名称: UP_Woody_Speech_ClearData
  函数功能: 清零UP_Woody_ReturnData[6]
  被本函数调用的函数: 无
  调用本函数的函数: 无    
  输入参数说明: 无
  输出参数说明: 无       
  函数返回值: 无
  其它说明: 无        
*************************************************/	
void UP_Woody_Speech_ClearData(void)
{
	for(Var=0;Var<6;Var++)
    {
			UP_Woody_ReturnData[Var]=0x00;
		}
}

/*************************************************
  函数名称: UP_Display_Return_Text
  函数功能: 显示测试通信时返回的数据
  被本函数调用的函数: void UP_LCD_ShowHex(u8 x,u8 y,u8 hex);UP_delay_ms(u32 ms);
  调用本函数的函数: 无    
  输入参数说明: 无
  输出参数说明: 无       
  函数返回值: 无
  其它说明: 无        
*************************************************/	
void  UP_Display_Return_Text(void)
 {
	 
if(Flage==0x01)
{

	  UP_delay_ms(500);
	  UP_LCD_ShowHex(1,0,UP_Woody_ReturnData[0]);
		UP_LCD_ShowHex(3,0,UP_Woody_ReturnData[1]);
	
		UP_LCD_ShowHex(5,0,UP_Woody_ReturnData[2]);
		
		UP_LCD_ShowHex(7,0,UP_Woody_ReturnData[3]);

		UP_LCD_ShowHex(9,0,UP_Woody_ReturnData[4]);
		
		Flage=0;
   	UP_delay_ms(500);
	
   }		
}

/*************************************************
  函数名称: UP_Display_Return_Read
  函数功能: 显示读数据时返回的数据
  被本函数调用的函数: void UP_LCD_ShowHex(u8 x,u8 y,u8 hex);UP_delay_ms(u32 ms);
  调用本函数的函数: 无    
  输入参数说明: 无
  输出参数说明: 无       
  函数返回值: 无
  其它说明: 无        
*************************************************/
 void  UP_Display_Return_Read(void)
 {
	 if(Flage==0x01)
	 {
	  UP_delay_ms(1000);
	  UP_LCD_ShowHex(0,1,UP_Woody_ReturnData[0]);
		
		UP_LCD_ShowHex(2,1,UP_Woody_ReturnData[1]);
		
		UP_LCD_ShowHex(4,1,UP_Woody_ReturnData[2]);
		
		UP_LCD_ShowHex(6,1,UP_Woody_ReturnData[3]);
		
		UP_LCD_ShowHex(8,1,UP_Woody_ReturnData[4]);
		
		UP_LCD_ShowHex(10,1,UP_Woody_ReturnData[5]);
	 
		Flage=0;	
		UP_delay_ms(1000);
  }
}

/*************************************************
  函数名称: UP_Woody_Speech_ID_Judge
  函数功能: 语音ID号计算处理
  被本函数调用的函数: 无
  调用本函数的函数: 无    
  输入参数说明: 无
  输出参数说明: 无       
  函数返回值: ID,十进制数值形式，范围1~255。
  其它说明: 无        
*************************************************/
u16 UP_Woody_Speech_ID_Judge(void)
{
  ID=(UP_Woody_ReturnData[4]&0X0F)+((UP_Woody_ReturnData[4]&0XF0)>>4)*16;
	return ID;
}

/*************************************************
  函数名称: UP_Woody_Display_Speech_ID
  函数功能: 显示语音识别返回的ID
  被本函数调用的函数: UP_LCD_ClearScreen();
  调用本函数的函数: 无    
  输入参数说明: 无
  输出参数说明: 无       
  函数返回值: 无
  其它说明: 此函数显示在LCD的第三行，约3s显示一次，用户可更改显示时间。    
*************************************************/
 void UP_Woody_Display_Speech_ID(void)
  {
	 printf("\n");
	 printf("\n");
   printf("(S_ID)(%lud)\n",UP_VoiceId);
	 UP_delay_ms(3000);
	 UP_LCD_ClearScreen();
	}
	
	
/*************************************************
/*************************************************
  函数名称: UP_Woody_TestCommand
  函数功能: 测试LUBY与Woody之间的通信是否成功
  被本函数调用的函数: void UP_USR232_Putc(unsigned char c);
  调用本函数的函数: 无    
  输入参数说明: 无
  输出参数说明: 无       
  函数返回值: 无
  其它说明: 无        
*************************************************/
void UP_Woody_Speech_TestCommand(void)
{
	for(Tab=0;Tab<5;Tab++)
  {
	   UP_USR232_Putc(TestCommand[Tab]);
  }	
}



/*************************************************
  函数名称: UP_Woody_Speech_ReadData，暂时预留该函数
  函数功能: 发送读数据协议
  被本函数调用的函数: void UP_USR232_Putc(unsigned char c);
  调用本函数的函数: 无    
  输入参数说明: 无
  输出参数说明: 无       
  函数返回值: 无
  其它说明: 无     
*************************************************/
void UP_Woody_Speech_ReadData(void)
{
	
//   for(Tab=0;Tab<5;Tab++)
//    {
// 	   UP_USR232_Putc(UP_Woody_Read[Tab]);
//    }
//    UP_delay_ms(DELAY_TIMES);
//    revoic_flage =0; 	 
}
/**************************************
	函数名称: UP_Woody_SpeechRecLetIssue
  函数功能: 发送读数据协议
  被本函数调用的函数: void UP_USR232_Putc(unsigned char c);
  调用本函数的函数: 无    
  输入参数说明: 无
  输出参数说明: 无       
  函数返回值: 无
  其它说明: 无   
**************************************/
void UP_Woody_SpeechRecLetIssue(void){
// 	char str[5]={0x55,0xAA,0x05,0x10,0x14};
// 	for(lab=0;lab<5;lab++){
// 		UP_UART5_Putc(str[lab]);
// 	}
	UP_UART5_Putc(0x55);
	UP_UART5_Putc(0xaa);
	UP_UART5_Putc(0x05);
	UP_UART5_Putc(0x10);
	UP_UART5_Putc(0x14);
	
	UP_delay_ms(DELAY_TIMES);
}
/**************************************
	函数名称: UP_Woody_SpeechRecForbidIssue
  函数功能: 发送读数据协议
  被本函数调用的函数: void UP_USR232_Putc(unsigned char c);
  调用本函数的函数: 无    
  输入参数说明: 无
  输出参数说明: 无       
  函数返回值: 无
  其它说明: 无  
**************************************/
void UP_Woody_SpeechRecForbidIssue(void){
	//char str[5]={0x55,0xAA,0x05,0x11,0x15};
	//for(lab=0;lab<5;lab++){
	//	UP_UART5_Putc(str[lab]);
	//}
	UP_UART5_Putc(0x55);
	UP_UART5_Putc(0xaa);
	UP_UART5_Putc(0x05);
	UP_UART5_Putc(0x10);
	UP_UART5_Putc(0x14);
	UP_delay_ms(DELAY_TIMES);
}
