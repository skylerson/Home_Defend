#include "relay.h"
#include "main.h"

void Relay_init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PC�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //LED0-->PC10 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��PC13
 GPIO_ResetBits(GPIOB,GPIO_Pin_12);						 //PC13�����

}



void Relay_On(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_12);						 //PC13�����
}

void Relay_Off(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);						 //PC13�����
}