#include "filter.h"

//可调电源一阶低通滤波
first_order_filter_type_t adjustable_power_first_order_filter;
//底盘电机转矩电流一阶低通滤波
first_order_filter_type_t chassic_left_font_motor_first_order_filter;
first_order_filter_type_t chassic_left_behind_motor_first_order_filter;
first_order_filter_type_t chassic_right_font_motor_first_order_filter;
first_order_filter_type_t chassic_right_behind_motor_first_order_filter;

/**
  * @brief          一阶低通滤波初始化
  * @author         RM
  * @param[in]      一阶低通滤波结构体
  * @param[in]      间隔的时间，单位 s
  * @param[in]      滤波参数
  * @retval         void
  */
first_order_filter_type_t *first_order_filter_init(first_order_filter_type_t *first_order_filter_type, float frame_period, const float num[1])
{
    first_order_filter_type->frame_period = frame_period;
    first_order_filter_type->num[0] = num[0];
    first_order_filter_type->input = 0.0f;
    first_order_filter_type->out = 0.0f;
    return first_order_filter_type;
}

/**
  * @brief          一阶低通滤波计算
  * @author         RM
  * @param[in]      一阶低通滤波结构体
  * @param[in]      间隔的时间，单位 s
  * @retval         float first_order_filter_type->out
  */
float first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, float input)
{
    first_order_filter_type->input = input;
    first_order_filter_type->out =
    first_order_filter_type->num[0] / (first_order_filter_type->num[0] + first_order_filter_type->frame_period) * first_order_filter_type->out + first_order_filter_type->frame_period / (first_order_filter_type->num[0] + first_order_filter_type->frame_period) * first_order_filter_type->input;
    return first_order_filter_type->out;
}

