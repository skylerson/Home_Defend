#ifndef __SMATH_H
#define __SMATH_H

#include "main.h"

#define arm_abs(x) (x>=0)? (x) : (-x)
				
#define ValueLimt(current,maxLimt,minLimt)  \
				{															      \
					if(current>maxLimt)						    \
					  {														    \
							current=maxLimt;					    \
						}														    \
					else if(current<minLimt)			    \
					{															    \
						current=minLimt;						    \
					}															    \
				}

//constraint		
float arm_constraint_fp32(float value,float max,float min );
int arm_constraint_int32( int value, int max, int min );
short int arm_constraint_int16(short int value,short int max,short int min );				
char arm_constraint_int8(char value,char max,char min );				
//abs				
short int arm_abs_int16(short int value);
float arm_abs_float(float value);				
				
int arm_int32_pow(int basic ,int num);
int arm_float_pow(float basic ,int num);
float invSqrt(float num);

#endif

