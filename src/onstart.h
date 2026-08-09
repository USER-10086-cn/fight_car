#ifndef ONSTART_H
#define ONSTART_H

#include "STM32Lib\\stm32f10x.h"

int8_t clambon(void);               // 上台启动流程状态机
void    clambon_reset(void);         // 重置启动状态
void    motor_set_duty(int8_t left, int8_t right);  // 设置左右电机占空比(-100~100, 负值反转)


int8_t leitai_state_update(void);  // 台上状态更新
void    leitai_state_reset(void);  // 台上状态重置


int8_t    taixia_state_update(void);   // 台下状态更新
void    taixia_state_reset(void);   // 台下状态重置



int8_t    xundi_state_update(void);    // 寻敌状态更新
void    xundi_state_reset(void);    // 寻敌状态重置
#endif 
