/******************************************************************/
//  Copyright (C), 2011-2012, 普中科技
//  Author         : UP Tech
//  Update Date    : 2026/07/25
//  Version        : 1.0
//  Description    : ADC 一阶低通滤波 —— 读取所有红外/灰度通道并更新滤波值
/******************************************************************/

#include "STM32Lib\\stm32f10x.h"
#include "UP_ADC.h"
#include "fighting.h"
#include "UP_LCD.h"
#include "UP_Globle.h"

static uint32_t nxt=0;

/* 红外通道映射表：g_ADValue 索引 → 逻辑通道 H0~H7（参照 shangweiji）*/
static const u8 g_fightHongwaiMap[FIGHT_HONGWAI_CH_COUNT] = {
    FIGHT_ADC_FORWARD,   // H0 → 前
    FIGHT_ADC_BACKWARD,  // H1 → 后
    FIGHT_ADC_L1,        // H2 → 左1
    FIGHT_ADC_L2,        // H3 → 左2
    FIGHT_ADC_L3,        // H4 → 左3
    FIGHT_ADC_R1,        // H5 → 右1
    FIGHT_ADC_R2,        // H6 → 右2
    FIGHT_ADC_R3,        // H7 → 右3
};

/* 灰度通道映射表：g_ADValue 索引 → 逻辑通道 D0~D3（参照 shangweiji）*/
static const u8 g_fightHuiduMap[FIGHT_HUIDU_CH_COUNT] = {
    FIGHT_ADC_FORWARD1,  // D0
    FIGHT_ADC_FORWARD2,  // D1
    FIGHT_ADC_BACKWARD1, // D2
    FIGHT_ADC_BACKWARD2, // D3
};

/* 光电通道映射表：g_ADValue 索引 */
static const u8 g_fightGuangdianMap[FIGHT_GUANGDIAN_CH_COUNT] = {
    FIGHT_ADC_GUANGDIAN_FORWARD1,
    FIGHT_ADC_GUANGDIAN_FORWARD2,
    FIGHT_ADC_GUANGDIAN_BACKWARD1,
    FIGHT_ADC_GUANGDIAN_BACKWARD2,
};

/* 滤波后的 ADC 值变量（全局，供外部读取）*/
u16 g_Fight_HongwaiFiltered[FIGHT_HONGWAI_CH_COUNT]     = {0};
u16 g_Fight_HuiduFiltered[FIGHT_HUIDU_CH_COUNT]         = {0};
u16 g_Fight_GuangdianFiltered[FIGHT_GUANGDIAN_CH_COUNT] = {0};

/*************************************************
  函数名称: Fighting_UpdateAllADC
  功能描述: 读取全部红外、灰度和光电 ADC 通道，执行一阶低通滤波，
            将滤波结果分别更新到对应滤波数组中。
  滤波公式: Y(n) = Y(n-1) + (X(n) - Y(n-1)) / K
           K = 2^FIGHT_FILTER_SHIFT = 16，α = 1/16
           使用整数移位实现，无浮点运算，适合 STM32
  调用说明: 在定时器中断或主循环中周期性调用（如每 10ms）
*************************************************/
void Fighting_UpdateAllADC(void)
{
    u8  i;
    u16 raw;         // 当前 ADC 原始值
    u16 filtered;    // 上一次滤波值

    /* ---- 红外 8 通道一阶低通滤波 ---- */
    for (i = 0; i < FIGHT_HONGWAI_CH_COUNT; i++)
    {
        raw      = UP_ADC_GetValue(g_fightHongwaiMap[i]);   // 读取原始值
        filtered = g_Fight_HongwaiFiltered[i];              // 取上次滤波结果

        // 一阶低通滤波，最小步长=1 避免截断死区
        if (raw > filtered)
        {
            u16 step = (raw - filtered) >> FIGHT_FILTER_SHIFT;
            if (step == 0) step = 1;
            filtered += step;
        }
        else if (raw < filtered)
        {
            u16 step = (filtered - raw) >> FIGHT_FILTER_SHIFT;
            if (step == 0) step = 1;
            filtered -= step;
        }

        g_Fight_HongwaiFiltered[i] = filtered;              // 更新滤波值
    }

    /* ---- 灰度 4 通道一阶低通滤波 ---- */
    for (i = 0; i < FIGHT_HUIDU_CH_COUNT; i++)
    {
        raw      = UP_ADC_GetValue(g_fightHuiduMap[i]);     // 读取原始值
        filtered = g_Fight_HuiduFiltered[i];                // 取上次滤波结果

        if (raw > filtered)
        {
            u16 step = (raw - filtered) >> FIGHT_FILTER_SHIFT;
            if (step == 0) step = 1;
            filtered += step;
        }
        else if (raw < filtered)
        {
            u16 step = (filtered - raw) >> FIGHT_FILTER_SHIFT;
            if (step == 0) step = 1;
            filtered -= step;
        }

        g_Fight_HuiduFiltered[i] = filtered;                // 更新滤波值
    }

    /* ---- 光电 4 通道一阶低通滤波 ---- */
    for (i = 0; i < FIGHT_GUANGDIAN_CH_COUNT; i++)
    {
        raw      = UP_ADC_GetValue(g_fightGuangdianMap[i]); // 读取原始值
        filtered = g_Fight_GuangdianFiltered[i];            // 取上次滤波结果

        if (raw > filtered)
        {
            u16 step = (raw - filtered) >> FIGHT_FILTER_SHIFT;
            if (step == 0) step = 1;
            filtered += step;
        }
        else if (raw < filtered)
        {
            u16 step = (filtered - raw) >> FIGHT_FILTER_SHIFT;
            if (step == 0) step = 1;
            filtered -= step;
        }

        g_Fight_GuangdianFiltered[i] = filtered;            // 更新滤波值
    }
		if(g_SysTickTimer>nxt)
		{
			UP_LCD_ShowHex(0,0,0x01);
//			UP_LCD_ClearScreen();
			// 直接显示原始 ADC 值（对应 g_Fight_GuangdianFiltered）
//			UP_LCD_ShowInt(0,0,g_Fight_HongwaiFiltered[FIGHT_HONGWAI_FORWARD]);   // 红外前
//			UP_LCD_ShowInt(0,1,g_Fight_HongwaiFiltered[FIGHT_HONGWAI_BACKWARD]);  // 红外后
//			UP_LCD_ShowInt(0,2,g_Fight_HongwaiFiltered[FIGHT_HONGWAI_L1]);       // 红外左1
//			UP_LCD_ShowInt(0,3,g_Fight_HongwaiFiltered[FIGHT_HONGWAI_L2]);       // 红外左2
//			UP_LCD_ShowInt(4,0,g_Fight_HongwaiFiltered[FIGHT_HONGWAI_L3]);       // 红外左3
//			UP_LCD_ShowInt(4,1,g_Fight_HongwaiFiltered[FIGHT_HONGWAI_R1]);       // 红外右1
//			UP_LCD_ShowInt(4,2,g_Fight_HongwaiFiltered[FIGHT_HONGWAI_R2]);       // 红外右2
//			UP_LCD_ShowInt(4,3,g_Fight_HongwaiFiltered[FIGHT_HONGWAI_R3]);       // 红外右3
//			UP_LCD_ShowInt(8,0,g_Fight_HuiduFiltered[FIGHT_HUIDU_FORWARD1]);     // 灰度前1
//			UP_LCD_ShowInt(8,1,g_Fight_HuiduFiltered[FIGHT_HUIDU_FORWARD2]);     // 灰度前2
//			UP_LCD_ShowInt(8,2,g_Fight_HuiduFiltered[FIGHT_HUIDU_BACKWARD1]);    // 灰度后1
//			UP_LCD_ShowInt(8,3,g_Fight_HuiduFiltered[FIGHT_HUIDU_BACKWARD2]);    // 灰度后2
//			UP_LCD_ShowInt(12,0,g_Fight_GuangdianFiltered[FIGHT_GUANGDIAN_FORWARD1]);  // 光电前1
//			UP_LCD_ShowInt(12,1,g_Fight_GuangdianFiltered[FIGHT_GUANGDIAN_FORWARD2]);  // 光电前2
//			UP_LCD_ShowInt(12,2,g_Fight_GuangdianFiltered[FIGHT_GUANGDIAN_BACKWARD1]); // 光电后1
//			UP_LCD_ShowInt(12,3,g_Fight_GuangdianFiltered[FIGHT_GUANGDIAN_BACKWARD2]); // 光电后2
			
			
			// 直接显示原始 ADC 值（对应 g_Fight_GuangdianFiltered）
			//UP_LCD_ClearScreen();
			//UP_LCD_ShowInt(1,1,UP_ADC_GetValue(1));  // FIGHT_ADC_GUANGDIAN_FORWARD1
			//UP_LCD_ShowInt(1,2,UP_ADC_GetValue(2));  // FIGHT_ADC_GUANGDIAN_FORWARD2  
			//UP_LCD_ShowInt(1,3,UP_ADC_GetValue(3));  // FIGHT_ADC_GUANGDIAN_BACKWARD1
			//UP_LCD_ShowInt(1,4,UP_ADC_GetValue(4));  // FIGHT_ADC_GUANGDIAN_BACKWARD2
			
			nxt=g_SysTickTimer+500;
		}
}


