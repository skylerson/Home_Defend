#include "sim800aTask.h"

#include "FreeRTOS.h"
#include "task.h"

#include "led.h"
#include "beep.h"

#include "bsp_sim800a.h"

#include "userTask.h"
#include <string.h>
int16_t CSQ = 0;
uint8_t NetStatus = 0;


uint8_t send_flag = 0;

extern uint8_t page;

uint8_t Alarm_Phone[100] = {0};

uint8_t Number[50] = {0};

/**
	* @name						User_Init
  * @brief          This function is init user_info
  * @author         ghyll
	* @param[in]      void
  * @retval         void
	* @note           null
  */
void Sim800a_Init(void)
{
  while(SIM800A_Init() != GSM_TRUE )
	{
		LED0 = !LED0;
		vTaskDelay(1000);
	}
	
}

char namenum[20*4]={0},str[512]={0};

/**
	* @name						user_task
  * @brief          This function is perform user task
  * @author         ghyll
	* @param[in]      void
  * @retval         void
	* @note           null
  */
void sim800a_task(void *pvParameters)
{
	uint8_t testCard = 0;
	uint8_t sms_point = NULL;
	uint8_t count =0;
	
	Sim800a_Init();
  vTaskDelay(2000);
			//18723081247
			//19324886547
	sprintf((char *)Alarm_Phone,"%s","17343852307");

	if(SIM800A_Cmd("AT+CMGF=1\r","OK", 100) != GSM_TRUE)
	{
		printf("\r\n设置文本模式错误\r\n");
	}
	vTaskDelay(1000);
	
//	if(SIM800A_Cmd("AT+CSMP=17,167,0,241\r","OK", 100) != GSM_TRUE)
//	{
//		printf("\r\n CSMP  \r\n");
//	}
//	vTaskDelay(1000);
	
//	if(SIM800A_Cmd("AT+CSCS=\"GSM\"\r","OK", 100) != GSM_TRUE)
//	{
//		printf("\r\n  GSM Coder   \r\n");
//	}
//	vTaskDelay(1000);
	
	if(SIM800A_Cmd("AT+CREG=1\r","OK", 100) != GSM_TRUE)
	{
		printf("\r\n  CERG Error   \r\n");
	}
	
	if(SIM800A_Cmd("AT+CNMI=2,1,0,1,1\r","OK", 100) != GSM_TRUE)
	{
		printf("\r\n  CNMI Error   \r\n");
	}

	while(SIM800A_IsInsertCard() != GSM_TRUE)
	{
		
		if(++testCard >20)
		{
			printf("\r\n检测不到电话卡，请断电并重新接入电话卡\r\n");
		}
		vTaskDelay(500); 		
	}		
	
	while (SIM800A_Cmd("AT+CMGD=1,4\r","OK", 100) != GSM_TRUE)
	{
		printf("\r\n  CMGD Error   \r\n");
		vTaskDelay(500); 	
	}
	
 	NetStatus = SIM800A_Get_Net_Register_Status();
	CSQ = SIM800A_Get_Singal_Quality();
	
	while(1)
	{	
		if(send_flag)
		{	
			send_flag = 0;
			count = 0;
			while(SIM800A_SendSMS(( char *)Alarm_Phone,"Alarm") != GSM_TRUE)
			{
				count++;
				vTaskDelay(1000);
				if(count > 15) break;
			}
			
			SIM800A_ClrRecBuffer();
		}

		if(page == 1) //System_Page
		{
			 	NetStatus = SIM800A_Get_Net_Register_Status();
	      CSQ = SIM800A_Get_Singal_Quality();
		}
		
		sms_point = SIM800A_IsReceiveMS();
		
		if(sms_point != NULL)
		{
			printf("address:%d\r\n",sms_point);
			count = 0;
			while(SIM800A_ReadMessage(sms_point,namenum,str) == 0)
			{
				count++;
				vTaskDelay(1000);
				if(count > 15) break;
			}
		  printf("新短信:\n发件人:%s\n内容:%s\n\r",namenum,str);
			
			if(strstr(str,"Open") != NULL)
			{
				Control = 1;
				count = 0;
				while(SIM800A_SendSMS(namenum,"Already Open") != GSM_TRUE)
				{
					count++;
					vTaskDelay(1000);
					if(count > 15) break;
				}
				SIM800A_ClrRecBuffer();
				BeepCount = 1;
			}
			else if(strstr(str,"Close") != NULL)
			{
				Control = 0;
				count = 0;
				while(SIM800A_SendSMS(namenum,"Already Close") != GSM_TRUE)
				{
					count++;
					vTaskDelay(1000);
					if(count > 15) break;
				}
				SIM800A_ClrRecBuffer();
				BeepCount = 1;
			}
			
			if(sms_point > 10)
			{
					while (SIM800A_Cmd("AT+CMGD=1,4\r","OK", 100) != GSM_TRUE)
					{
						printf("\r\n  CMGD Error   \r\n");
						vTaskDelay(500); 	
					}
			}
		}
		
    vTaskDelay(100);

	}
}












