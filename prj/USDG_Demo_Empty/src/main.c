#include "UPLib\\UP_System.h"
#include "UPLib\\UP_ADC.h"
#include "UPLib\\UP_Timer.h"
#include "UPLib\\UP_CDS5500.h"


//void move(int forward,int turn);

//int left = 0;
//int right = 0;
//int AD0 = 0;  //右边红外传感器
//int AD1 = 0;  //右前方红外
//int AD2 = 0;  //后面红外


//int main()
//{
//	UP_System_Init();
//	//printf("Hello World");
//	

//	
//	
//	while(1)
//	{ 

//	}
//}



////电机移动函数
//void move(int forward,int turn)
//{
//	// 计算左右电机实际速度：forward控制整体前进/后退，turn控制转向
//	left = forward;		// 左电机速度 = 前进速度（转向时会叠加转向速度）
//	right = turn;		// 右电机速度 = 转向速度（与左电机配合实现转向）

//	// 速度限制：电机最大速度为1023，最小为-1023，避免超出范围烧毁电机
//	if(left > 1023)
//	{
//		left = 1023;	// 左电机速度上限1023
//	}
//	if(left < -1023)
//	{
//		left = -1023;	// 左电机速度下限-1023
//	}
//	if(right > 1023)
//	{
//		right = 1023;	// 右电机速度上限1023
//	}
//	if(right < -1023)
//	{
//		right = -1023;	// 右电机速度下限-1023
//	}

//	// 控制4个电机转动：
//	UP_CDS_SetSpeed(1, left);	// 1号电机（左前）：按left速度转动
//	UP_CDS_SetSpeed(2, right);	// 2号电机（右前）：按 right速度转动（反向）

//}



// 1. 灰度传感器通道定义
#define AD_CH_RIGHT    0   // 右前侧灰度传感器（检测右侧边缘）
#define AD_CH_FRONT    1   // 左前侧灰度传感器（检测左侧边缘）
#define AD_CH_BACK     2   // 后侧灰度传感器（检测后退时位置）

// 2. 擂台灰度阈值（纯黑→边缘渐变→中部渐变→中心红白区）
#define G_BLACK        300  // 擂台外纯黑地面阈值（周围地面纯黑）
#define G_EDGE         650  // 擂台边缘渐变阈值（外侧边缘，纯黑与中部灰之间）
#define G_MID          1800 // 擂台中部渐变阈值（非边缘/中心区域）
#define G_WHITE        3500 // 中心白色“武”字区域阈值
#define G_RED          600  // 中心红色区域阈值

// 3. 位置状态枚举
typedef enum {
    POS_OUTSIDE,     // 场外（纯黑区域，需启动登台程序）
    POS_EDGE,        // 擂台边缘（需规避，避免掉台失分）
    POS_MID_AREA,    // 擂台中部（正常格斗区域）
    POS_CENTER_RED,  // 擂台中心红色区域
    POS_CENTER_WHITE // 擂台中心白色“武”字区域（便于定位）
} StagePosition;

// 4. 全局变量
StagePosition g_current_pos = POS_OUTSIDE; // 当前位置状态
uint16_t g_adc_val[3] = {0};               // 3个传感器ADC值
uint32_t g_climb_start_time = 0;           // 登台开始时间（规则6.3：10秒超时计时）
uint8_t g_is_climbing = 0;                 // 登台状态标记（0：未登台，1：登台中）
const u8 MOTOR_ID[2] = {1, 2};             // 电机ID（2个）
uint8_t g_motor_init_flag = 0;             // 电机模式初始化标记（确保驱动合规）



/**
 * @brief 电机模式初始化（将舵机设为电机模式，规则5.2：合规动力模块）
 * @note 确保电机驱动前完成模式配置，避免无效动作
 */
void Motor_Mode_Init(void)
{
    if (g_motor_init_flag == 0)
    {
        // 遍历电机，设置为CDS_MOTOMODE（电机模式，规则5.2：合规驱动方式）
		uint8_t i = 0;
        for (i = 0; i < 2; i++)
        {
            UP_CDS_SetMode(MOTOR_ID[i], CDS_MOTOMODE);
            UP_delay_ms(50); // 等待配置生效，避免通信冲突
        }
        // 初始化后电机静止（规则6.3：比赛前机器人需保持静止）
        UP_CDS_SetSpeed(MOTOR_ID[0], 0);
        UP_CDS_SetSpeed(MOTOR_ID[1], 0);
        g_motor_init_flag = 1;
    }
}

/**
 * @brief 电机移动函数（实现前进、后退、转向）
 * @param forward：前进/后退速度（-1023~1023，正数=前进，负数=后退）
 * @param turn：转向速度（-1023~1023，正数=右转，负数=左转）
 */
void move(int forward, int turn)
{
	int16_t left_speed;  
	int16_t right_speed;
    // 未初始化电机时先执行初始化（确保动作有效）
    if (g_motor_init_flag == 0)
    {
        Motor_Mode_Init();
        return;
    }

    // 计算左右电机速度（前进与转向叠加，适配擂台格斗动作）
    left_speed = forward - turn;  // 左电机：前进速度 - 转向速度
    right_speed = forward + turn; // 右电机：前进速度 + 转向速度

    // 速度限制（避免超出电机范围，规则5.2：保护动力模块）
    left_speed = (left_speed > 1023) ? 1023 : (left_speed < -1023) ? -1023 : left_speed;
    right_speed = (right_speed > 1023) ? 1023 : (right_speed < -1023) ? -1023 : right_speed;

    // 驱动电机（规则5.2：合规电机控制）
    UP_CDS_SetSpeed(MOTOR_ID[0], left_speed);
    UP_CDS_SetSpeed(MOTOR_ID[1], right_speed);
}

/**
 * @brief 读取3个灰度传感器值（规则5.1：三线制AD传感器数据采集）
 * @note 定时采集，为位置识别提供数据支撑
 */
void GraySensor_Read(void)
{
    g_adc_val[0] = UP_ADC_GetValue(AD_CH_RIGHT); // 右前侧传感器
    g_adc_val[1] = UP_ADC_GetValue(AD_CH_FRONT); // 左前侧传感器
    g_adc_val[2] = UP_ADC_GetValue(AD_CH_BACK);  // 后侧传感器
}

int main(void)
{
	   // 1. 系统初始化（适配规则5.1、5.2：传感器、电机、定时器合规初始化）
    UP_System_Init();

    // 2. 电机模式初始化（规则5.2：合规动力模块配置）
    Motor_Mode_Init();
	
	move(500,200);
	
}
