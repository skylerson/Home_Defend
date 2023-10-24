#ifndef __ESP8266_CONFIG_H
#define __ESP8266_CONFIG_H

#include "stm32f4xx_conf.h" 
#include <stdio.h>
#include <stdbool.h>


#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

typedef enum{
	STA,
  AP,
  STA_AP  
} ENUM_Net_ModeTypeDef;


typedef enum{
	 enumTCP,
	 enumUDP,
} ENUM_NetPro_TypeDef;
	

typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;

typedef enum{
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4,
} ENUM_AP_PsdMode_TypeDef;

#define RX_BUF_MAX_LEN     512                                     //最大接收缓存字节数
typedef struct                                                    //串口数据帧的处理结构体
{
	char  Data_RX_BUF [ RX_BUF_MAX_LEN ];
	char  Data_TX_BUF [ RX_BUF_MAX_LEN ];

  union {
    __IO u16 InfAll;
    struct {
		  __IO u16 FramLength       :15;                               // 14:0 
		  __IO u16 FramFinishFlag   :1;                                // 15 
	  } InfBit;
  }; 
}strEsp8266_Fram_Record_t;


extern strEsp8266_Fram_Record_t strEsp8266_Fram_Record;

void Esp8266_printf(char* fmt,...) ;
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime );

void Esp8266_Sta_Init(void);

#endif


