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


/********************************** 用户需要设置的参数**********************************/
#define      macUser_ESP8266_ApSsid                      "ghyll"                     //     "Jxust-nc"                                    //要连接的热点的名称
#define      macUser_ESP8266_ApPwd                        "gh18879667847"                //  "jxustnc123456"                  //要连接的热点的密钥

#define      macUser_ESP8266_TcpServer_IP                  "10.25.9.67"                       //要连接的服务器的 IP
#define      macUser_ESP8266_TcpServer_Port                "8080"                             //要连接的服务器的端口

#define      macUser_ESP8266_OnenetMqttServer_IP                 "183.230.40.33"                 //要连接的服务器的 IP
#define      macUser_ESP8266_OnenetMqttServer_Port               "1883"                          //要连接的服务器的端口

#define      macUser_ESP8266_NongLi_Request "http: //api.xlongwei.com/service/datetime/convert.json"


esp8266_task_info_s esp8266_task_info;

/**
  * @brief  ESP8266 （Sta Tcp Client）透传
  * @param  无
  * @retval 无
  */
void ESP8266_StaTcpClient_UnvarnishTest ( void )
{

	ESP8266_Init (); 
	macESP8266_CH_ENABLE();
	ESP8266_ExitUnvarnishSend();
	ESP8266_AT_Test ();
	ESP8266_Net_Mode_Choose ( STA );
	vTaskDelay(500);

	//u2_printf("连接热点.......\r\n");  
  while ( ! ESP8266_JoinAP ( esp8266_task_info.espSsid , esp8266_task_info.espPwd ) )
	{
	  vTaskDelay(500);
	}
	//u2_printf("热点连接成功\r\n");  
	ESP8266_Enable_MultipleId ( DISABLE );

	//u2_printf("连接服务器.......\r\n");  
	while ( !	ESP8266_Link_Server ( enumTCP, esp8266_task_info.espServerIp , esp8266_task_info.espServerPort , Single_ID_0 ) )
	{
		vTaskDelay(500);
	}

	//u2_printf("服务器连接成功。\r\n");  
	while ( ! ESP8266_UnvarnishSend () )
	{
	  vTaskDelay(500);
	}
	//u2_printf("进入透传.......\r\n");  

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
		
	  if(strEsp8266_Fram_Record .InfBit .FramFinishFlag)                                   //如果接收到了ESP8266的数据
	 {					
			strEsp8266_Fram_Record .Data_RX_BUF[strEsp8266_Fram_Record .InfBit .FramLength] = '\0';
		 
			//u2_printf("%s",strEsp8266_Fram_Record .Data_RX_BUF);                                    //数据从ESP8266转发到串口调试助手
			strEsp8266_Fram_Record .InfBit .FramLength = 0;                                         //接收数据长度置零
			strEsp8266_Fram_Record.InfBit.FramFinishFlag = 0;                                       //接收标志置零
		}

			if ( ucTcpClosedFlag )                                             //检测是否失去连接
			{
				ESP8266_ExitUnvarnishSend ();                                    //退出透传模式

				do ucStatus = ESP8266_Get_LinkStatus ();                         //获取连接状态
				while ( ! ucStatus );
				
				if ( ucStatus == 4 )                                             //确认失去连接后重连
				{ 
					while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );
					
					while ( !	ESP8266_Link_Server ( enumTCP,macUser_ESP8266_TcpServer_IP,macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
					
				}
				while ( ! ESP8266_UnvarnishSend () );		
			}
    vTaskDelay(1000);
	}
}



