#include "timer.h"
#include "main.h"

//  定时器溢出时间
//  Tout=(arr+1)*(psc+1)/tclk
//  当PSC为7199时,此时的频率为10K,也就是运行一个周期需要0.1ms
//  芯片的一个工作周期好像是1us

void timer3_init(u16 arr,u16 psc)
{
	NVIC_InitTypeDef NVIC_Instruct;
	TIM_TimeBaseInitTypeDef TIM_Instructr;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//打开定时器，定时器2,3,4都在APB1内。
	//初始化定时器************************************************************************
	
	TIM_Instructr.TIM_Period=arr;//自动装载值
	TIM_Instructr.TIM_Prescaler=psc;//预分频系数
	TIM_Instructr.TIM_ClockDivision=TIM_CKD_DIV1;//这个是输入捕获
	TIM_Instructr.TIM_CounterMode=TIM_CounterMode_Up;//这个是计数模式
 //TIM_Instructr.TIM_RepetitionCounter=这个是高级定时器所用到的功能，在这里用通用定时器，所以可以不用到它
	TIM_TimeBaseInit(TIM3,&TIM_Instructr);
//使能定时器的更新中断**********************************************************************
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//第二个参数我们设置哪一种中断  ,这里我们设置更新中断；还有捕获中断1 TIM_IT_CC1；
	
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

 


//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM4_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	
  //设置该引脚为复用输出功能,输出TIM4 CH4的PWM脉冲波形	GPIOB.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; //TIM3_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
	
  TIM_DeInit(TIM4);
  //初始化TIM4
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2
	
  TIM_CtrlPWMOutputs(TIM4,ENABLE);          //MOE 主输出使能
	
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
	
  TIM_ARRPreloadConfig(TIM4, ENABLE); 
	
	TIM_Cmd(TIM4, ENABLE);  //使能TIM3
	
}

