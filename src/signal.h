#ifndef SIGNAL_H
#define SIGNAL_H

#include "STM32Lib\\stm32f10x.h"
#include "fighting.h"

uint8_t is_hangon(void);        // 判断是否挂住
uint8_t is_off_leitai(void);    // 判断是否掉下擂台
uint8_t is_enemy(void);         // 判断是否有敌人
uint8_t is_edge(void);          // 判断是否到边沿
uint8_t is_edge_behind(void);    //判断是否后面到边沿
uint8_t is_to_leitai(void);     // 判断台下是否对准擂台
uint8_t is_accident(void);      // 判断是否对准敌人
uint8_t is_on_leitai(void);     // 判断是否在台上
uint8_t is_hand(void);          // 判断是否有手放在左边
uint8_t is_no_enemy(void);      // 判断是否一段时间无敌人
uint8_t which_to_enemy(void);   // 判断哪个传感器对准敌人
uint8_t is_jiaoluo(void);       // 判断是否处于台下角落处
uint8_t is_jianyi(void);        //判断是不是减益方块（有阻塞等待结果）
uint8_t is_zengyi(void);
void Bluetooth_Rx_CallBack(u32 dat);
#endif // SIGNAL_H
