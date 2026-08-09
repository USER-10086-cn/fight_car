/******************************************************************/
//  Copyright (C), 2011-2012, 普中科技
//  Author         : UP Tech
//  Update Date    : 2026/07/25
//  Version        : 1.0
//  Description    : ADC 滤波变量定义 与 一阶低通滤波更新
/******************************************************************/

#ifndef FIGHTING_H
#define FIGHTING_H

#include "STM32Lib\\stm32f10x.h"

/* 通道数量（参照 shangweiji）*/
#define FIGHT_HONGWAI_CH_COUNT  8   // 红外通道数
#define FIGHT_HUIDU_CH_COUNT    4   // 灰度通道数
#define FIGHT_GUANGDIAN_CH_COUNT 4  // 光电通道数

/* ---------- g_Fight_HongwaiFiltered 数组索引 ---------- */
#define FIGHT_HONGWAI_FORWARD   0   // 红外前
#define FIGHT_HONGWAI_BACKWARD  1   // 红外后
#define FIGHT_HONGWAI_L1        2   // 红外左1
#define FIGHT_HONGWAI_L2        3   // 红外左2
#define FIGHT_HONGWAI_L3        4   // 红外左3
#define FIGHT_HONGWAI_R1        5   // 红外右1
#define FIGHT_HONGWAI_R2        6   // 红外右2
#define FIGHT_HONGWAI_R3        7   // 红外右3

/* ---------- g_Fight_GuangdianFiltered 数组索引 ---------- */
#define FIGHT_GUANGDIAN_FORWARD1   0   // 光电前1
#define FIGHT_GUANGDIAN_FORWARD2   1   // 光电前2
#define FIGHT_GUANGDIAN_BACKWARD1  2   // 光电后1
#define FIGHT_GUANGDIAN_BACKWARD2  3   // 光电后2

/* ---------- g_Fight_HuiduFiltered 数组索引 ---------- */
#define FIGHT_HUIDU_FORWARD1   0   // 灰度前1
#define FIGHT_HUIDU_FORWARD2   1   // 灰度前2
#define FIGHT_HUIDU_BACKWARD1  2   // 灰度后1
#define FIGHT_HUIDU_BACKWARD2  3   // 灰度后2

/* 一阶低通滤波系数：滤波强度 K = 2^N，N 越大滤波越强、响应越慢 */
#define FIGHT_FILTER_SHIFT      4   // K = 16，α = 1/16

/* ---------- g_ADValue 红外通道索引映射（与 shangweiji 一致）---------- */
#define FIGHT_ADC_FORWARD   0   // 前
#define FIGHT_ADC_BACKWARD  1   // 后
#define FIGHT_ADC_L1        2   // 左1
#define FIGHT_ADC_L2        3   // 左2
#define FIGHT_ADC_L3        4   // 左3
#define FIGHT_ADC_R1        5   // 右1
#define FIGHT_ADC_R2        6   // 右2
#define FIGHT_ADC_R3        7   // 右3

/* ---------- g_ADValue 灰度通道索引映射 ---------- */
#define FIGHT_ADC_FORWARD1   8
#define FIGHT_ADC_FORWARD2   9
#define FIGHT_ADC_BACKWARD1  10
#define FIGHT_ADC_BACKWARD2  11

/* ---------- g_ADValue 光电通道索引映射 ---------- */
#define FIGHT_ADC_GUANGDIAN_FORWARD1   12
#define FIGHT_ADC_GUANGDIAN_FORWARD2   13
#define FIGHT_ADC_GUANGDIAN_BACKWARD1  14
#define FIGHT_ADC_GUANGDIAN_BACKWARD2  15

/* 滤波后的 ADC 值（全局变量，供其他模块读取）*/
extern u16 g_Fight_HongwaiFiltered[FIGHT_HONGWAI_CH_COUNT];   // 红外滤波值
extern u16 g_Fight_HuiduFiltered[FIGHT_HUIDU_CH_COUNT];       // 灰度滤波值
extern u16 g_Fight_GuangdianFiltered[FIGHT_GUANGDIAN_CH_COUNT]; // 光电滤波值

/* 函数声明 */
void Fighting_UpdateAllADC(void);     // 读取全部 ADC 通道并执行一阶低通滤波（红外+灰度+光电）

#endif // FIGHTING_H
