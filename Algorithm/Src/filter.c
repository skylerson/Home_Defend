#include "filter.h"

//�ɵ���Դһ�׵�ͨ�˲�
first_order_filter_type_t adjustable_power_first_order_filter;
//���̵��ת�ص���һ�׵�ͨ�˲�
first_order_filter_type_t chassic_left_font_motor_first_order_filter;
first_order_filter_type_t chassic_left_behind_motor_first_order_filter;
first_order_filter_type_t chassic_right_font_motor_first_order_filter;
first_order_filter_type_t chassic_right_behind_motor_first_order_filter;

/**
  * @brief          һ�׵�ͨ�˲���ʼ��
  * @author         RM
  * @param[in]      һ�׵�ͨ�˲��ṹ��
  * @param[in]      �����ʱ�䣬��λ s
  * @param[in]      �˲�����
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
  * @brief          һ�׵�ͨ�˲�����
  * @author         RM
  * @param[in]      һ�׵�ͨ�˲��ṹ��
  * @param[in]      �����ʱ�䣬��λ s
  * @retval         float first_order_filter_type->out
  */
float first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, float input)
{
    first_order_filter_type->input = input;
    first_order_filter_type->out =
    first_order_filter_type->num[0] / (first_order_filter_type->num[0] + first_order_filter_type->frame_period) * first_order_filter_type->out + first_order_filter_type->frame_period / (first_order_filter_type->num[0] + first_order_filter_type->frame_period) * first_order_filter_type->input;
    return first_order_filter_type->out;
}
