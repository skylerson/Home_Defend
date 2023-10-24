#ifndef __SIM800ATASK_H
#define __SIM800ATASK_H

#include "sys.h"
extern uint8_t send_flag ;

extern void sim800a_task(void *pvParameters);



extern uint32_t ShakeCount;
extern uint32_t SendCount;
extern uint32_t BeepCount;

extern uint32_t ShakeCountNum;
extern uint32_t SendCountNum;
extern uint32_t BeepCountNum;













#endif


