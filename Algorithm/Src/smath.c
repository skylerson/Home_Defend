#include "smath.h"



float arm_abs_float(float value)
{
	if(value<0)
	{
		return -value;
	}
	return value;
}


float arm_constraint_fp32(float value,float max,float min )
{
		if(value>max)
		{
			return  max;
		}
		else if(value <min)
		{
			return  min;
		}
		return value;
}

int arm_constraint_int32( int value, int max, int min )
{
		if(value>max)
		{
			return  max;
		}
		else if(value <min)
		{
			return  min;
		}
		return value;
}

short int arm_constraint_int16(short int value,short int max,short int min )
{
		if(value>max)
		{
			return  max;
		}
		else if(value <min)
		{
			return  min;
		}
		return value;
}

char arm_constraint_int8(char value,char max,char min )
{
		if(value>max)
		{
			return  max;
		}
		else if(value <min)
		{
			return  min;
		}
		return value;
}

short int arm_abs_int16(short int value)
{
    if(value <0)
		{
			return -value;
		}
		return value;
}

 int arm_abs_int32(int value)
{
    if(value <0)
		{
			return -value;
		}
		return value;
}

/**
  * @brief          幂函数
  * @author         ...
  * @param[in]     
  * @retval         basic 的 num次方
  */
int arm_int32_pow(int basic ,int num)
{
	int i=0,value=1;
	for(i=0;i<num;i++)
	{
		value*=basic;
	}
	return value;
}

/**
  * @brief          幂函数
  * @author         ...
  * @param[in]     
  * @retval         basic 的 num次方
  */
int arm_float_pow(float basic ,int num)
{
	int i=0;
	float value=1.0f;
	for(i=0;i<num;i++)
	{
		value*=basic;
	}
	return value;
}

/**
  * @brief          快速开方函数,求根号X分之一
  * @author         ...
  * @param[in]     
  * @retval         返回空
  */
float invSqrt(float num)
{
    float halfnum = 0.5f * num;
    float y = num;
    long i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (1.5f - (halfnum * y * y));
    return y;
}





