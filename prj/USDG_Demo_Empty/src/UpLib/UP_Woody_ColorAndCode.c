/******************************************************************************** 
 * @file     UP_Woody_ColorAndCode.c
 * @brief    颜色识别与条码识别
 * @details  1：Variable is not standardized；2：some fuction used last version ,
 *this version,We will not delete them,because of i am not sure if they will be use.
 *1:这个版本规范了全局变量的使用2:预留未使用到的函数
 * @颜色识别与条码识别流程一致，下面介绍放在一块
 * @颜色识别与条码识别基本流程：第一步：系统初始化之前调用UP_Woody_Init();初始化Woody模块
 * @颜色识别与条码识别基本流程：第二步：调用UP_Woody_StartUp(0x08)或UP_Woody_StartUp(0x10);启动Woody语音播放功能,(一个字节的第四或第五位为1,0000 1000或0001 0000)
 * @颜色识别与条码识别基本流程：第三步：调用UP_Woody_ColorRecLetIssue()(UP_Woody_BarCodeRecLetIssue());(允许woody下发识别结果)
 * @颜色识别与条码识别基本使用流程：第四步：判断识别结果UP_ColorId(或UP_ColorId)
 * @author   nealwang
 * @date     18/12/2015 
 * @version  v1.0 
 * @par Copyright (c): UpTech 
 * @par History: version: v0.8 
 ********************************************************************************/  
#include "STM32Lib\\stm32f10x.h"
#include"UP_System.h"
#include "UP_USR232.h" 
#include "UP_UART.h"
#include "UP_Globle.h"
#include "UP_Woody_ColorAndCode.h"

u8  UP_ColorId =0,UP_CodeId=0;
static u16 ID=0;
/****************************************************
起停控制
*****************************************************/

/**************************************
函数名称;UP_Woody_StartUp
函数功能:启动woody的相应功能
被本函数调用的函数:UP_USR232_Putc?UP_UART5_Putc
调用本函数的函数：无
输入参数说明:u32 command:范围0-255(实际范围0-35,请查看启停命令控制字)
输出参数说明：无
函数返回值：无
其它说明：无
**************************************/
void UP_Woody_StartUp(u32 command){
	u32 sum=0x55+0xaa+0x06+0x02;
	UP_UART5_Putc(0x55);
	UP_UART5_Putc(0xaa);
	UP_UART5_Putc(0x06);
	UP_UART5_Putc(0x02);
	UP_UART5_Putc(command);
	sum+=command;
	UP_UART5_Putc(sum);
	UP_delay_ms(DELAY_TIMES);
}
/******************************************************************
颜色识别模块 
******************************************************************/

/**************************************
函数名称；UP_Woody_ColorRecLetIssue
函数功能：颜色识别允许下发
被本函数调用的函数：UP_USR232_Putc或UP_UART5_Putc
调用本函数的函数：无
输入参数说明：无
输出参数说明：无
函数返回值：无
其它说明：无
**************************************/
void UP_Woody_ColorRecLetIssue(){
	//char str[5]={0x55,0xAA,0x05,0x40,0x44};
	UP_UART5_Putc(0x55);
	UP_UART5_Putc(0xaa);
	UP_UART5_Putc(0x05);
	UP_UART5_Putc(0x40);
	UP_UART5_Putc(0x44);
// 	for(lab=0;lab<5;lab++){
// 		UP_UART5_Putc(str[lab]);
// 	}
	UP_delay_ms(DELAY_TIMES);
}
/**************************************
函数名称；UP_Woody_ColorRecLetIssue
函数功能：颜色识别允许下发
被本函数调用的函数：UP_USR232_Putc或UP_UART5_Putc
调用本函数的函数：无
输入参数说明：无
输出参数说明：无
函数返回值：无
其它说明：无
**************************************/
void UP_Woody_ColorRecForbidIssue(){
	//char str[5]={0x55,0xAA,0x05,0x41,0x45};
	UP_UART5_Putc(0x55);
	UP_UART5_Putc(0xaa);
	UP_UART5_Putc(0x05);
	UP_UART5_Putc(0x41);
	UP_UART5_Putc(0x45);
// 	for(lab=0;lab<5;lab++){
// 		UP_UART5_Putc(str[lab]);
// 	}
	UP_delay_ms(DELAY_TIMES);
}
/**************************************
函数名称；UP_Woody_ColorRec_ID_Judge
函数功能：颜色识别返回的ID
被本函数调用的函数：UP_USR232_Putc或UP_UART5_Putc
调用本函数的函数：无
输入参数说明：无
输出参数说明：无
函数返回值：ID，十进制数值形式，范围0~255
其它说明：无
**************************************/
u16 UP_Woody_ColorRec_ID_Judge(void)
{
	ID=(UP_Woody_ReturnData[4]&0X0F)+((UP_Woody_ReturnData[4]&0XF0)>>4)*16;
	return ID;
}
/*************************************************
函数名称；UP_Woody_ColorRec_ID_Judge
函数功能：显示颜色识别返回的ID
被本函数调用的函数：无
调用本函数的函数：无
输入参数说明：无
输出参数说明：无
函数返回值：无
其它说明：无
*************************************************/
 void UP_Woody_Display_ColorID(void)
  {
	 printf("(Pixel)%lud\n",UP_ColorId);
	 UP_delay_ms(3000);
	 UP_LCD_ClearScreen();
	}
/******************************************************************
条码识别模块 
******************************************************************/

/**************************************
函数名称；UP_Woody_BarCodeRecLetIssue
函数功能：条码识别允许下发
被本函数调用的函数：UP_USR232_Putc或UP_UART5_Putc
调用本函数的函数：无
输入参数说明：无
输出参数说明：无
函数返回值：无
其它说明：无
**************************************/
void UP_Woody_BarCodeRecLetIssue(){
// 	char str[5]={0x55,0xAA,0x05,0x50,0x54};
// 	for(lab=0;lab<5;lab++){
// 		UP_UART5_Putc(str[lab]);
// 	}
	UP_UART5_Putc(0x55);
	UP_UART5_Putc(0xaa);
	UP_UART5_Putc(0x05);
	UP_UART5_Putc(0x50);
	UP_UART5_Putc(0x54);
	UP_delay_ms(DELAY_TIMES);
}
/**************************************
函数名称；UP_Woody_BarCodeRecForbidIssue
函数功能：条码识别允许下发
被本函数调用的函数：UP_USR232_Putc或UP_UART5_Putc
调用本函数的函数：无
输入参数说明：无
输出参数说明：无
函数返回值：无
其它说明：无
**************************************/
void UP_Woody_BarCodeRecForbidIssue(){
// 	char str[5]={0x55,0xAA,0x05,0x51,0x55};
// 	for(lab=0;lab<5;lab++){
// 		UP_UART5_Putc(str[lab]);
// 	}
	UP_UART5_Putc(0x55);
	UP_UART5_Putc(0xaa);
	UP_UART5_Putc(0x05);
	UP_UART5_Putc(0x51);
	UP_UART5_Putc(0x55);
	UP_delay_ms(DELAY_TIMES);
}
/**************************************
函数名称；UP_Woody_CodeRec_ID_Judge
函数功能：条码识别返回的ID
被本函数调用的函数：UP_USR232_Putc或UP_UART5_Putc
调用本函数的函数：无
输入参数说明：无
输出参数说明：无
函数返回值：ID，十进制数值形式，范围0~255
其它说明：无
**************************************/
u16 UP_Woody_BarCodeRec_ID_Judge(void)
{
	ID=(UP_Woody_ReturnData[4]&0X0F)+((UP_Woody_ReturnData[4]&0XF0)>>4)*16;
	return ID;
}
/*************************************************
函数名称；UP_Woody_Display_BarCodeID
函数功能：显示条码识别返回的ID
被本函数调用的函数：无
调用本函数的函数：无
输入参数说明：无
输出参数说明：无
函数返回值：无
其它说明：无
*************************************************/
 void UP_Woody_Display_BarCodeID(void)
  {
	 printf("(ID)%lu\n",UP_CodeId);
	 UP_delay_ms(2000);
	 UP_LCD_ClearScreen();
	}

/******************************************************************
二维码识别模块 
******************************************************************/

/**************************************
函数名称；UP_Woody_QrCodeRecLetIssue
函数功能：条码识别允许下发
被本函数调用的函数：UP_USR232_Putc或UP_UART5_Putc
调用本函数的函数：无
输入参数说明：无
输出参数说明：无
函数返回值：无
其它说明：无
**************************************/
void UP_Woody_QrCodeRecLetIssue(){
// 	char str[5]={0x55,0xAA,0x05,0x60,0x64};
// 	for(lab=0;lab<5;lab++){
// 		UP_UART5_Putc(str[lab]);
// 	}
	UP_delay_ms(DELAY_TIMES);
}
/**************************************
函数名称；UP_Woody_QrCodeRecForbidIssue
函数功能：条码识别允许下发
被本函数调用的函数：UP_USR232_Putc或UP_UART5_Putc
调用本函数的函数：无
输入参数说明：无
输出参数说明：无
函数返回值：无
其它说明：无
**************************************/
void UP_Woody_QrCodeRecForbidIssue(){
// 	char str[5]={0x55,0xAA,0x05,0x61,0x65};
// 	for(lab=0;lab<5;lab++){
// 		UP_UART5_Putc(str[lab]);
// 	}
	UP_delay_ms(DELAY_TIMES);
}
/**************************************
函数名称；UP_Woody_QrCodeRec_ID_Judge
函数功能：条码识别返回的ID
被本函数调用的函数：UP_USR232_Putc或UP_UART5_Putc
调用本函数的函数：无
输入参数说明：无
输出参数说明：无
函数返回值：ID，十进制数值形式，范围0~255
其它说明：无
**************************************/
u16 UP_Woody_QrCodeRec_ID_Judge(void)
{
	ID=(UP_Woody_ReturnData[4]&0X0F)+((UP_Woody_ReturnData[4]&0XF0)>>4)*16;
	return ID;
}
/**************************************
函数名称；UP_Woody_Display_QrCodeID
函数功能：显示二维码识别返回的ID
被本函数调用的函数：无
调用本函数的函数：无
输入参数说明：无
输出参数说明：无
函数返回值：无
其它说明：无
*************************************************/
 void UP_Woody_Display_QrCodeID(void)
  {
	 printf("(Pixel)%lud\n",UP_Woody_BarCodeRec_ID_Judge());
	 UP_delay_ms(3000);
	 UP_LCD_ClearScreen();
	}
