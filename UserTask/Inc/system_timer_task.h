#ifndef __SYSTEM_TIMER_TASK_H
#define  __SYSTEM_TIMER_TASK_H 

#include "main.h"


typedef struct
{
	uint32_t tick_count;
	
}system_timer_info_s;

typedef struct
{
	uint8_t flag;
	uint16_t Interactive;
}system_task_info_s;

extern system_task_info_s alert_task_info;
extern system_task_info_s  esp8266_task_info;
uint32_t get_System_Tick_Count(void);


#endif




