#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "main.h"


#define K_TIME_SHORT  150
#define K_TIME_LONG   3000

//每一个按键按下所对应的值
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

//按键是否按下的值
typedef enum
{ 
	
	K_CONTROL_D,
	K_CONTROL_U
} 
K_control_down;


void key_IRQHandler(void);//--------------------------中断读取按键按下的时间函数
K_control getKey(u16 time,u16 LongTime);//------------------判断哪个按键按下函数
K_control_down KEY_get(K_control key);//----------获取按键按下函数
K_control_down Read_KeyGpio(u8 i);//----------获取哪个按键按下函数




#endif

