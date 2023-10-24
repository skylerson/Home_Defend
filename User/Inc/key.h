#ifndef __KEY_H
#define __KEY_H

#include "delay.h"
#include "stm32f10x.h"

#define KEY_ENTER PBin(6)
#define KEY_ESC   PBin(7)

#define KEY_RIGHT PBin(8)
#define KEY_LEFT  PBin(9)

#define KEY_UP    PBin(5)
#define KEY_DOWN  PAin(12)


void KEY_init(void);


#endif

