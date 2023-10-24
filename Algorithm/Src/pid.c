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

#include "pid.h"
#include "stdio.h"

#define LimitMax(input, max)   \
    {                          \
        if (input > max)       \
        {                      \
            input = max;       \
        }                      \
        else if (input < -max) \
        {                      \
            input = -max;      \
        }                      \
    }

void PID_Init(PidTypeDef *pid, uint8_t mode, const float PID[3], float max_out, float max_iout)
{
    if (pid == NULL || PID == NULL)
    {
        return;
    }
    pid->mode = mode;
    pid->Kp = PID[0];
    pid->Ki = PID[1];
    pid->Kd = PID[2];
    pid->max_out = max_out;
    pid->max_iout = max_iout;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->error[0] = pid->error[1] = pid->error[2] = pid->Pout = pid->Iout = pid->Dout = pid->out = 0.0f;
}

float PID_Calc(PidTypeDef *pid, float ref, float set)
{
    if (pid == NULL)
    {
        return 0.0f;
    }

    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->set = set;
    pid->fdb = ref;
    pid->error[0] = set - ref;
    if (pid->mode == POSITION_PID)
    {
        pid->Pout = pid->Kp * pid->error[0];
        pid->Iout += pid->Ki * pid->error[0];
        pid->Dbuf[2] = pid->Dbuf[1];
        pid->Dbuf[1] = pid->Dbuf[0];
        pid->Dbuf[0] = (pid->error[0] - pid->error[1]);
        pid->Dout = pid->Kd * pid->Dbuf[0];
        LimitMax(pid->Iout, pid->max_iout);
        pid->out = pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);
    }
    else if (pid->mode == DELTA_PID)
    {
        pid->Pout = pid->Kp * (pid->error[0] - pid->error[1]);
        pid->Iout = pid->Ki * pid->error[0];
        pid->Dbuf[2] = pid->Dbuf[1];
        pid->Dbuf[1] = pid->Dbuf[0];
        pid->Dbuf[0] = (pid->error[0] - 2.0f * pid->error[1] + pid->error[2]);
        pid->Dout = pid->Kd * pid->Dbuf[0];
        pid->out += pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);
    }
    return pid->out;
}

void PID_clear(PidTypeDef *pid)
{
    if (pid == NULL)
    {
        return;
    }

    pid->error[0] = pid->error[1] = pid->error[2] = 0.0f;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->out = pid->Pout = pid->Iout = pid->Dout = 0.0f;
    pid->fdb = pid->set = 0.0f;
}



void abs_limit(float *a, float ABS_MAX)
{
    if (*a > ABS_MAX)
        *a = ABS_MAX;
    if (*a < -ABS_MAX)
        *a = -ABS_MAX;
}

static void pid_param_init(
    pid_t 		*pid,
    uint8_t  	mode,
    uint32_t 	maxout,
    uint32_t	intergral_limit,

    float 		kp,
    float		ki,
    float 		kd)
{
    pid->max_out		 = maxout;
    pid->pid_mode		 = mode;
    pid->intergral_limit = intergral_limit;

    pid->p = kp;
    pid->i = ki;
    pid->d = kd;
}

static void pid_reset(pid_t *pid, float kp, float ki, float kd)
{
    pid->p = kp;
    pid->i = ki;
    pid->d = kd;

    pid->pout = 0;
    pid->iout = 0;
    pid->dout = 0;
    pid->out = 0;

}

float pid_calc(pid_t *pid, float get, float set)
{
    pid->get = get;
    pid->set = set;
    pid->err[NOW] = set - get;

    if (pid->pid_mode == POSITION_PID) //position PID
    {
        pid->pout  = pid->p * pid->err[NOW];					//P计算
        pid->iout += pid->i * pid->err[NOW];					//积分计算
        pid->dout  = pid->d * (pid->err[NOW] - pid->err[LAST]);//误差计算

        abs_limit(&(pid->iout), pid->intergral_limit); //设置积分上限
        pid->out = pid->pout + pid->iout + pid->dout;
        abs_limit(&(pid->out), pid->max_out); //设置调整上限
    }
    else if (pid->pid_mode == DELTA_PID) //delta PID
    {
        pid->pout = pid->p * (pid->err[NOW] - pid->err[LAST]);
        pid->iout = pid->i * pid->err[NOW];
        pid->dout = pid->d * (pid->err[NOW] - 2 * pid->err[LAST] + pid->err[LLAST]);

        pid->out += pid->pout + pid->iout + pid->dout;
        abs_limit(&(pid->out), pid->max_out);
    }
    pid->err[LLAST] = pid->err[LAST];
    pid->err[LAST]  = pid->err[NOW];

    return pid->out;
}


float loop_float_constrain(float Input, float minValue, float maxValue)
{
    if (maxValue < minValue)
    {
        return Input;
    }

    if (Input > maxValue)
    {
        float len = maxValue - minValue;
        while (Input > maxValue)
        {
            Input -= len;
        }
    }
    else if (Input < minValue)
    {
        float len = maxValue - minValue;
        while (Input < minValue)
        {
            Input += len;
        }
    }
    return Input;
}

float rad_format(float Ang)
{
    return loop_float_constrain(Ang,-3.14159265358979f, 3.14159265358979f);
}

float GIMBAL_PID_Calc(pid_t *pid, float get, float set, float error_delta)
{

    pid->get = get;
    pid->set = set;

    pid->err[NOW] = set - get;
    pid->err[NOW] = rad_format(pid->err[NOW]);
    pid->pout = pid->p * pid->err[NOW];
    pid->iout += pid->i * pid->err[NOW];
    pid->dout = pid->d * error_delta;

    abs_limit(&pid->iout, pid->intergral_limit);
    pid->out = pid->pout + pid->iout + pid->dout;
    abs_limit(&pid->out, pid->max_out);
    return pid->out;
}

void PID_Struct_Init(
    pid_t 		*pid,
    uint8_t		mode,
    uint32_t 	maxout,
    uint32_t	intergral_limit,

    float 		kp,
    float		  ki,
    float 		kd)
{
    pid->pid_param_init = pid_param_init;
    pid->pid_reset = pid_reset;

    pid->pid_param_init(pid,mode,maxout,intergral_limit,kp,ki,kd);
    pid->pid_reset(pid,kp,ki,kd);

}


pid_t  *pid_point_init(pid_t 		*pid,
                       uint8_t		mode,
                       uint32_t 	maxout,
                       uint32_t	intergral_limit,

                       float 		kp,
                       float		ki,
                       float 		kd)
{
    PID_Struct_Init(pid,mode,maxout,intergral_limit,kp,ki,kd);
    return pid;
}

