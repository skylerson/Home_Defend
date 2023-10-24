#ifndef __ALERT_TASK_H
#define __ALERT_TASK_H

#include "main.h"

typedef struct
{
	uint8_t flag;
	uint32_t timer;
}alert_info_s;

extern alert_info_s password_alert_info;

void alert_task(void);


#endif




