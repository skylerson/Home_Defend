#include "ds1302.h"


ds1302_time_info_t ds1302_time_info;

//---DS1302д��Ͷ�ȡʱ����ĵ�ַ����---//
//---���ʱ�������� ���λ��дλ;-------//
const uint8_t READ_RTC_ADDR[7] = {0x81, 0x83, 0x85, 0x87, 0x89, 0x8b, 0x8d};
const uint8_t WRITE_RTC_ADDR[7] = {0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c};


//---DS1302ʱ�ӳ�ʼ��2016��5��7��������12��00��00�롣---//
//---�洢˳�������ʱ��������,�洢��ʽ����BCD��---//
uint8_t TIME[7] = {0, 0x04, 0x12, 0x06, 0x03, 0x07, 0x22};


/*******************************************************************************
* �� �� ��         : Ds1302Write
* ��������		   : ��DS1302�����ַ+���ݣ�
* ��    ��         : addr,dat
* ��    ��         : ��
*******************************************************************************/

void Ds1302Write(uint8_t addr, uint8_t dat)
{
    uint8_t n;
    DS1302_RST = 0;
    __NOP();
    DS1302_IIC_SCL = 0;//�Ƚ�SCLK�õ͵�ƽ��
    __NOP();
    DS1302_RST = 1; //Ȼ��RST(CE)�øߵ�ƽ��
    __NOP();

    DS1302_SDA_OUT(); //SDIN �������

    for (n=0; n<8; n++)//��ʼ���Ͱ�λ��ַ����
    {
			
			DS1302_IIC_SDA =  addr & 0x01; //���ݴӵ�λ��ʼ����
      addr >>= 1;

        DS1302_IIC_SCL = 1;  //������������ʱ��DS1302��ȡ����
        __NOP();
        DS1302_IIC_SCL = 0;
        __NOP();
    }
    for (n=0; n<8; n++)//д��8λ����
    {
			DS1302_IIC_SDA =  dat & 0x01; //���ݴӵ�λ��ʼ����
        dat >>= 1;
        DS1302_IIC_SCL = 1;  //������������ʱ��DS1302��ȡ����
        __NOP();
        DS1302_IIC_SCL = 0;
        __NOP();
    }

    DS1302_RST = 0;    //�������ݽ���
    __NOP();
}

/*******************************************************************************
* �� �� ��         : Ds1302Read
* ��������		   : ��ȡһ����ַ������
* ��    ��         : addr
* ��    ��         : dat
*******************************************************************************/

uint8_t Ds1302Read(uint8_t addr)
{
    uint8_t n,dat=0,dat1=0;

    DS1302_RST = 0; 
    __NOP();
   DS1302_IIC_SCL = 0;//�Ƚ�SCLK�õ͵�ƽ��
    __NOP();
    DS1302_RST = 1; //Ȼ��RST(CE)�øߵ�ƽ��
    __NOP();


    DS1302_SDA_OUT(); //SDIN �������

    for (n=0; n<8; n++)//��ʼ���Ͱ�λ��ַ����
    {
     	DS1302_IIC_SDA =  addr & 0x01; //���ݴӵ�λ��ʼ����

        addr >>= 1;

        DS1302_IIC_SCL = 1;  //������������ʱ��DS1302��ȡ����
        __NOP();
        DS1302_IIC_SCL = 0;
        __NOP();
    }
		
    DS1302_SDA_IN();
    __NOP();
    for(n=0; n<8; n++)//��ȡ8λ����
    {
        dat1 = DS1302_READ_SDA;//�����λ��ʼ����
        dat = (dat>>1) | (dat1<<7);

        DS1302_IIC_SCL = 1;  //������������ʱ��DS1302��ȡ����
        __NOP();
        DS1302_IIC_SCL = 0;
        __NOP();
    }

    DS1302_RST = 0;//����ΪDS1302��λ���ȶ�ʱ��,����ġ�
    __NOP();

   DS1302_IIC_SCL = 1;
    __NOP();

     DS1302_SDA_OUT(); //SDIN ������� 

    DS1302_IIC_SDA = 0;
    __NOP();
   DS1302_IIC_SDA = 1;
    __NOP();
    return dat;
}


/*******************************************************************************
* �� �� ��         : Ds1302Init
* ��������		   : ��ʼ��DS1302.
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/

void Ds1302Init(void)
{
    uint8_t n;

    /* Define the init structure for the output Ds1302 pin*/
		GPIO_InitTypeDef  GPIO_InitStructure;
			
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);//��ʹ������IO PORTBʱ�� 
				
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;	 // �˿�����
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIO 
			
		GPIO_SetBits(GPIOC,GPIO_Pin_14|GPIO_Pin_15);						 //PB11 �����	
	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	 // �˿�����
	  GPIO_Init(GPIOA, &GPIO_InitStructure);	
		GPIO_SetBits(GPIOA,GPIO_Pin_0);						 //PB11 �����	

//    Ds1302Write(0x8E,0X00);	//��ֹд���������ǹر�д��������
//    for (n=0; n<7; n++)//д��7���ֽڵ�ʱ���źţ�����ʱ��������
//    {
//        Ds1302Write(WRITE_RTC_ADDR[n],TIME[n]);
//    }
//    Ds1302Write(0x8E,0x80);		 //��д��������
}

/*******************************************************************************
* �� �� ��         : Ds1302ReadTime
* ��������		   : ��ȡʱ����Ϣ
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/

void Ds1302ReadTime(void)
{
    uint8_t n;
    for (n=0; n<7; n++)//��ȡ7���ֽڵ�ʱ���źţ�����ʱ��������
    {
        TIME[n] = Ds1302Read(READ_RTC_ADDR[n]);
    }

}


ds1302_time_info_t *Ds1302ReadDecTime(void)
{
    Ds1302ReadTime();

    ds1302_time_info.second=(TIME[0]>>4)*10+(TIME[0]&0x0f);
    ds1302_time_info.minute=((TIME[1]>>4)&(0x07))*10+(TIME[1]&0x0f);
    ds1302_time_info.hour=(TIME[2]>>4)*10+(TIME[2]&0x0f);
    ds1302_time_info.day=(TIME[3]>>4)*10+(TIME[3]&0x0f);
    ds1302_time_info.month=(TIME[4]>>4)*10+(TIME[4]&0x0f);
    ds1302_time_info.week=TIME[5];
    ds1302_time_info.year=(TIME[6]>>4)*10+(TIME[6]&0x0f)+2000;

    return &ds1302_time_info;
}

void Ds1302WriteDecTime(ds1302_time_info_t ds1302_time_info)
{
	uint8_t n;
	ds1302_time_info.year-=2000;
  TIME[6]= (((ds1302_time_info.year/10)&0x0f)<<4)|((ds1302_time_info.year%10)&0x0f);
	TIME[5]= ds1302_time_info.week;
	TIME[4]= (((ds1302_time_info.month/10)&0x0f)<<4)|((ds1302_time_info.month%10)&0x0f);
	TIME[3]= (((ds1302_time_info.day/10)&0x0f)<<4)|((ds1302_time_info.day%10)&0x0f);
	TIME[2]= (((ds1302_time_info.hour/10)&0x0f)<<4)|((ds1302_time_info.hour%10)&0x0f);
	TIME[1]= (((ds1302_time_info.minute/10)&0x0f)<<4)|((ds1302_time_info.minute%10)&0x0f);
	TIME[0]= (((ds1302_time_info.second/10)&0x0f)<<4)|((ds1302_time_info.second%10)&0x0f);
	
	Ds1302Write(0x8E,0X00);	//��ֹд���������ǹر�д��������
  for (n=0; n<7; n++)//д��7���ֽڵ�ʱ���źţ�����ʱ��������
  {
    Ds1302Write(WRITE_RTC_ADDR[n],TIME[n]);
  }
  Ds1302Write(0x8E,0x80);		 //��д��������

}


/*
 * �������ջ�ȡ���� https://www.cnblogs.com/fengbohello/p/3264300.html
 * ����1��7 ��������һ��������
 * 
 */
int getWeekdayByYearday( int iY, int iM, int iD )
{
	int iWeekDay = -1;
	if ( 1 == iM || 2 == iM )
	{
		iM += 12;
		iY--;
	}
	iWeekDay = (iD + 1 + 2 * iM + 3 * (iM + 1) / 5 + iY + iY / 4 - iY / 100 + iY / 400) % 7;
	if ( iWeekDay == 0 )
		iWeekDay = 7;
	return(iWeekDay);
}



