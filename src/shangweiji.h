/******************************************************************/
//  Copyright (C), 2011-2012, 普中科技
//  Author         : 丁志伟
//  Reviser        : 马维国
//  Update Date    : 2012/08/01
//  Version        : 1.3
//  Description    : 蓝牙发送 ADC 数据 - 头文件
/******************************************************************/

#ifndef shangweiji_H
#define shangweiji_H

#include "STM32Lib\\stm32f10x.h"

// 通道数量（与 fighting.c 一致）
#define HONGWAI_CH_COUNT  8   // 红外通道数
#define HUIDU_CH_COUNT    4   // 灰度通道数

// 蓝牙发送间隔（毫秒）
#define BT_SEND_INTERVAL_MS  100

/**
 * @brief 发送当前 ADC 数值（红外 + 灰度）
 * @details 通过蓝牙格式化发送所有通道的滤波后值
 * @note 需要在 main 循环或定时器中周期性调用（建议每 100ms）
 */
void BT_SendADCData(void);

/**
 * @brief 简化版 - 一行把所有通道打包发送
 * @details 格式：H:2048,1024,500,...\r\n D:2048,1000,3000,500\r\n
 * @note 适合调试和快速发送全部数据
 */
void BT_SendAllADC_Simple(void);

#endif // shangweiji_H
