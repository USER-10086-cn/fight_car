#include "fighting.h"
#include "UP_Globle.h"
#include "signal.h"
#include "UP_CDS5500.h"

#define MOTO_ID_LEFT   1    // 左电机 ID
#define MOTO_ID_RIGHT  2    // 右电机 ID

#define TIMEOUT(start, ms)  ((g_SysTickTimer - (start)) >= (ms))  //传入开始时间与延迟时间ms
                                                                  //无阻塞延迟，时间到返回1

static int8_t left=0,  right=0;


// 四个状态，巡逻2，台下1，出击3，上台0 后面数字为状态机对应编号
typedef enum {
    xunluo,
    taixia,
    chuji,
    shangtai
} fight_state;


//fight_state current_state = shangtai;





/**

@brief 根据当前状态返回下一时刻状态

@param 此时正在处理的状态

@return 返回状态对应编号

@note 只有改变状态的那一刻返回状态编号改变

@date date7 12：58
*/
uint8_t is_goto_state(fight_state state)
{
    uint8_t ret;
    switch (state) {
    case shangtai:
        ret=(is_hangon() || is_off_leitai())?1:0;
        if(ret==0) ret=is_on_leitai()?2:0;   //if语句防止ret=1时再执行该语句ret被覆盖
        
        break;

    case taixia:
        ret=is_to_leitai()?0:1;
        break;

    case xunluo:
        ret=(is_off_leitai() || is_hangon())?1:2;
        if(ret==2) ret=is_enemy()?3:2;
        
        break;

    case chuji:
        ret=(is_off_leitai() || is_hangon())?1:3;
        if(ret==3) ret=(is_edge() || is_no_enemy())?2:3;
        
        break;
    }
    return ret;
}





/**

@brief 改变电机pwm占空比

@param 左电机占空比，右电机占空比

@return 无

@note 调用后直到下一次调用改变占空比

@date date7 11：12
*/
void motor_set_duty(int8_t sped_l,int8_t sped_r)
{
	left=sped_l;
	right=sped_r;
}



/**

@brief 使得电机按所设占空比运行

@param 无

@return 无

@note 循环需一直调用，否则电机将停止

@date date7 11：14
*/
void motor_run(void)
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





/**
@brief 对准函数，台下状态内部调用，对准返回1，传入参数为1 step复位为0

@param 是否中断台下对准

@return 对准完成标志位

@note 只有调用后第一次完成对准时返回1，待优化：处在角落位置的处理待优化

@date date7 16：01
*/

uint8_t duizhun_step(uint8_t is_exti) {
    static uint8_t finish_flag = 0; //对准完成标志位，返回值
    static uint8_t step = 0; //步进计数
    static uint32_t t_start; //开始时间

    switch (step) {
    case 0:
        finish_flag = 0;
        motor_set_duty(50, -50);         // 旋转
        step = 1;
        break;

    case 1:
        if (is_jiaoluo()) {
            motor_set_duty(50, 50);    //  直行
            t_start = g_SysTickTimer;
            step = 2;
        }
        if (is_to_leitai()) {
            motor_set_duty(0, 0);       // 停止
            t_start = g_SysTickTimer;
            step = 3;                   
    
        }

        break;


    case 2:


        if (TIMEOUT(t_start, 2000)) {
            motor_set_duty(0, 0);       // 2000ms后停止
            step = 0;                   // 继续去旋转
        }
        break;    

    case 3:
        if (TIMEOUT(t_start, 500)) {
            if(is_to_leitai())
            {
                finish_flag=1;
                step=0;
            }
            else step=0;

                              // 继续去旋转
        }
        break;    
    }
    if(is_exti==1) step=0;
    return finish_flag;
}




/**
@brief 退台函数，擂台状态内部调用，退完返回1，传入参数为1 step复位为0

@param 是否中断退台

@return 退台完成标志位

@note 只有调用后第一次完成退台时返回1，待优化：后退时退过头情况未考虑，遇到敌人和退台优先级未做处理

@date date7 11：42
*/
uint8_t tuitai_step(uint8_t is_exti) {
    static uint8_t finish_flag = 0; //退台完成标志位，返回值
    static uint8_t step = 0; //步进计数
    static uint32_t t_start; //开始时间

    switch (step) {
    case 0:
        finish_flag = 0;
        motor_set_duty(-50, -50);         // 后退
        t_start = g_SysTickTimer;
        step = 1;
        break;

    case 1:
        if (TIMEOUT(t_start, 1000)) {
            motor_set_duty(50, -50);    // 2000ms后右转
            t_start = g_SysTickTimer;
            step = 2;
        }

        break;


    case 2:


        if (TIMEOUT(t_start, 1000)) {
            motor_set_duty(0, 0);       // 2000ms后停止
            step = 0;                   // 复位，可循环或停止
            finish_flag=1;              // 设置退台完成标志位
        }
        break;    

    }
    if (is_exti) step = 0;

    return finish_flag;
}





/**
@brief 瞄准函数，出击状态内部调用，瞄准完成返回1，传入参数为1 dir复位为0

@param 是否中断瞄准

@return 瞄准完成标志位

@note 只有瞄准好为1，待优化：多目标难以专一瞄准，前后和左右传感器没归一化，死区处理待优化

@date date7 18：00
*/
uint8_t miaozhun_step(uint8_t is_exti) {
   
    uint8_t temp=0;
    static uint8_t dir=0;  //记录上一时刻的旋转方向 1为左 2为右
    uint8_t miaozhun_ok=0;
    int16_t k=0;

    temp=which_to_enemy();  //得到哪个方向对着最近目标
    switch(temp)
    {
			case 1:          //正对的话，不给旋转速度（k=0），瞄准ok标志位置位
        k=0;
        miaozhun_ok=1;
        break;
        case 2:        //反向对着给最大旋转速度
        k=2;
        break;
        case 3:        //345为在左边，离正前角度越大，k越大，旋转速度越大
        case 4:
        case 5:
        k=1+(temp-2)*0.2;
        break;
        case 6:        //678与345逻辑相反
        case 7:
        case 8:
        k=-1-(temp-5)*0.2;
        break;
        default:
        if(dir)
        {
					k=dir==1?1:-1;    //当所有方向都没有时，根据记录的上一时刻的方向慢速转
        }
        break;

    }
    if(k!=0 && miaozhun_ok!=1)      //记录这一时刻的方向为下一时刻（没检测到目标）使用，设置旋转速度
    {
        dir=k>0?1:2;
        motor_set_duty(-40*k, 40*k);
    }
    else
    {
			motor_set_duty(0, 0);      //瞄准到了，不转了，且要返回1了
    }

    if(is_exti==1)               //如果在外部终止瞄准则复位
    {
        dir=0;

    }
    return miaozhun_ok;
}






uint8_t shangtai_deal(void)
{
    uint8_t ret=0;
    static uint8_t temp_flag=0;
    static uint32_t t_start;
    if(is_hand() && temp_flag==0)
    {
        temp_flag=1;
        
    }
    else if(temp_flag==1)          //有手或刚对准蓄力，速度拉大
    {
        motor_set_duty(-90,-90);
        temp_flag=2;
    }
    else if(temp_flag==2 && is_hangon())           //挂住开始计时
    {
        
        temp_flag=3;
        t_start=g_SysTickTimer;
    }
    else if(temp_flag==3 && TIMEOUT(t_start, 200))
    {
        ret=is_goto_state(shangtai);              //200ms判断跳转
    }
    else if(temp_flag==4  && TIMEOUT(t_start, 2000))
    {
        ret=2;
        temp_flag=1;                   //转向完成后复位
    }

    if(ret==2)                    //如果上台就转向
    {
        temp_flag=4;
        motor_set_duty(50,-50);
        t_start=g_SysTickTimer;
        ret=0;
    }

    return ret;
    
}





uint8_t taixia_deal(void)
{
    uint8_t ret=1;
    static uint32_t t_start;
    static uint8_t temp_flag=0;
    if(is_hangon())                //如果挂住先自己下来
    {
        motor_set_duty(50,-50);
    }
    else if(duizhun_step(0))         //如果对准再次确认
    {
        ret=is_goto_state(taixia);
    }

    if(ret==0)                      //对准就开始蓄力
    {
        t_start=g_SysTickTimer;
        temp_flag=1;
        ret=1;
        motor_set_duty(50,50);
    }
    else if(temp_flag==1 && TIMEOUT(t_start, 2000))
    {
        ret=2;                      
        temp_flag=0;                   //蓄力完成后复位
    }
    return ret;

}




uint8_t xunluo_deal(void)
{
	static uint8_t temp_flag=0;  //检测到边沿置位
    uint8_t ret=2;
    if(is_edge())
    {
        temp_flag=1;
    }

    if(temp_flag==1)                //到边沿执行退台
    {
        if(tuitai_step(0))
        {
            temp_flag=0;
        }
    }
    else motor_set_duty(50,50);     //否则继续巡逻

		motor_run();
		
    ret=is_goto_state(xunluo);              //判断跳转
    if(ret==3 && temp_flag==1) ret=2;  //没退完台不出击
		
		
		if(ret==1) ret=2;
		
    if(ret==1 ) tuitai_step(1);  //如果退台就中断退台
		
		
		
    return ret;

}




uint8_t chuji_deal(void)
{
    uint8_t ret=3;

    if(miaozhun_step(0))            //如果瞄准到，直行
    {
        motor_set_duty(50,50);
    }
	  
		motor_run();
		
    ret=is_goto_state(chuji);
    if(ret==2 || ret==1) miaozhun_step(1);  //中断瞄准
		
		if(ret==1) ret=3;
		
    return ret;

}