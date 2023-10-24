#include "beep.h"


void Beep_Gpio_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PC端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 //LED0-->PC10 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化PC13
	
  GPIO_SetBits(GPIOB,GPIO_Pin_11);		
}


void Beep_On(void)
{
		 GPIO_ResetBits(GPIOB,GPIO_Pin_11);		
}

void Beep_Off(void)
{
	 GPIO_SetBits(GPIOB,GPIO_Pin_11);	
}



