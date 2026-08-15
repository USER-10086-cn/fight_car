
#if 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x.h" // Device header

#include "fsm.h"
#include "queue.h"

#include "UP_System.h"
#include "UP_LCD.h"
#include "shangweiji.h"
#include "onstart.h"
#include "fighting.h"

enum
{
    EVENT_GOTO_STATE0 = EVENT_USER,
    EVENT_GOTO_STATE1,  
    EVENT_GOTO_STATE2,              
    EVENT_GOTO_STATE3,             
    EVENT_RUN_CURRENT,
    EVENT_RUN_STATE0_ONLY,              
    EVENT_RUN_STATE1_ONLY,         
    EVENT_RUN_STATE2_ONLY,          
    EVENT_RUN_STATE3_ONLY,          
};


static fsm_t g_fsm;

static queue_handle_t g_event_queue;


static status_t handle_state_init(fsm_t *self, event_t event);
static status_t handle_state0(fsm_t *self, event_t event);
static status_t handle_state1(fsm_t *self, event_t event);
static status_t handle_state2(fsm_t *self, event_t event);
static status_t handle_state3(fsm_t *self, event_t event);


static void process_events(void)
{
    event_t event;

    Fighting_UpdateAllADC();   // 更新全部传感器滤波值（红外+灰度+光电）

    while (!queue_is_empty(g_event_queue))
    {
       
        if (queue_receive(g_event_queue, &event))
        {
           
            fsm_dispatch(&g_fsm, event);
        }
    }
}


static void add_event_to_queue(event_t event)
{
   
    queue_send(g_event_queue, &event);
}


int main(void)
{
   
 

  
    g_event_queue = queue_create(sizeof(event_t));
    
    fsm_ctor(&g_fsm, handle_state_init);
    
    fsm_init(&g_fsm, EVENT_STATE_INIT);

    
    while (1)
    {
        process_events();
    }
}


static status_t handle_state_init(fsm_t *self, event_t event)
{
    UP_System_Init();
    return TRAN_TO(handle_state0);
}



static status_t handle_state0(fsm_t *self, event_t event)
{
    int8_t state_flag=0;
    
    status_t ret = STATUS_HANDLED;

    switch (event)
    {
    case EVENT_STATE_ENTER:
       
        
        add_event_to_queue(EVENT_RUN_CURRENT);
        break;

    case EVENT_STATE_EXIT:
       
        
        break;

    case EVENT_GOTO_STATE0:
        break;

    case EVENT_GOTO_STATE1:
        ret = TRAN_TO(handle_state1);
       
        break;

    case EVENT_GOTO_STATE2:
        
        ret = TRAN_TO(handle_state2);
        break;

    case EVENT_GOTO_STATE3:
       
        ret = TRAN_TO(handle_state3);
        break;

    case EVENT_RUN_CURRENT:
        state_flag=clambon();
        switch(state_flag)
        {
            case 1:
            add_event_to_queue(EVENT_GOTO_STATE1);
            break;
            case 2:
            add_event_to_queue(EVENT_GOTO_STATE2);
            break;
            case 3:
            add_event_to_queue(EVENT_GOTO_STATE3);
            break;
            default:
            break;
            
        }
        if(state_flag==1 || state_flag==2 || state_flag==3)
        {
            clambon_reset();
            break;
        }
        add_event_to_queue(EVENT_RUN_CURRENT);
        break;

    case EVENT_RUN_STATE0_ONLY:
       
        break;
    case EVENT_RUN_STATE1_ONLY:
    case EVENT_RUN_STATE2_ONLY:
    case EVENT_RUN_STATE3_ONLY:
        
        break;
    }

    return ret;
}


 
static status_t handle_state1(fsm_t *self, event_t event)
{
    int8_t state_flag=0;
    status_t ret = STATUS_HANDLED;

    switch (event)
    {
    case EVENT_STATE_ENTER:
      add_event_to_queue(EVENT_RUN_CURRENT);
       
        
        break;

    case EVENT_STATE_EXIT:
       
        
        break;
        
    case EVENT_GOTO_STATE0:
        ret = TRAN_TO(handle_state0);
        break;

    case EVENT_GOTO_STATE1:
       
        break;

    case EVENT_GOTO_STATE2:
        
        ret = TRAN_TO(handle_state2);
        break;

    case EVENT_GOTO_STATE3:
       
        ret = TRAN_TO(handle_state3);
        break;

    case EVENT_RUN_CURRENT:
        state_flag=taixia_state_update();
        switch(state_flag)
        {
            case 0:
            add_event_to_queue(EVENT_GOTO_STATE0);
            break;
            default:
            break;
            
        }
        if(state_flag==0)
        {
            taixia_state_reset();
            break;
        }

        add_event_to_queue(EVENT_RUN_CURRENT);
       
        break;

    case EVENT_RUN_STATE1_ONLY:
       
        break;
    case EVENT_RUN_STATE0_ONLY:
    case EVENT_RUN_STATE2_ONLY:
    case EVENT_RUN_STATE3_ONLY:
        
        break;
    }

    return ret;
}


static status_t handle_state2(fsm_t *self, event_t event)
{
    int8_t state_flag=0;
    status_t ret = STATUS_HANDLED;

    switch (event)
    {
    case EVENT_STATE_ENTER:
       
        add_event_to_queue(EVENT_RUN_CURRENT);
        break;

    case EVENT_STATE_EXIT:
     

        break;

    case EVENT_GOTO_STATE0:
        ret = TRAN_TO(handle_state0);
        break;

    case EVENT_GOTO_STATE1:
    
        ret = TRAN_TO(handle_state1);
        break;

    case EVENT_GOTO_STATE2:
     
     
        break;

    case EVENT_GOTO_STATE3:
    
       
        ret = TRAN_TO(handle_state3);
        break;

    case EVENT_RUN_CURRENT:
        state_flag=leitai_state_update();
        switch(state_flag)
        {
            
            case 1:
            add_event_to_queue(EVENT_GOTO_STATE1);
            break;
            
            case 3:
            add_event_to_queue(EVENT_GOTO_STATE3);
            break;
            default:
            break;
            
        }
        if(state_flag==1  || state_flag==3)
        {
            leitai_state_reset();
            break;
        }
        add_event_to_queue(EVENT_RUN_CURRENT);
        break;

    case EVENT_RUN_STATE2_ONLY:
       
        break;

    case EVENT_RUN_STATE0_ONLY:
    case EVENT_RUN_STATE1_ONLY:
    case EVENT_RUN_STATE3_ONLY:
        
        break;
    }

    return ret;
}


static status_t handle_state3(fsm_t *self, event_t event)
{
    int8_t state_flag=0;
    status_t ret = STATUS_HANDLED;

    switch (event)
    {
    case EVENT_STATE_ENTER:
        add_event_to_queue(EVENT_RUN_CURRENT);
        break;

    case EVENT_STATE_EXIT:
        
        
        break;

        
    case EVENT_GOTO_STATE0:
        ret = TRAN_TO(handle_state0);
        break;

    case EVENT_GOTO_STATE1:
      
        ret = TRAN_TO(handle_state1);
        break;

    case EVENT_GOTO_STATE2:
       
        ret = TRAN_TO(handle_state2);
        break;

    case EVENT_GOTO_STATE3:
        
        break;

    case EVENT_RUN_CURRENT:
        state_flag=xundi_state_update();
        switch(state_flag)
        {
            case 1:
            add_event_to_queue(EVENT_GOTO_STATE1);
            break;
            case 2:
            add_event_to_queue(EVENT_GOTO_STATE2);
            break;
            
            default:
            break;

            
        }
        if(state_flag==1  || state_flag==2)
        {
            xundi_state_reset();
            break;
        }
        add_event_to_queue(EVENT_RUN_CURRENT);
        break;

    case EVENT_RUN_STATE3_ONLY:
        
        break;

    case EVENT_RUN_STATE0_ONLY:
    case EVENT_RUN_STATE1_ONLY:
    case EVENT_RUN_STATE2_ONLY:
        
        
        break;
    }

    return ret;
}



#endif











#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x.h" // Device header


#include "UP_System.h"
#include "UP_LCD.h"
#include "shangweiji.h"
#include "run.h"
#include "fighting.h"
#include "UP_Globle.h"
int main(void)
 {
	 static uint8_t state_now=2;
	 static uint32_t nxt=0;
 	UP_System_Init();
 	while(1)
 	{
		Fighting_UpdateAllADC();
		
		switch(state_now)
		{
			case 0:
				state_now=shangtai_deal();
				break;
			case 1:
				state_now=taixia_deal();
				break;
			case 2:
				state_now=xunluo_deal();
				break;
			case 3:
				state_now=chuji_deal();
				break;
		}
		
		UP_LCD_ShowHex(4,0,state_now);
		
		
		if(g_SysTickTimer>nxt)
		{
			switch(state_now)
			{
				case 1:
					UP_Bluetooth_Putc('b');
					break;
				case 2:
					UP_Bluetooth_Putc('c');
					break;
				case 3:
					UP_Bluetooth_Putc('d');
					break;
				case 0:
					UP_Bluetooth_Putc('a');
					break;
			}
			nxt=g_SysTickTimer+500;
		}
		
 		//BT_SendADCData();
      // UP_USR232_Putc('a');
 		//UP_delay_ms(100);
 	}
 }


