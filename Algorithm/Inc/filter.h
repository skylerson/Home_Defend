#ifndef __FILTER_H
#define __FILTER_H

#include "main.h"

typedef __packed struct
{
    float input;        //��������
    float out;          //�˲����������
    float num[1];       //�˲�����
    float frame_period; //�˲���ʱ���� ��λ s
} first_order_filter_type_t;

extern first_order_filter_type_t adjustable_power_first_order_filter;
extern first_order_filter_type_t chassic_left_font_motor_first_order_filter;
extern first_order_filter_type_t chassic_left_behind_motor_first_order_filter;
extern first_order_filter_type_t chassic_right_font_motor_first_order_filter;
extern first_order_filter_type_t chassic_right_behind_motor_first_order_filter;

first_order_filter_type_t *first_order_filter_init(first_order_filter_type_t *first_order_filter_type, float frame_period, const float num[1]);
float first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, float input);

#endif

