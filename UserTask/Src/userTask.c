#include "userTask.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timer.h"
#include "led.h"
#include "oledTask.h"

#include "sim800aTask.h"

#include "user.h"
#include "beep.h"


uint8_t Ladar = 0;
uint8_t Shake = 0;

uint32_t ShakeCount = 0;
uint32_t SendCount = 0;
uint32_t BeepCount = 0;

uint32_t ShakeCountNum = 5;
uint32_t SendCountNum = 600;
uint32_t BeepCountNum = 10;

uint8_t Control = 0;

/**
	* @name						User_Init
  * @brief          This function is init user_info
  * @author         ghyll
	* @param[in]      void
  * @retval         void
	* @note           null
  */
void User_Init(void)
{
  
}



/**
	* @name						user_task
  * @brief          This function is perform user task
  * @author         ghyll
	* @param[in]      void
  * @retval         void
	* @note           null
  */
void user_task(void *pvParameters)
{
	User_Init();

	while(1)
	{	
		Shake = SHAKE;
		Ladar = LADAR_OUT;
	 	LED0 = !LED0;
		
		if(SendCount == 0 && Control == 1)	
		{			
			if(Shake == 1)
			{
				 ShakeCount = ShakeCountNum;
			}
			if(ShakeCount > 0 ) //
			{
				if(Ladar == 1)
				{
					SendCount = SendCountNum;
					send_flag = 1;
					BeepCount = BeepCountNum;
				}
			}
	 }
		
	 if(BeepCount > 0)
	 {
			Beep_On();
	 }
	 else 
	 {
			Beep_Off();
	 }
   vTaskDelay(100);

	}
}
















