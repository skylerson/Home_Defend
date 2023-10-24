#include "esp8266Task.h"

#include "delay.h"

#include "FreeRTOS.h"
#include "task.h"

#include "bsp_esp8266.h"

#include "usart.h"

#include "oledTask.h"



#include "cJSON.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"


/********************************** �û���Ҫ���õĲ���**********************************/
#define      macUser_ESP8266_ApSsid                      "ghyll"                     //     "Jxust-nc"                                    //Ҫ���ӵ��ȵ������
#define      macUser_ESP8266_ApPwd                        "gh18879667847"                //  "jxustnc123456"                  //Ҫ���ӵ��ȵ����Կ

#define      macUser_ESP8266_TcpServer_IP                  "10.25.9.67"                       //Ҫ���ӵķ������� IP
#define      macUser_ESP8266_TcpServer_Port                "8080"                             //Ҫ���ӵķ������Ķ˿�

#define      macUser_ESP8266_OnenetMqttServer_IP                 "183.230.40.33"                 //Ҫ���ӵķ������� IP
#define      macUser_ESP8266_OnenetMqttServer_Port               "1883"                          //Ҫ���ӵķ������Ķ˿�

#define      macUser_ESP8266_NongLi_Request "http: //api.xlongwei.com/service/datetime/convert.json"


esp8266_task_info_s esp8266_task_info;

/**
  * @brief  ESP8266 ��Sta Tcp Client��͸��
  * @param  ��
  * @retval ��
  */
void ESP8266_StaTcpClient_UnvarnishTest ( void )
{

	ESP8266_Init (); 
	macESP8266_CH_ENABLE();
	ESP8266_ExitUnvarnishSend();
	ESP8266_AT_Test ();
	ESP8266_Net_Mode_Choose ( STA );
	vTaskDelay(500);

	//u2_printf("�����ȵ�.......\r\n");  
  while ( ! ESP8266_JoinAP ( esp8266_task_info.espSsid , esp8266_task_info.espPwd ) )
	{
	  vTaskDelay(500);
	}
	//u2_printf("�ȵ����ӳɹ�\r\n");  
	ESP8266_Enable_MultipleId ( DISABLE );

	//u2_printf("���ӷ�����.......\r\n");  
	while ( !	ESP8266_Link_Server ( enumTCP, esp8266_task_info.espServerIp , esp8266_task_info.espServerPort , Single_ID_0 ) )
	{
		vTaskDelay(500);
	}

	//u2_printf("���������ӳɹ���\r\n");  
	while ( ! ESP8266_UnvarnishSend () )
	{
	  vTaskDelay(500);
	}
	//u2_printf("����͸��.......\r\n");  

}





void ESP8266_Task_Init(void)
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
void esp8266_task(void *pvParameters)
{
  uint8_t ucStatus;
	ESP8266_Task_Init();
  ESP8266_StaTcpClient_UnvarnishTest();
	while(1)
	{	
		
	  if(strEsp8266_Fram_Record .InfBit .FramFinishFlag)                                   //������յ���ESP8266������
	 {					
			strEsp8266_Fram_Record .Data_RX_BUF[strEsp8266_Fram_Record .InfBit .FramLength] = '\0';
		 
			//u2_printf("%s",strEsp8266_Fram_Record .Data_RX_BUF);                                    //���ݴ�ESP8266ת�������ڵ�������
			strEsp8266_Fram_Record .InfBit .FramLength = 0;                                         //�������ݳ�������
			strEsp8266_Fram_Record.InfBit.FramFinishFlag = 0;                                       //���ձ�־����
		}

			if ( ucTcpClosedFlag )                                             //����Ƿ�ʧȥ����
			{
				ESP8266_ExitUnvarnishSend ();                                    //�˳�͸��ģʽ

				do ucStatus = ESP8266_Get_LinkStatus ();                         //��ȡ����״̬
				while ( ! ucStatus );
				
				if ( ucStatus == 4 )                                             //ȷ��ʧȥ���Ӻ�����
				{ 
					while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );
					
					while ( !	ESP8266_Link_Server ( enumTCP,macUser_ESP8266_TcpServer_IP,macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
					
				}
				while ( ! ESP8266_UnvarnishSend () );		
			}
    vTaskDelay(1000);
	}
}



