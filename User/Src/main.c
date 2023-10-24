#include "main.h"
// ------------------------------  User --------------------------------------
#include "oled.h"
#include "key.h"
#include "delay.h"
#include "led.h"
#include "timer.h"
#include "usart.h"

#include "beep.h"
// ------------------------------ UserBsp ------------------------------------
#include "bsp_oled.h"

// ------------------------------ UserTask -----------------------------------

#include "oledTask.h"
#include "system_timer_task.h"

#include "FreeRTOS.h"
#include "task.h"

#include "startTask.h"

#include "user.h"

int main(void)
 {
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	 delay_init();
	 LED_init();
	 KEY_init();
	 Beep_Gpio_Init();
	 Shake_Gpio_Init();
	 Ladar_Gpio_Init();
	 uart1_init(115200);
	 timer3_init(9,7199);	
	 startTaskCreate();
	 vTaskStartScheduler();
	 while(1)
	{
		;
  }
}
 
