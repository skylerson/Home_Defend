#ifndef __USER_H
#define __USER_H

#include "delay.h"
#include "stm32f10x.h"




#define SHAKE PBin(10)


#define LADAR_CON PAout(5)
#define LADAR_OUT PAin(4)


void Shake_Gpio_Init(void);

void Ladar_Gpio_Init(void);





#endif