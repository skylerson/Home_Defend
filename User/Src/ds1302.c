#include "ds1302.h"


ds1302_time_info_t ds1302_time_info;

//---DS1302写入和读取时分秒的地址命令---//
//---秒分时日月周年 最低位读写位;-------//
const uint8_t READ_RTC_ADDR[7] = {0x81, 0x83, 0x85, 0x87, 0x89, 0x8b, 0x8d};
const uint8_t WRITE_RTC_ADDR[7] = {0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c};


//---DS1302时钟初始化2016年5月7日星期六12点00分00秒。---//
//---存储顺序是秒分时日月周年,存储格式是用BCD码---//
uint8_t TIME[7] = {0, 0x04, 0x12, 0x06, 0x03, 0x07, 0x22};


/*******************************************************************************
* 函 数 名         : Ds1302Write
* 函数功能		   : 向DS1302命令（地址+数据）
* 输    入         : addr,dat
* 输    出         : 无
*******************************************************************************/

void Ds1302Write(uint8_t addr, uint8_t dat)
{
    uint8_t n;
    DS1302_RST = 0;
    __NOP();
    DS1302_IIC_SCL = 0;//先将SCLK置低电平。
    __NOP();
    DS1302_RST = 1; //然后将RST(CE)置高电平。
    __NOP();

    DS1302_SDA_OUT(); //SDIN 输出配置

    for (n=0; n<8; n++)//开始传送八位地址命令
    {
			
			DS1302_IIC_SDA =  addr & 0x01; //数据从低位开始传送
      addr >>= 1;

        DS1302_IIC_SCL = 1;  //数据在上升沿时，DS1302读取数据
        __NOP();
        DS1302_IIC_SCL = 0;
        __NOP();
    }
    for (n=0; n<8; n++)//写入8位数据
    {
			DS1302_IIC_SDA =  dat & 0x01; //数据从低位开始传送
        dat >>= 1;
        DS1302_IIC_SCL = 1;  //数据在上升沿时，DS1302读取数据
        __NOP();
        DS1302_IIC_SCL = 0;
        __NOP();
    }

    DS1302_RST = 0;    //传送数据结束
    __NOP();
}

/*******************************************************************************
* 函 数 名         : Ds1302Read
* 函数功能		   : 读取一个地址的数据
* 输    入         : addr
* 输    出         : dat
*******************************************************************************/

uint8_t Ds1302Read(uint8_t addr)
{
    uint8_t n,dat=0,dat1=0;

    DS1302_RST = 0; 
    __NOP();
   DS1302_IIC_SCL = 0;//先将SCLK置低电平。
    __NOP();
    DS1302_RST = 1; //然后将RST(CE)置高电平。
    __NOP();


    DS1302_SDA_OUT(); //SDIN 输出配置

    for (n=0; n<8; n++)//开始传送八位地址命令
    {
     	DS1302_IIC_SDA =  addr & 0x01; //数据从低位开始传送

        addr >>= 1;

        DS1302_IIC_SCL = 1;  //数据在上升沿时，DS1302读取数据
        __NOP();
        DS1302_IIC_SCL = 0;
        __NOP();
    }
		
    DS1302_SDA_IN();
    __NOP();
    for(n=0; n<8; n++)//读取8位数据
    {
        dat1 = DS1302_READ_SDA;//从最低位开始接收
        dat = (dat>>1) | (dat1<<7);

        DS1302_IIC_SCL = 1;  //数据在上升沿时，DS1302读取数据
        __NOP();
        DS1302_IIC_SCL = 0;
        __NOP();
    }

    DS1302_RST = 0;//以下为DS1302复位的稳定时间,必须的。
    __NOP();

   DS1302_IIC_SCL = 1;
    __NOP();

     DS1302_SDA_OUT(); //SDIN 输出配置 

    DS1302_IIC_SDA = 0;
    __NOP();
   DS1302_IIC_SDA = 1;
    __NOP();
    return dat;
}


/*******************************************************************************
* 函 数 名         : Ds1302Init
* 函数功能		   : 初始化DS1302.
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void Ds1302Init(void)
{
    uint8_t n;

    /* Define the init structure for the output Ds1302 pin*/
		GPIO_InitTypeDef  GPIO_InitStructure;
			
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);//先使能外设IO PORTB时钟 
				
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;	 // 端口配置
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIO 
			
		GPIO_SetBits(GPIOC,GPIO_Pin_14|GPIO_Pin_15);						 //PB11 输出高	
	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	 // 端口配置
	  GPIO_Init(GPIOA, &GPIO_InitStructure);	
		GPIO_SetBits(GPIOA,GPIO_Pin_0);						 //PB11 输出高	

//    Ds1302Write(0x8E,0X00);	//禁止写保护，就是关闭写保护功能
//    for (n=0; n<7; n++)//写入7个字节的时钟信号：分秒时日月周年
//    {
//        Ds1302Write(WRITE_RTC_ADDR[n],TIME[n]);
//    }
//    Ds1302Write(0x8E,0x80);		 //打开写保护功能
}

/*******************************************************************************
* 函 数 名         : Ds1302ReadTime
* 函数功能		   : 读取时钟信息
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void Ds1302ReadTime(void)
{
    uint8_t n;
    for (n=0; n<7; n++)//读取7个字节的时钟信号：分秒时日月周年
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
	
	Ds1302Write(0x8E,0X00);	//禁止写保护，就是关闭写保护功能
  for (n=0; n<7; n++)//写入7个字节的时钟信号：分秒时日月周年
  {
    Ds1302Write(WRITE_RTC_ADDR[n],TIME[n]);
  }
  Ds1302Write(0x8E,0x80);		 //打开写保护功能

}


/*
 * 由年月日获取星期 https://www.cnblogs.com/fengbohello/p/3264300.html
 * 返回1到7 代表星期一到星期天
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



