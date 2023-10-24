
#include "oled.h"

#include "bsp_oled.h"
#include "oledfont.h"
#include "delay.h"
#include "math.h"

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
u8 OLED_GRAM[128][8];	 
//更新显存到OLED		 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}

//起始信号
void I2C_Start(void)
{
	OLED_SDIN_Set();
	OLED_SCLK_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}

//结束信号
void I2C_Stop(void)
{
	OLED_SCLK_Set();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
}

//等待信号响应
void I2C_WaitAck(void) //测数据信号的电平
{
	OLED_SCLK_Set();
	OLED_SCLK_Clr();
}

//写入一个字节
void Send_Byte(u8 dat)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		OLED_SCLK_Clr();//将时钟信号设置为低电平
		if(dat&0x80)//将dat的8位从最高位依次写入
		{
			OLED_SDIN_Set();
    }
		else
		{
			OLED_SDIN_Clr();
    }
		OLED_SCLK_Set();//将时钟信号设置为高电平
		OLED_SCLK_Clr();//将时钟信号设置为低电平
		dat<<=1;
  }
}

//发送一个字节
//向SSD1306写入一个字节。
//mode:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 mode)
{
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	if(mode){Send_Byte(0x40);}
  else{Send_Byte(0x00);}
	I2C_WaitAck();
	Send_Byte(dat);
	I2C_WaitAck();
	I2C_Stop();
}	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}


//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	


//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)
	{
		for(n=0;n<128;n++)
		{
			OLED_GRAM[n][i]=0X00;
		}
	}		
	OLED_Refresh_Gram();//更新显示
}


//反显函数
void OLED_ColorTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
		}
}

//屏幕旋转180度
void OLED_DisplayTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
			OLED_WR_Byte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
			OLED_WR_Byte(0xA0,OLED_CMD);
		}
}

//--------------------------------------------------------------------------------
//函数名:OLED_DrawPoint
//函数功能:画点 
//形参:x:0~127
//形参:y:0~63
//形参:t:1 填充 0,清空
//--------------------------------------------------------------------------------
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=y/8;
	bx=y%8;
	temp=1<<(bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
//OLED光标设置函数

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
}  


//void OLED_Set_Pos(unsigned char x, unsigned char y)//这两个函数另有他用，在别的程序当中
//{ 
//  OLED_WR_Byte(0xb0+y,OLED_CMD);
//  OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
//  OLED_WR_Byte((x&0x0f),OLED_CMD);
//}

//--------------------------------------------------------------------------------
//函数名:OLED_Line_B                                             
//函数功能:划任意角度的直线                                                             
//参数:  x(0~127)                                  
//参数：y(0~63)                                                        
//参数:  color  :1表示画白线；0表示画黑线                            
//--------------------------------------------------------------------------------

#define abs(x) x>=0?x:-x

void OLED_Line_B(int x1, int y1, int x2, int y2,u8 color)
{
	int x = x1;
	int y = y1;
	int dx = abs(x2 - x1) ;
	int dy = abs(y2 - y1) ;
	int s1 = x2 > x1 ? 1 : -1;
	int s2 = y2 > y1 ? 1 : -1;
	int p;
	int temp,i;
	char interchange = 0;	
	if (dy > dx)				
	{
		 temp = dx;
		dx = dy;
		dy = temp;
		interchange = 1;
	}

	 p = 2 * dy - dx;
	for( i = 0; i <= dx; i++)
	{
		OLED_DrawPoint(x,y,color);
		if (p >= 0)
		{
			if (!interchange)		
				y += s2;
			else		
				x += s1;
			p -= 2 * dx;
		}
		if (!interchange)
			x += s1;				
		else
			y += s2;				
		p += 2 * dy;
	}
} 


//--------------------------------------------------------------------------------
//函数名:OLED_ShowLine
//函数功能:画一条任意角度的线 
//形参:x1,x2:0~127
//形参:y1,y2:0~63
//声明:该函数允许x1<=x2和x1>x2;但是只允许y1<=y2,否则会显示错误。
//--------------------------------------------------------------------------------
void OLED_ShowLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		OLED_DrawPoint(uRow,uCol,dot);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}

//--------------------------------------------------------------------------------
//函数名:draw_fill_circle
//函数功能:画一个实心圆
//参数: x(0~127)                                  
//参数：y(0~63)  
//参数:r(0~32)
//参数:  dot  :1表示画白线；0表示画黑线                            
//--------------------------------------------------------------------------------
void draw_fill_circle(u8 x0,u8 y0,u8 r,u8 dot)//圆心(x0,y0),半径r
{	
		u8 x = 0,y = 0,R = 0;
		for(x = x0-r;x <= x0+r;x++){
			for(y = y0-r; y <= y0+r ;y++ ){
				R = sqrt(pow(r,2)-pow(x-x0,2))+y0; //圆方程，x,y反置	
				if( (y >= y0 && y <= R) || (y < y0 && y >= 2*y0-R )|| dot == 0 ) 
				{  //点限制在圆方程内
					OLED_DrawPoint(x,y,dot);
				}	
			}
		}
}

//--------------------------------------------------------------------------------
//函数名:OLED_Fill
//函数功能:对于所选区域进行填充
//形参:x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	
//返回类型：无
//--------------------------------------------------------------------------------
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)
		{
			OLED_DrawPoint(x,y,dot);//画点函数
		}
	}													    
	OLED_Refresh_Gram();//更新显示
}
//--------------------------------------------------------------------------------
//函数名:OLED_ShowChar
//函数作用：在指定位置显示一个字符,包括部分字符
//形参x:0~127
//形参y:0~63
//形参mode:0,反白显示; 1,正常显示				 
//形参size:选择字体 12/16/24
//返回类型：无
//理解:对于mode 的0和1:0的话，是让除了a以外的区域都亮起来,而1的话刚好相反。
//--------------------------------------------------------------------------------
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	 u8 temp,t,t1;
	 u8 y0=y;
	 u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	
		chr=chr-' ';	//得到偏移后的值	
    for(t=0;t<csize;t++)
    {   
			if(size==12)         temp=asc2_1206[chr][t]; 	 	//调用1206字体
			else if(size==16)    temp=asc2_1608[chr][t];	//调用1608字体
			else if(size==24)
			{				                                   //调用2412字体 
				temp=asc2_2412[chr][t];	
			}else if(size == 32)
			{ temp = ascii_3232[chr][t] ;}		
			else if(size == 48)
			{ temp = ascii_4848[chr][t];}	
			else return;								              //没有的字库
			for(t1=0;t1<8;t1++)
			{
				if(temp&0x80) OLED_DrawPoint(x,y,mode);
				else          OLED_DrawPoint(x,y,!mode);
				temp<<=1;
				y++;
				if((y-y0)==size)
				{
					y=y0;
					x++;
					break;
				}
		  }  	 
    } 		
}


//--------------------------------------------------------------------------------
//函数名:mypow
//函数作用：指数函数（m^n函数）
//形参m:底数
//形参n:指数
//返回类型：无符号长整型（result）；
//--------------------------------------------------------------------------------
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}		

//--------------------------------------------------------------------------------
//函数名:OLED_ShowNum
//函数功能：显示2个数字
//形参:x,y :起点坐标	 
//形参:len :数字的位数
//形参:size:字体大小
//形参:mode:模式	0,填充模式;1,叠加模式
//形参:num:数值(0~4294967295);
//返回值类型:无
//--------------------------------------------------------------------------------
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 
//--------------------------------------------------------------------------------
//函数名:OLED_ShowString
//函数作用：显示字符串
//形参x,y:起点坐标  
//形参size:字体大小 
//形参*p:字符串起始地址 
//返回类型：无
//理解：size可以选择字体的大小，但是只有三种选择方法，在一个界面当中可以写多行，但下一行要是上一行的行数加上size的大小。
//--------------------------------------------------------------------------------
void OLED_ShowString(u8 x,u8 y,const char *p,u8 size,u8 mode)
{	
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,mode);	 
        x+=size/2;
        p++;
    }  
}	  
//--------------------------------------------------------------------------------
//函数名:OLED_ShowGBK
//函数作用：显示一个汉字                       
//形参x,y:起点坐标  
//形参size:字体大小 
//形参num :显示字库中的第几个字
//形参:mode:模式	0,填充模式;1,叠加模式
//--------------------------------------------------------------------------------
void OLED_ShowGBK(u8 x, u8 y,  u8 num, u8 size,u8 mode)
{
    u8 temp,t,t1;
    u8 y0=y;
    u8 csize=(size/8 + ((size%8)?1:0)) * size; //得到每一个字符所对应点阵集的字节数
    for(t=0;t<csize;t++)
    {  
        if(size==12)      temp = gbk_1212[num][t];    //调用1212字体
        else if(size==16) temp = gbk_1616[num][t];    //调用1616字体
				else if(size==24) temp = gbk_2412[num][t];    //调用2412字体
        else return;                                  //没有的字体
				for(t1=0;t1<8;t1++)
        {
            if(temp&0x80)OLED_DrawPoint(x,y,mode);
            else OLED_DrawPoint(x,y,!mode);
            temp<<=1;
            y++;
            if((y-y0)==size)
            {
                y=y0;
                x++;
                break;
            }
        }    
    } 	
}

//--------------------------------------------------------------------------------
//函数名:OLED_ShowGBKString
//函数作用：显示一串汉字                       
//形参x,y:起点坐标  
//形参size:字体大小 
//形参*p :字符串，汉字在字库中的顺序
//形参:mode:模式	0,填充模式;1,叠加模式
//--------------------------------------------------------------------------------
void OLED_ShowGBKString(u8 x,u8 y,const u8 *p,u8 size,u8 mode)
{
		while(*p!='\0')
		{
				 if(x>(128-(size/2))){x=0;y+=size;}
					if(y>(64-size)){y=x=0;OLED_Clear();}
					OLED_ShowGBK(x,y,(u8)(*p-'0'),size,mode);
					x+=size;
					p++;
		}
}
//--------------------------------------------------------------------------------
//函数名:OLED_DrawBMP
//函数作用：显示一幅图片                       
//形参x0,y0:起点坐标,x1,y1:终点坐标 
//形参BMP[] :图片库
//--------------------------------------------------------------------------------
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x=0,y=0;
  
  if(y1%8==0)
     y=y1/8;      
  else
     y=y1/8+1;
	
	for(y=y0;y<y1;y++)
	{
		   OLED_Set_Pos(x0,y);
       for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
	    }
	}

}

//x0,y0：起点坐标
//x1,y1：终点坐标
//BMP[]：要写入的图片数组
void OLED_ShowPicture(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t BMP[])
{
	u32 j=0;
	u8 x=0,y=0;
	if(y%8==0)y=0;
	else y+=1;
	for(y=y0;y<y1;y++)
	 {
		 OLED_Set_Pos(x0,y);
		 for(x=x0;x<x1;x++)
		 {
			 OLED_WR_Byte(BMP[j],OLED_DATA);
			 j++;
     }
	 }
}
//初始化SSD1306,采用GPIO口模拟SPI通信			    
void OLED_Init(void)
{ 
	Oled_Gpio_Init();
//	OLED_DC=1;
//	OLED_RST=0;
//	delay_ms(50);
//	OLED_RST=1;
	
//	OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示
//	OLED_WR_Byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
//	OLED_WR_Byte(80,OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
//	OLED_WR_Byte(0xA8,OLED_CMD); //设置驱动路数
//	OLED_WR_Byte(0X3F,OLED_CMD); //默认0X3F(1/64) 
//	OLED_WR_Byte(0xD3,OLED_CMD); //设置显示偏移
//	OLED_WR_Byte(0X00,OLED_CMD); //默认为0
//	
//	OLED_WR_Byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
//	

													    
	OLED_WR_Byte(0x8D,OLED_CMD); //电荷泵设置
	OLED_WR_Byte(0x14,OLED_CMD); //bit2，开启/关闭
	OLED_WR_Byte(0x20,OLED_CMD); //设置内存地址模式
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	
	OLED_WR_Byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;                                     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数 0xc0上下反置 0xc8正常 
	
	OLED_WR_Byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]配置

	OLED_WR_Byte(0x81,OLED_CMD); //对比度设置
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
	OLED_WR_Byte(0xD9,OLED_CMD); //设置预充电周期
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_Byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //开启显示	 



//	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
//	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
//	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
//	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
//	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
//	OLED_WR_Byte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
//	
//	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
//	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
//	
//	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
//	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
//	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
//	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
//	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
//	
//	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
//	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
//	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
//	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
//	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
//	OLED_WR_Byte(0x12,OLED_CMD);
//	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
//	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
//	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
//	OLED_WR_Byte(0x02,OLED_CMD);//
//	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
//	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
//	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
//	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
//	OLED_WR_Byte(0xAF,OLED_CMD);
	OLED_Clear();
}  

