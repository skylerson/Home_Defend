#ifndef __ESP8266_TASK_H
#define __ESP8266_TASK_H

#include "bsp_esp8266.h"

typedef struct {
     char id[15];
     char name[20];
     char country[10];
     char path[50];
     char timezone[20];
     char timezone_offset[10];
     char text[20];
     char last_update[30];
     char code[5];
     char temperature[5];
}XinZhi_weather_info_s;

typedef struct {
    char success[2];
    char timestamp[15];
    char datetime_1[21];
    char datetime_2[25];
    char week_1[2];
    char week_2[10];
    char week_3[10];
    char week_4[10];

}NowApi_time_info_s;


typedef enum
{
	weather_id = 0,
	time_id,
	max_id
}net_id;


typedef struct
{
	char espSsid[30];
	char espPwd[30];
	char espServerIp[20];
	char espServerPort[10];
}esp8266_task_info_s;


void esp8266_task(void *pvParameters);
void espSetRequestInfo(net_id id);
extern uint8_t time_success;
extern uint8_t weather_success;


extern  XinZhi_weather_info_s           XinZhi_weather_info;
extern  NowApi_time_info_s              NowApi_time_info;

extern uint8_t init_step;
extern uint8_t hand_request;

#endif



