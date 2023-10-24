#ifndef __BSP_SIM800A_H
#define __BSP_SIM800A_H



#include "stm32f10x.h"
#include <stdio.h>
#include <stdbool.h>



#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define GSM_DEBUG_ON         	1
#define GSM_DEBUG_ARRAY_ON    0
#define GSM_DEBUG_FUNC_ON   	0

#define GSM_INFO(fmt,arg...)           printf("<<-GSM-INFO->> "fmt"\n",##arg)
#define GSM_ERROR(fmt,arg...)          printf("<<-GSM-ERROR->> "fmt"\n",##arg)
#define GSM_DEBUG(fmt,arg...)          do{\
                                         if(GSM_DEBUG_ON)\
                                         printf("<<-GSM-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
																					}while(0)

#define GSM_DEBUG_ARRAY(array, num)    do{\
                                         int32_t i;\
                                         uint8_t* a = array;\
                                         if(GSM_DEBUG_ARRAY_ON)\
                                         {\
                                            printf("<<-GSM-DEBUG-ARRAY->> [%d]\n",__LINE__);\
                                            for (i = 0; i < (num); i++)\
                                            {\
                                                printf("%02x   ", (a)[i]);\
                                                if ((i + 1 ) %10 == 0)\
                                                {\
                                                    printf("\n");\
                                                }\
                                            }\
                                            printf("\n");\
                                        }\
                                       }while(0)

#define GSM_DEBUG_FUNC()               do{\
                                         if(GSM_DEBUG_FUNC_ON)\
                                         printf("<<-GSM-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)

/******************************* SIM800A 数据类型定义 ***************************/
//typedef enum{
//	STA,
//  AP,
//  STA_AP  
//} ENUM_Net_ModeTypeDef;


//typedef enum{
//	 enumTCP,
//	 enumUDP,
//} ENUM_NetPro_TypeDef;
//	

//typedef enum{
//	Multiple_ID_0 = 0,
//	Multiple_ID_1 = 1,
//	Multiple_ID_2 = 2,
//	Multiple_ID_3 = 3,
//	Multiple_ID_4 = 4,
//	Single_ID_0 = 5,
//} ENUM_ID_NO_TypeDef;
//	

//typedef enum{
//	OPEN = 0,
//	WEP = 1,
//	WPA_PSK = 2,
//	WPA2_PSK = 3,
//	WPA_WPA2_PSK = 4,
//} ENUM_AP_PsdMode_TypeDef;

typedef enum{
    GSM_FALSE,
	  GSM_TRUE
}ENUM_GSM_Res_TypeDef;



/******************************* SIM800A 外部全局变量声明 ***************************/
#define RX_BUF_MAX_LEN     300                                     //最大接收缓存字节数

extern struct  STRUCT_USARTx_Fram                                  //串口数据帧的处理结构体
{
	char  Data_RX_BUF [ RX_BUF_MAX_LEN ];
	
  union {
    __IO u16 InfAll;
    struct {
		  __IO u16 FramLength       :15;                               // 14:0 
		  __IO u16 FramFinishFlag   :1;                                // 15 
	  } InfBit;
  }; 
	
} strSim800a_Fram_Record;

//extern struct STRUCT_USARTx_Fram strSim800a_Fram_Record;

/******************************** SIM800A 连接引脚定义 ***********************************/


#define      macSIM800A_USART_BAUD_RATE                       115200

#define      macSIM800A_USARTx                                USART2
#define      macSIM800A_USART_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define      macSIM800A_USART_CLK                             RCC_APB1Periph_USART2
#define      macSIM800A_USART_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define      macSIM800A_USART_GPIO_CLK                        RCC_APB2Periph_GPIOA     
#define      macSIM800A_USART_TX_PORT                         GPIOA   
#define      macSIM800A_USART_TX_PIN                          GPIO_Pin_2
#define      macSIM800A_USART_RX_PORT                         GPIOA
#define      macSIM800A_USART_RX_PIN                          GPIO_Pin_3
#define      macSIM800A_USART_IRQ                             USART2_IRQn
#define      macSIM800A_USART_INT_FUN                         USART2_IRQHandler

void                     USART_printf                       ( USART_TypeDef * USARTx, char * Data, ... );

/*********************************************** SIM800A 函数宏定义 *******************************************/
#define     macSIM800A_Usart( fmt, ... )           USART_printf ( macSIM800A_USARTx, fmt, ##__VA_ARGS__ ) 
#define     macPC_Usart( fmt, ... )                u2_printf ( fmt, ##__VA_ARGS__ )
//#define     macPC_Usart( fmt, ... )                



/****************************************** SIM800A 函数声明 ***********************************************/
void                     SIM800A_ClrRecBuffer(void);
char                     *SIM800A_GetRecBuffer(uint16_t *length);
uint8_t                  SIM800A_Init                        ( void );
void                     SIM800A_Rst                         ( void );
bool                     SIM800A_Cmd                         ( char * cmd, char * reply, u32 waittime );
bool                     SIM800A_Data                        ( char * cmd, char * reply1, char * reply2, u32 waittime );
void                     SIM800A_AT_Test                     ( void );
uint8_t                  SIM800A_IsInsertCard(void);
uint8_t                  SIM800A_Get_Number(char *num);
uint8_t									 SIM800A_IsReceiveMS(void);
void 										 SIM800A_Call(char *num);
uint8_t 						     SIM800A_IsRing(char *num);
uint8_t 								 SIM800A_SendSMS(char *num,char *smstext);
uint8_t									 IsASSIC(char * str);
int32_t                  SIM800A_Get_Singal_Quality(void);
uint8_t                  SIM800A_Get_Net_Register_Status(void);
uint8_t                  SIM800A_ReadMessage(uint8_t messadd,char *num,char *str);
#endif


