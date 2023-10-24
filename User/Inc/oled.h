#ifndef __OLED_H
#define __OLED_H

#include "sys.h"

//#define OLED_RST  PAout(9) 	 //原理图定义上是RES
//#define OLED_DC   PAout(8)   //原理图定义上是DC 
//#define OLED_SCLK  PBout(0)  
//#define OLED_SDIN  PBout(1)
#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_3)
#define OLED_SCLK_Set() GPIO_SetBits(GPIOB,GPIO_Pin_3)//SCL

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_4)//DIN
#define OLED_SDIN_Set() GPIO_SetBits(GPIOB,GPIO_Pin_4)

void Oled_Gpio_Init(void);


#endif

