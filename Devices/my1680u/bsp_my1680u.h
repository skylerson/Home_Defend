#ifndef __MY1680U_H
#define __MY1680U_H

#include "stdio.h"	

#include "sys.h"

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
	  	
extern u8  USART3_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART3_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart3_init(u32 bound);



#define MY1680U_CMD_PLAY       				0X11
#define MY1680U_CMD_SUSPEND    				0X12
#define MY1680U_CMD_NEXT       				0X13
#define MY1680U_CMD_LAST       				0X14
#define MY1680U_CMD_VOLUME_INCREASE   0X15
#define MY1680U_CMD_VOLUME_DECREASE   0X16
#define MY1680U_CMD_RESET       			0X19
#define MY1680U_CMD_FORWARD       	  0X1A
#define MY1680U_CMD_BACKWARD       	  0X1B
#define MY1680U_CMD_PLAY_OR_STOP      0X1C
#define MY1680U_CMD_STOP              0X1E


#define MY1680U_CMD_SET_VOLUME        0X31
#define MY1680U_CMD_PLAY_ROOT_SONG    0X41


#define MY1680U_CMD_PARAM_NO		0
#define MY1680U_CMD_PARAM_ONE   1
#define MY1680U_CMD_PARAM_TWO   2



#define MY1680U_CMD_START_CHAR 0X7E
#define MY1680U_CMD_STOP_CHAR 0XEF


typedef struct
{
	uint8_t valume;

}my1680u_info_s ;


extern my1680u_info_s my1680u_info;

void MY1680U_Init(void);

void MY1680U_Transmit_Cmd(uint8_t cmd,uint8_t paramExistNum,uint8_t param1,uint8_t param2);



#endif


