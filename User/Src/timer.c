#include "timer.h"
#include "main.h"

//  ��ʱ�����ʱ��
//  Tout=(arr+1)*(psc+1)/tclk
//  ��PSCΪ7199ʱ,��ʱ��Ƶ��Ϊ10K,Ҳ��������һ��������Ҫ0.1ms
//  оƬ��һ���������ں�����1us

void timer3_init(u16 arr,u16 psc)
{
	NVIC_InitTypeDef NVIC_Instruct;
	TIM_TimeBaseInitTypeDef TIM_Instructr;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//�򿪶�ʱ������ʱ��2,3,4����APB1�ڡ�
	//��ʼ����ʱ��************************************************************************
	
	TIM_Instructr.TIM_Period=arr;//�Զ�װ��ֵ
	TIM_Instructr.TIM_Prescaler=psc;//Ԥ��Ƶϵ��
	TIM_Instructr.TIM_ClockDivision=TIM_CKD_DIV1;//��������벶��
	TIM_Instructr.TIM_CounterMode=TIM_CounterMode_Up;//����Ǽ���ģʽ
 //TIM_Instructr.TIM_RepetitionCounter=����Ǹ߼���ʱ�����õ��Ĺ��ܣ���������ͨ�ö�ʱ�������Կ��Բ��õ���
	TIM_TimeBaseInit(TIM3,&TIM_Instructr);
//ʹ�ܶ�ʱ���ĸ����ж�**********************************************************************
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//�ڶ�����������������һ���ж�  ,�����������ø����жϣ����в����ж�1 TIM_IT_CC1��
	
	NVIC_Instruct.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_Instruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Instruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_Instruct.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_Instruct);
	
	TIM_Cmd(TIM3,ENABLE);
	
}

#define Servo_MAX_LIMIT 1970 // mid 1860
#define Servo_MIN_LIMIT 1760

void Servo_Pwm_Set(uint16_t v)
{
	if(v < Servo_MIN_LIMIT) v = Servo_MIN_LIMIT;
	else if (v > Servo_MAX_LIMIT) v = Servo_MAX_LIMIT;
	TIM_SetCompare4(TIM4,v);
}

// lr 0-180   ud 0-90
void Servo_Set_Angle(float angle)
{
	if(angle < 0 ) angle = 0;
	else if(angle >180 ) angle = 180;

	angle = angle/180*(Servo_MAX_LIMIT - Servo_MIN_LIMIT) +Servo_MIN_LIMIT;

	Servo_Pwm_Set((uint16_t)angle);

}

void Fan_Set_Speed(float speed)
{
  if(speed < 0 ) speed = 0;
	else if(speed >10 ) speed = 10;
	
	TIM_SetCompare3(TIM4,(uint16_t)(speed*199));

}

 void Ptc_Set_Power(float power)
{
  if(power < 0 ) power = 0;
	else if(power >10 ) power = 10;
	
	TIM_SetCompare2(TIM4,(uint16_t)(power*200));

}

 


//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM4_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
  //���ø�����Ϊ�����������,���TIM4 CH4��PWM���岨��	GPIOB.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; //TIM3_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO
	
  TIM_DeInit(TIM4);
  //��ʼ��TIM4
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM3 Channel2 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2
	
  TIM_CtrlPWMOutputs(TIM4,ENABLE);          //MOE �����ʹ��
	
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	
  TIM_ARRPreloadConfig(TIM4, ENABLE); 
	
	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM3
	
}

