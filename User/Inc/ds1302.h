#ifndef __DS1302_H
#define __DS1302_H
#include "main.h"

#include "sys.h"

//IO方向设置
#define DS1302_SDA_IN()  {GPIOC->CRH&=0XF0FFFFFF;GPIOC->CRH|=8<<24;}
#define DS1302_SDA_OUT() {GPIOC->CRH&=0XF0FFFFFF;GPIOC->CRH|=3<<24;}

//IO操作函数	 
#define DS1302_IIC_SCL    PAout(0) 		//SCL
#define DS1302_IIC_SDA    PCout(14) 		//SDA	 
#define DS1302_READ_SDA   PCin(14) 		  //输入SDA 
#define DS1302_RST        PCout(15) 		//输入SDA 


typedef struct
{
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;
    uint8_t  minute;
    uint8_t  second;
    uint8_t  week;
} ds1302_time_info_t;
typedef struct
{
 ds1302_time_info_t ds1302_time_info;
 uint8_t state;
}alarm_s;

void Ds1302ReadTime(void);
void Ds1302Init(void);
ds1302_time_info_t *Ds1302ReadDecTime(void);

extern uint8_t TIME[7];
extern ds1302_time_info_t ds1302_time_info;
extern 
void Ds1302WriteDecTime(ds1302_time_info_t ds1302_time_info);
static const char *weekString[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};


#endif


