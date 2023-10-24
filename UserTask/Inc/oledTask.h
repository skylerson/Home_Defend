#ifndef __UI_OLED_TASK_H
#define __UI_OLED_TASK_H


#include "main.h"
#include "ds1302.h"

#define PAGE_MAX  30

//界面光标结构体
typedef struct
{
	short int ui_x;
	short int ui_y;
	short int ui_z;
}ui_coordinate_t;

void oled_task(void *pvParameters);
int stringToInt(unsigned char *pData , unsigned short int startOffest ,unsigned short int endOffest);

extern uint16_t Max_Temp ;
extern uint16_t Min_Temp ;
extern uint16_t Max_Humid ;
extern uint16_t Min_Humid ;

extern uint16_t angle ;
extern uint16_t speed ;
extern uint16_t ptc ;
extern uint8_t  Mode;

extern uint8_t Feed ;
extern uint32_t FeedBackCount ;
#endif

