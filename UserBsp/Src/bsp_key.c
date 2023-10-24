#include "key.h"
#include "bsp_key.h"

__IO int keytime[K_MAX]={0};//���ڴ���ÿ�����������µ�ʱ��

/**
  * @brief          ����״̬��ȡ����
  * @author         yangchen
	* @param[in]      key:��Ӧ������ֵ
	* @param[in]      Adc_Value:��Ӧ����ADCֵ(no use)
  * @retval         ���ؿ�
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
  * @brief          ɨ��ÿһ�������ĺ���
  * @author         ghyll
	* @param[in]      key:��Ӧ������ֵ
	* @param[in]      Adc_Value:��Ӧ����ADCֵ(no use)
  * @retval         ���ؿ�
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
  * @brief          ɨ��ÿһ�������ĺ���
  * @author         yangchen
	* @param[in]      time:����������̵�ʱ����ܱ�ʶ��
	* @param[in]      LongTime:�������������ʱ���ʶ��Ϊ��������
  * @retval         ���ؿ�
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
  * @brief          ���ж���ɨ�谴�����µ�ʱ�䣬��Ҫ�ڶ�ʱ���ж��е���
  * @author         yangchen
	* @param[in]      void
  * @retval         ���ؿ�
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

















