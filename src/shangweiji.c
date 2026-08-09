

/******************************************************************/
//  Copyright (C), 2011-2012, 普中科技
//  Author         : 丁志伟
//  Reviser        : 马维国
//  Update Date    : 2012/08/01
//  Version        : 1.3
//  Description    : 蓝牙发送 ADC 数据，协议见 embedded_bt_test.h
/******************************************************************/

#include "STM32Lib\\stm32f10x.h"
#include "UP_ADC.h"
#include "shangweiji.h"
#include "UP_Bluetooth.h"
#include "fighting.h"
#include <stdio.h>   // sprintf
#include <string.h>  // strlen



/*************************************************
  函数名称：BT_SendADCData
  功能描述：通过蓝牙发送当前 ADC 数值
  调用示例：在主循环或定时器中，每 100ms 调用一次
*************************************************/
void BT_SendADCData(void)
{
    char buf[32];
    u16 val;
    u8 i;

    // ---- 发送红外 (ADC1) 8 个通道 ----
    for (i = 0; i < HONGWAI_CH_COUNT; i++)
    {
        val = g_Fight_HongwaiFiltered[i];          // 直接读取 fighting 滤波后的红外值
        sprintf(buf, "H%d:%u\r\n", i, val);       // 格式: H0:2048
		UP_Bluetooth_Puts(buf);
    }

    // ---- 发送灰度 (ADC2) 4 个通道 ----
    for (i = 0; i < HUIDU_CH_COUNT; i++)
    {
        val = g_Fight_HuiduFiltered[i];            // 直接读取 fighting 滤波后的灰度值
        sprintf(buf, "D%d:%u\r\n", i, val);       // 格式: D0:500
		UP_Bluetooth_Puts(buf);
    }
}

/*************************************************
  函数名称：BT_SendAllADC_Simple
  功能描述：简化版 - 一行把所有红外和灰度通道打包发送
  协议式：H:2048,1024,500,...\r\n  D:2048,1000,3000,500\r\n
*************************************************/
void BT_SendAllADC_Simple(void)
{
    char buf[128];
    int pos = 0;
    u8 i;

    // 红外一组
    pos += sprintf(buf + pos, "H:");
    for (i = 0; i < HONGWAI_CH_COUNT; i++)
    {
        pos += sprintf(buf + pos, "%u", g_Fight_HongwaiFiltered[i]);
        if (i < HONGWAI_CH_COUNT - 1) buf[pos++] = ',';
    }
    buf[pos++] = '\r';
    buf[pos++] = '\n';
    buf[pos] = '\0';
 
	UP_Bluetooth_Puts(buf);

    // 灰度一组
    pos = 0;
    pos += sprintf(buf + pos, "D:");
    for (i = 0; i < HUIDU_CH_COUNT; i++)
    {
        pos += sprintf(buf + pos, "%u", g_Fight_HuiduFiltered[i]);
        if (i < HUIDU_CH_COUNT - 1) buf[pos++] = ',';
    }
    buf[pos++] = '\r';
    buf[pos++] = '\n';
    buf[pos] = '\0';

	UP_Bluetooth_Puts(buf);
}
