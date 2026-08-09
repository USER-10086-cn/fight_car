#include "fighting.h"
#include "UP_Globle.h"
#include "signal.h"
#include "UP_CDS5500.h"

#define MOTO_ID_LEFT   1    // 左电机 ID
#define MOTO_ID_RIGHT  2    // 右电机 ID






void motor_set_duty(int8_t left, int8_t right)
{
    u8  id[4];
    s16 speed[4];

    id[0] = MOTO_ID_LEFT;
    id[1] = MOTO_ID_LEFT;
    id[2] = MOTO_ID_RIGHT;
    id[3] = MOTO_ID_RIGHT;

    speed[0] = (s16)left  * 1023 / 100;
    speed[1] = (s16)left  * 1023 / 100;
    speed[2] = (s16)right * 1023 / 100;
    speed[3] = (s16)right * 1023 / 100;

    UP_CDS_Set4MotoSpeed(id, speed);
}




static uint32_t start_time=0;




static int8_t start_state=-2;


int8_t clambon(void)
{
    
    if(is_hand() && start_state==-2)//手检测
    {
        start_state=-1;
        
    }
    else if(start_state==-1)
    {
        //设定转速
        motor_set_duty(50,50);
        start_state=0;
    }
    else if(start_state==0 && is_hangon())//倾角大于一定值，防止还没上去就转到上台状态
    {
        start_time=g_SysTickTimer;
        start_state=4;
    }
    else if(start_state==4 && is_on_leitai() && g_SysTickTimer-start_time>500) //过了一段时间，并且倾角小于一定值
    {
        //跳转到状态1巡逻
        start_state=2;
    }
    else if(start_state==4  && is_off_leitai() && g_SysTickTimer-start_time>500) //过了一段时间
    {
        //跳转到上台状态
        start_state=1;
    }
    else if(start_state==4 && is_hangon() && g_SysTickTimer-start_time>500)//过了一段时间 倾角大于一定值
    {
        //跳转到上台状态，上台状态解决挂壁问题和掉下问题
        start_state=1;
    }
    return start_state;
}
void clambon_reset(void)
{
    start_state=-1;
    start_time=0;
}
//跳转后start_state变为-1，start_time变为0




static int8_t leitai_state=-4;


int8_t leitai_state_update(void)
{
   



    //检测是否有敌人
    //检测是否掉下
    //检测是否到边沿

    if(is_hangon())
    {
        leitai_state=1;
    }
    else if(is_off_leitai())
    {
        leitai_state=1;
    }
    else if(is_enemy())
    {
        leitai_state=3;
    }






    if(leitai_state==-4)
    {
        //直行
        motor_set_duty(50,50);
        leitai_state=-3;
    }
    else if(leitai_state==-3 && is_edge())
    {
        leitai_state=-2;
        start_time=g_SysTickTimer;
        motor_set_duty(-50,-50);
        //设置为后退
    }
    if(leitai_state==-2 && g_SysTickTimer-start_time>500)
    {
        
        //设置为旋转
        motor_set_duty(50,-50);
        start_time=g_SysTickTimer;
        leitai_state=-1;
        
    }
    
    else if(leitai_state==-1 && g_SysTickTimer-start_time>500)
    {
        leitai_state=-4;
    }

    
    return leitai_state;
}
void leitai_state_reset(void)
{
    leitai_state=-4;
    start_time=0;
}




static int8_t taixia_state=-2;


int8_t taixia_state_update(void)
{
    if(is_hangon())
    {
        //主动掉下
        motor_set_duty(-50,-50);
    }
    else
    {
        if(is_accident() && taixia_state==-2)
        {
            //停止
            motor_set_duty(0,0);
            start_time=g_SysTickTimer;
            taixia_state=-1;
        }
        else if(taixia_state==-2)
        {
            //旋转
            motor_set_duty(50,-50);
        }
        if(taixia_state==-1)
        {
            //后退
            motor_set_duty(-50,-50);
            if(g_SysTickTimer-start_time>500)
            {
                //速度变0
                motor_set_duty(0,0);
                taixia_state=0;
            }
            
            //退后蓄力一段时间
           
        }
    }
    return taixia_state;
    //检测是否挂住          挂住则主动掉下函数
    //是否对准              对准则跳转start，没对准接着对准      
}
void taixia_state_reset(void)
{
    taixia_state=-2;
    start_time=0;
}




static int8_t xundi_state=-3;


int8_t xundi_state_update(void)   //没对准就对准，对准就冲
{


    if(xundi_state==-3 )
    {
        xundi_state=-2;
        start_time=g_SysTickTimer;
        motor_set_duty(50,-50);
        //旋转
    }
    else if(xundi_state==-2 && is_accident())
    {
        motor_set_duty(50,50);
        //直行
        xundi_state=-1;
    }
    else if(xundi_state==-2 && g_SysTickTimer-start_time>2000 && !is_enemy())
    {
        xundi_state=2;
    }
    else if(xundi_state==-1 && !is_accident())
    {
        motor_set_duty(0,0);
        
        xundi_state=-3;
    }






    if(is_hangon())
    {
        xundi_state=1;
    }
    else if(is_off_leitai())
    {
        xundi_state=1;
    }
    else if(is_edge())
    {
        xundi_state=2;
    }
    



    return xundi_state;
    
}

void xundi_state_reset(void)
{
    xundi_state=-3;
    start_time=0;
}
