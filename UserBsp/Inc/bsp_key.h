#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "main.h"


#define K_TIME_SHORT  150
#define K_TIME_LONG   3000

//ÿһ��������������Ӧ��ֵ
typedef enum
{
	Key_Enter=0,
	Key_Up ,
	Key_Esc,
	Key_Left,
	Key_Down,
	Key_Right,
	K_MAX,
} K_control;

//�����Ƿ��µ�ֵ
typedef enum
{ 
	
	K_CONTROL_D,
	K_CONTROL_U
} 
K_control_down;


void key_IRQHandler(void);//--------------------------�ж϶�ȡ�������µ�ʱ�亯��
K_control getKey(u16 time,u16 LongTime);//------------------�ж��ĸ��������º���
K_control_down KEY_get(K_control key);//----------��ȡ�������º���
K_control_down Read_KeyGpio(u8 i);//----------��ȡ�ĸ��������º���




#endif

