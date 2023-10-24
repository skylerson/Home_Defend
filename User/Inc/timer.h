#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

void timer3_init(u16 arr,u16 psc);
void TIM4_PWM_Init(u16 arr,u16 psc);
void Servo_Set_Angle(float angle);
void Fan_Set_Speed(float speed);

 void Ptc_Set_Power(float power);
#endif

