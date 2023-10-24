#include "key.h"
#include "bsp_key.h"

__IO int keytime[K_MAX]={0};//用于储存每个按键所按下的时间

/**
  * @brief          按键状态读取函数
  * @author         yangchen
	* @param[in]      key:对应按键的值
	* @param[in]      Adc_Value:对应按键ADC值(no use)
  * @retval         返回空
 **/
K_control_down KEY_get(K_control key)
{
	 if(Read_KeyGpio(key)==K_CONTROL_D)
	 {
		 return  K_CONTROL_D;
	 }
	 return  K_CONTROL_U;
}

/**
  * @brief          扫描每一个按键的函数
  * @author         ghyll
	* @param[in]      key:对应按键的值
	* @param[in]      Adc_Value:对应按键ADC值(no use)
  * @retval         返回空
  */
K_control_down Read_KeyGpio(u8 i)
{
	if(Key_Enter==i)
		{
			if(KEY_ENTER == K_CONTROL_D){return K_CONTROL_D;}
			else   											{return K_CONTROL_U;}
		}
		else 
			if(Key_Up == i)
		{
			if(KEY_UP == K_CONTROL_D)     {return K_CONTROL_D;}
			else    									    {return K_CONTROL_U;}
		}
		else if(Key_Esc ==i)
		{
			if(KEY_ESC == K_CONTROL_D)  {return K_CONTROL_D;}
			else  											{return K_CONTROL_U;}
		}
		 if(Key_Left==i)
		{
			if(KEY_LEFT == K_CONTROL_D) {return K_CONTROL_D;}
			else    										{return K_CONTROL_U;}
		}
		else if(Key_Down==i)
		{
			if(KEY_DOWN == K_CONTROL_D) {return K_CONTROL_D;}
			else 												{return K_CONTROL_U;}
		}
		else if(Key_Right==i)
		{
			if(KEY_RIGHT==K_CONTROL_D)  {return K_CONTROL_D;}
			else   											{return K_CONTROL_U;}
		}
		
		return  K_CONTROL_U;
}


/**
  * @brief          扫描每一个按键的函数
  * @author         yangchen
	* @param[in]      time:按键按下最短的时间才能被识别到
	* @param[in]      LongTime:按键超过了这个时间就识别为连续按下
  * @retval         返回空
 **/

K_control getKey(u16 time,u16 LongTime)
{
   K_control i=K_MAX;
   while(i--)
    {
      if(KEY_get(i)==K_CONTROL_U)
      {
        if(keytime[i]> time && keytime[i]<LongTime )
        {
          keytime[i]=-1;
				 return i;
        }
				if(keytime[i]>0)  
				{					
					keytime[i]=0;
				}
      }
      else if(KEY_get(i)==K_CONTROL_D) 
      {
          if(keytime[i]>LongTime)
          {
            keytime[i]=LongTime-500;
						return i;						
          }
      } 
    }
  return K_MAX;
}

/**
  * @brief          在中断中扫描按键按下的时间，需要在定时器中断中调用
  * @author         yangchen
	* @param[in]      void
  * @retval         返回空
 **/
void key_IRQHandler(void)
{ 
    K_control i = K_MAX; 
    while(i--)
    { 
      if(KEY_get(i) == K_CONTROL_D)
      {
        if(keytime[i]==-1)
        {
          keytime[i]=0;
        }
        else if( keytime[i]<30000 )
        {
          keytime[i]+=10;
        }
      } 
    }
}

















