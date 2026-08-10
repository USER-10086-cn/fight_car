#include "fighting.h"
#include "UP_Globle.h"
#include "UP_Bluetooth.h"
#include "UP_LCD.h"

#define TIMEOUT(start, ms)  ((g_SysTickTimer - (start)) >= (ms))


#define handon 1600  //手部接近最小值
#define leitai 450  //台上最小值
#define taixia 250  //台下最大值
#define enemy 400  //敌人在范围内的最小值（80cm）
#define wuti 800   //敌人在范围内的最小值 （150cm）
#define edge 1500    //光电到边沿阈值
static uint8_t jianyi_flag=0;

void Bluetooth_Rx_CallBack(u32 dat)//接收树莓派返回结果
{
    UP_LCD_ClearScreen();
    UP_LCD_ShowInt(0,2,g_Fight_HongwaiFiltered[FIGHT_HONGWAI_FORWARD]);
    UP_LCD_ShowHex(1,1,dat);
	if(dat=='0x01')
	{
		jianyi_flag=1;
	}
}


uint8_t is_jianyi(void)
{
	uint8_t ret = 0;
    UP_Bluetooth_Putc('a');  //先发a告诉树莓派开始，阻塞等待100ms取结果
    UP_delay_ms(100);  //阻塞等待100ms取结果，注意：时间控制不好容易晚复位jianyi_flag
    ret=jianyi_flag;
    jianyi_flag=0;
    return ret;
}




//判断是否挂住函数，是则返回1，否则返回0
uint8_t is_hangon(void)
{
    return 0;
}
//判断是否掉下函数
uint8_t is_off_leitai(void)
{
    return (g_Fight_HuiduFiltered[FIGHT_HUIDU_FORWARD1] < taixia && g_Fight_HuiduFiltered[FIGHT_HUIDU_FORWARD2] < taixia);
}
//判断是否有敌人函数
uint8_t is_enemy(void)
{
    return (g_Fight_HongwaiFiltered[FIGHT_HONGWAI_FORWARD]>enemy || g_Fight_HongwaiFiltered[FIGHT_HONGWAI_BACKWARD]>enemy
    || g_Fight_HongwaiFiltered[FIGHT_HONGWAI_L1]>wuti || g_Fight_HongwaiFiltered[FIGHT_HONGWAI_L2]>wuti
    || g_Fight_HongwaiFiltered[FIGHT_HONGWAI_L3]>wuti || g_Fight_HongwaiFiltered[FIGHT_HONGWAI_R1]>wuti
    || g_Fight_HongwaiFiltered[FIGHT_HONGWAI_R2]>wuti || g_Fight_HongwaiFiltered[FIGHT_HONGWAI_R3]>wuti);
}
//判断是否到边沿函数
uint8_t is_edge(void)
{
    return (g_Fight_GuangdianFiltered[FIGHT_GUANGDIAN_FORWARD1]  > edge
         || g_Fight_GuangdianFiltered[FIGHT_GUANGDIAN_FORWARD2]  > edge);
         //|| g_Fight_GuangdianFiltered[FIGHT_GUANGDIAN_BACKWARD1] > edge
         //|| g_Fight_GuangdianFiltered[FIGHT_GUANGDIAN_BACKWARD2] > edge);
}

//判断是否到后沿函数
uint8_t is_edge_behind(void)
{
    return (g_Fight_GuangdianFiltered[FIGHT_GUANGDIAN_BACKWARD1]  > edge
         || g_Fight_GuangdianFiltered[FIGHT_GUANGDIAN_BACKWARD2]  > edge);
}

//判断在台下是否对准擂台函数
uint8_t is_to_leitai(void)
{
    return (g_Fight_HongwaiFiltered[FIGHT_HONGWAI_FORWARD]>enemy && g_Fight_HongwaiFiltered[FIGHT_HONGWAI_BACKWARD]<enemy);
}

uint8_t is_jiaoluo(void)
{
    return 0;
}

//判断是否对准敌人函数
uint8_t is_accident(void)
{
    return (g_Fight_HongwaiFiltered[FIGHT_HONGWAI_FORWARD]>enemy);
}
//判断是否在台上函数
uint8_t is_on_leitai(void)
{
    return (g_Fight_HuiduFiltered[FIGHT_HUIDU_BACKWARD1] > leitai && g_Fight_HuiduFiltered[FIGHT_HUIDU_BACKWARD2] >leitai);
}
//判断是否有手放在左边
uint8_t is_hand(void)
{
    return (g_Fight_HongwaiFiltered[FIGHT_HONGWAI_L2]>handon);
}

//判断一段时间内是否无敌人
uint8_t is_no_enemy(void)
{
        // 需要的静态变量
    static uint8_t  waiting = 0;          // 是否正在等待确认
    static uint32_t trig_tick;            // 首次触发时的 tick
    uint8_t ret = 0;
    // 在主循环中调用
    if (!is_enemy()) {
        if (!waiting) {
            // 第一次发现，记录时间，开始等待
            trig_tick = g_SysTickTimer;
            waiting = 1;
        } else {
            // 已经在等待，检查是否超时
            if (TIMEOUT(trig_tick, 500)) {
                // 超时后条件依然成立 → 确认有效！
                ret=1;
                waiting = 0;  // 重置状态（如果需要单次触发）
            }
        }
    } else {
        // 条件不成立，立即取消等待（防止误触发）
        waiting = 0;
    }
    return ret;
}


//判断哪个传感器对准敌人 1正 2后 3左 6右
uint8_t which_to_enemy(void)
{
    uint8_t max_idx = 0;
    float max_val = 0.0f; // 或者 enemy 作为下限

    // 依次检查8个传感器
    if (g_Fight_HongwaiFiltered[FIGHT_HONGWAI_FORWARD] > enemy && g_Fight_HongwaiFiltered[FIGHT_HONGWAI_FORWARD] > max_val) {
        max_val = g_Fight_HongwaiFiltered[FIGHT_HONGWAI_FORWARD]*2;  //前面权重大
        max_idx = 1;
    }
    if (g_Fight_HongwaiFiltered[FIGHT_HONGWAI_BACKWARD] > enemy && g_Fight_HongwaiFiltered[FIGHT_HONGWAI_BACKWARD] > max_val) {
        max_val = g_Fight_HongwaiFiltered[FIGHT_HONGWAI_BACKWARD];
        max_idx = 2;
    }
    if (g_Fight_HongwaiFiltered[FIGHT_HONGWAI_L1] > wuti && g_Fight_HongwaiFiltered[FIGHT_HONGWAI_L1] > max_val) {
        max_val = g_Fight_HongwaiFiltered[FIGHT_HONGWAI_L1];
        max_idx = 3;
    }
    if (g_Fight_HongwaiFiltered[FIGHT_HONGWAI_L2] > wuti && g_Fight_HongwaiFiltered[FIGHT_HONGWAI_L2] > max_val) {
        max_val = g_Fight_HongwaiFiltered[FIGHT_HONGWAI_L2];
        max_idx = 4;
    }
    if (g_Fight_HongwaiFiltered[FIGHT_HONGWAI_L3] > wuti && g_Fight_HongwaiFiltered[FIGHT_HONGWAI_L3] > max_val) {
        max_val = g_Fight_HongwaiFiltered[FIGHT_HONGWAI_L3];
        max_idx = 5;
    }
    if (g_Fight_HongwaiFiltered[FIGHT_HONGWAI_R1] > wuti && g_Fight_HongwaiFiltered[FIGHT_HONGWAI_R1] > max_val) {
        max_val = g_Fight_HongwaiFiltered[FIGHT_HONGWAI_R1];
        max_idx = 6;
    }
    if (g_Fight_HongwaiFiltered[FIGHT_HONGWAI_R2] > wuti && g_Fight_HongwaiFiltered[FIGHT_HONGWAI_R2] > max_val) {
        max_val = g_Fight_HongwaiFiltered[FIGHT_HONGWAI_R2];
        max_idx = 7;
    }
    if (g_Fight_HongwaiFiltered[FIGHT_HONGWAI_R3] > wuti && g_Fight_HongwaiFiltered[FIGHT_HONGWAI_R3] > max_val) {
        max_val = g_Fight_HongwaiFiltered[FIGHT_HONGWAI_R3];
        max_idx = 8;
    }
    return max_idx;
}