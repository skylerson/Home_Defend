/**
  ****************************(C) COPYRIGHT 2016 DJI****************************
  * @file       pid.c/h
  * @brief      pid实现函数，包括初始化，PID计算函数，
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. 完成
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2016 DJI****************************
  */
#ifndef __PID_H
#define __PID_H
#include "main.h"

enum
{
  LLAST = 0,
  LAST,
  NOW,
  POSITION_PID,
  DELTA_PID,
};

typedef struct
{
    uint8_t mode;
    //PID 三参数
    float Kp;
    float Ki;
    float Kd;

    float max_out;  //最大输出
    float max_iout; //最大积分输出

    float set;
    float fdb;

    float out;
    float Pout;
    float Iout;
    float Dout;
    float Dbuf[3];  //微分项 0最新 1上一次 2上上次
    float error[3]; //误差项 0最新 1上一次 2上上次

} PidTypeDef;


typedef struct pid_t
{
	float p;
	float i;
	float d;
	
	float set;
	float get;
	float err[3];
	
	float pout;
	float iout;
	float dout;
	float out;
	
	uint8_t pid_mode;
	uint32_t max_out;
	uint32_t intergral_limit; //积分上限
	
	/*函数指针*/
	void(*pid_param_init)(struct pid_t 		*pid,
							uint8_t			mode,
							uint32_t 		max_output,
							uint32_t 		inte_limit,
							float			p,
							float			i,
							float 		d);
	void(*pid_reset)(struct pid_t *pid,float p, float i, float d);
	
}pid_t;



extern pid_t pid_imu_temp;
extern pid_t pid_power_limit;
extern pid_t pid_motor_position[4]; //7 = MOTOR_MAX_NUM In bsp_can.h
extern pid_t pid_motor_delta[4];
extern pid_t pid_motor_current[4];

extern pid_t gimbal_yaw_absolute_angle_pid;
extern pid_t gimbal_yaw_relative_angle_pid;
extern pid_t gimbal_yaw_gyro_pid;
extern pid_t gimbal_pitch_absolute_angle_pid;
extern pid_t gimbal_pitch_relative_angle_pid;
extern pid_t gimbal_pitch_gyro_pid;
extern pid_t chassic_fllow_pid;	

extern pid_t pid_shoot_motor;
extern pid_t pid_shoot_motor_angle;

extern void PID_Init(PidTypeDef *pid, uint8_t mode, const float PID[3], float max_out, float max_iout);
extern float PID_Calc(PidTypeDef *pid, float ref, float set);
extern void PID_clear(PidTypeDef *pid);

float pid_calc(pid_t *pid, float get, float set);
float GIMBAL_PID_Calc(pid_t *pid, float get, float set, float error_delta);
extern pid_t  *pid_point_init(pid_t *pid,uint8_t mode,uint32_t maxout,uint32_t	intergral_limit,float kp,float ki,float kd);
float rad_format(float Ang);
#endif
