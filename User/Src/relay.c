#include "relay.h"
#include "main.h"

void Relay_init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PC端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //LED0-->PC10 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化PC13
 GPIO_ResetBits(GPIOB,GPIO_Pin_12);						 //PC13输出高

}



void Relay_On(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_12);						 //PC13输出高
}

void Relay_Off(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);						 //PC13输出高
}