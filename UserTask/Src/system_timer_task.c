#include "system_timer_task.h"
#include "led.h"
#include "bsp_key.h"
#include "key.h"

#include "oledTask.h"
#include "userTask.h"
#include "sim800aTask.h"

system_timer_info_s  system_timer_info;
void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
    {
			 system_timer_info.tick_count ++;
			 if( system_timer_info.tick_count == 0xFFFFFFFF) system_timer_info.tick_count = 0;
			
			 if( system_timer_info.tick_count %1000 == 0)
			 {
					if(ShakeCount > 0 )  ShakeCount--;
				  if(SendCount > 0 )  SendCount--;  
				  if(BeepCount > 0 )  BeepCount--;
			 }				

        key_IRQHandler();
    }
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}

uint32_t get_System_Tick_Count(void)
{
	return  system_timer_info.tick_count;
}






