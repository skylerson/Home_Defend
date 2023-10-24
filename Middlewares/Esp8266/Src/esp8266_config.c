#include "esp8266_config.h"
#include "esp8266.h"

#include "stdio.h"
#include "stdarg.h"	 
#include "stdlib.h"
#include "string.h"

#include "delay.h"


strEsp8266_Fram_Record_t strEsp8266_Fram_Record = {0};

//void USART6_IRQHandler(void)            //����6�����жϷ������
//{
//	uint8_t ucCh;
//	if ( USART_GetITStatus ( USART6, USART_IT_RXNE ) != RESET )
//	{
//		ucCh  = USART_ReceiveData( USART6 );
//		if(strEsp8266_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ))                       //Ԥ��1���ֽ�д������	
//		{
//			strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ++ ]  = ucCh;
//		}
//	}
//	if( USART_GetITStatus( USART6, USART_IT_IDLE ) == SET )                                         //����֡�������
//	{
//    strEsp8266_Fram_Record .InfBit .FramFinishFlag = 1;
//		ucCh = USART_ReceiveData( USART6 );                                                           //�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR)	
//  }	
//}

/**
	* @name						esp8266_printf
  * @brief          we can connect esp8266 with usart through this function, facilitate our better communication
  * @author         ghyll
	* @param[in]      char* fmt : this is a point of character type ,point to the string to be sent
	* @param[in]      ...       : these are the data to be written to the string
  * @retval         void
  */
void Esp8266_printf(char* fmt,...)  
{  
	uint16_t i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)(strEsp8266_Fram_Record.Data_TX_BUF),fmt,ap);
	va_end(ap);
	i=strlen((const char*)(strEsp8266_Fram_Record.Data_TX_BUF));		
	for(j=0;j<i;j++)					
	{
		while((USART6->SR&0X40)==0);	
		USART6->DR = strEsp8266_Fram_Record.Data_TX_BUF[j];  
	}
}

/*
 * ��������ESP8266_Cmd
 * ����  ����WF-ESP8266ģ�鷢��ATָ��
 * ����  ��cmd�������͵�ָ��
 *         reply1��reply2���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
 *         waittime���ȴ���Ӧ��ʱ��
 * ����  : 1��ָ��ͳɹ�
 *         0��ָ���ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	strEsp8266_Fram_Record .InfBit .FramLength = 0;                //���¿�ʼ�����µ����ݰ�

	Esp8266_printf ( "%s\r\n", cmd );

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //����Ҫ��������
		return true;
	
	delay_ms ( waittime );                                         //��ʱ
	
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';
  
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) );
	
}


void Esp8266_Ap_Init(void)
{

	char cStr [ 100 ] = { 0 }, cCh;
	
	ESP8266_Rst();
	ESP8266_Net_Mode_Choose ( AP );
	
	while ( ! ESP8266_CIPAP ( "192.168.1.1" ) ) {;}

  while ( ! ESP8266_BuildAP ( "YehuoLink" , "wildfire", WPA2_PSK ) ) {;}
	
	ESP8266_Enable_MultipleId ( ENABLE );
	
	while ( !	ESP8266_StartOrShutServer ( ENABLE, "8080", "1800" ) ) {;}

	ESP8266_Inquire_ApIp ( cStr, 20 );
	
	strEsp8266_Fram_Record .InfBit .FramLength = 0;
	strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;	
	
}


void Esp8266_Sta_Init(void)
{
	
	ESP8266_ExitUnvarnishSend ();  
	
	ESP8266_AT_Test ();
	
	ESP8266_Net_Mode_Choose ( STA );

  while ( ! ESP8266_JoinAP ( "ghyll", "gh18879667847" ) );	
	
	ESP8266_Enable_MultipleId ( DISABLE );
	
	while ( !	ESP8266_Link_Server ( enumTCP,"192.168.43.247", "6688", Single_ID_0 ) );
	
	while ( ! ESP8266_UnvarnishSend () );


}

















