
#include "oled.h"

#include "bsp_oled.h"
#include "oledfont.h"
#include "delay.h"
#include "math.h"

//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
u8 OLED_GRAM[128][8];	 
//�����Դ浽OLED		 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}

//��ʼ�ź�
void I2C_Start(void)
{
	OLED_SDIN_Set();
	OLED_SCLK_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}

//�����ź�
void I2C_Stop(void)
{
	OLED_SCLK_Set();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
}

//�ȴ��ź���Ӧ
void I2C_WaitAck(void) //�������źŵĵ�ƽ
{
	OLED_SCLK_Set();
	OLED_SCLK_Clr();
}

//д��һ���ֽ�
void Send_Byte(u8 dat)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		OLED_SCLK_Clr();//��ʱ���ź�����Ϊ�͵�ƽ
		if(dat&0x80)//��dat��8λ�����λ����д��
		{
			OLED_SDIN_Set();
    }
		else
		{
			OLED_SDIN_Clr();
    }
		OLED_SCLK_Set();//��ʱ���ź�����Ϊ�ߵ�ƽ
		OLED_SCLK_Clr();//��ʱ���ź�����Ϊ�͵�ƽ
		dat<<=1;
  }
}

//����һ���ֽ�
//��SSD1306д��һ���ֽڡ�
//mode:����/�����־ 0,��ʾ����;1,��ʾ����;
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
//����OLED��ʾ    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}


//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	


//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
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
	OLED_Refresh_Gram();//������ʾ
}


//���Ժ���
void OLED_ColorTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,OLED_CMD);//������ʾ
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,OLED_CMD);//��ɫ��ʾ
		}
}

//��Ļ��ת180��
void OLED_DisplayTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,OLED_CMD);//������ʾ
			OLED_WR_Byte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);//��ת��ʾ
			OLED_WR_Byte(0xA0,OLED_CMD);
		}
}

//--------------------------------------------------------------------------------
//������:OLED_DrawPoint
//��������:���� 
//�β�:x:0~127
//�β�:y:0~63
//�β�:t:1 ��� 0,���
//--------------------------------------------------------------------------------
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=y/8;
	bx=y%8;
	temp=1<<(bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
//OLED������ú���

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
}  


//void OLED_Set_Pos(unsigned char x, unsigned char y)//�����������������ã��ڱ�ĳ�����
//{ 
//  OLED_WR_Byte(0xb0+y,OLED_CMD);
//  OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
//  OLED_WR_Byte((x&0x0f),OLED_CMD);
//}

//--------------------------------------------------------------------------------
//������:OLED_Line_B                                             
//��������:������Ƕȵ�ֱ��                                                             
//����:  x(0~127)                                  
//������y(0~63)                                                        
//����:  color  :1��ʾ�����ߣ�0��ʾ������                            
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
//������:OLED_ShowLine
//��������:��һ������Ƕȵ��� 
//�β�:x1,x2:0~127
//�β�:y1,y2:0~63
//����:�ú�������x1<=x2��x1>x2;����ֻ����y1<=y2,�������ʾ����
//--------------------------------------------------------------------------------
void OLED_ShowLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		OLED_DrawPoint(uRow,uCol,dot);//���� 
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
//������:draw_fill_circle
//��������:��һ��ʵ��Բ
//����: x(0~127)                                  
//������y(0~63)  
//����:r(0~32)
//����:  dot  :1��ʾ�����ߣ�0��ʾ������                            
//--------------------------------------------------------------------------------
void draw_fill_circle(u8 x0,u8 y0,u8 r,u8 dot)//Բ��(x0,y0),�뾶r
{	
		u8 x = 0,y = 0,R = 0;
		for(x = x0-r;x <= x0+r;x++){
			for(y = y0-r; y <= y0+r ;y++ ){
				R = sqrt(pow(r,2)-pow(x-x0,2))+y0; //Բ���̣�x,y����	
				if( (y >= y0 && y <= R) || (y < y0 && y >= 2*y0-R )|| dot == 0 ) 
				{  //��������Բ������
					OLED_DrawPoint(x,y,dot);
				}	
			}
		}
}

//--------------------------------------------------------------------------------
//������:OLED_Fill
//��������:������ѡ����������
//�β�:x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	
//�������ͣ���
//--------------------------------------------------------------------------------
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)
		{
			OLED_DrawPoint(x,y,dot);//���㺯��
		}
	}													    
	OLED_Refresh_Gram();//������ʾ
}
//--------------------------------------------------------------------------------
//������:OLED_ShowChar
//�������ã���ָ��λ����ʾһ���ַ�,���������ַ�
//�β�x:0~127
//�β�y:0~63
//�β�mode:0,������ʾ; 1,������ʾ				 
//�β�size:ѡ������ 12/16/24
//�������ͣ���
//���:����mode ��0��1:0�Ļ������ó���a���������������,��1�Ļ��պ��෴��
//--------------------------------------------------------------------------------
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	 u8 temp,t,t1;
	 u8 y0=y;
	 u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	
		chr=chr-' ';	//�õ�ƫ�ƺ��ֵ	
    for(t=0;t<csize;t++)
    {   
			if(size==12)         temp=asc2_1206[chr][t]; 	 	//����1206����
			else if(size==16)    temp=asc2_1608[chr][t];	//����1608����
			else if(size==24)
			{				                                   //����2412���� 
				temp=asc2_2412[chr][t];	
			}else if(size == 32)
			{ temp = ascii_3232[chr][t] ;}		
			else if(size == 48)
			{ temp = ascii_4848[chr][t];}	
			else return;								              //û�е��ֿ�
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
//������:mypow
//�������ã�ָ��������m^n������
//�β�m:����
//�β�n:ָ��
//�������ͣ��޷��ų����ͣ�result����
//--------------------------------------------------------------------------------
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}		

//--------------------------------------------------------------------------------
//������:OLED_ShowNum
//�������ܣ���ʾ2������
//�β�:x,y :�������	 
//�β�:len :���ֵ�λ��
//�β�:size:�����С
//�β�:mode:ģʽ	0,���ģʽ;1,����ģʽ
//�β�:num:��ֵ(0~4294967295);
//����ֵ����:��
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
//������:OLED_ShowString
//�������ã���ʾ�ַ���
//�β�x,y:�������  
//�β�size:�����С 
//�β�*p:�ַ�����ʼ��ַ 
//�������ͣ���
//��⣺size����ѡ������Ĵ�С������ֻ������ѡ�񷽷�����һ�����浱�п���д���У�����һ��Ҫ����һ�е���������size�Ĵ�С��
//--------------------------------------------------------------------------------
void OLED_ShowString(u8 x,u8 y,const char *p,u8 size,u8 mode)
{	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,mode);	 
        x+=size/2;
        p++;
    }  
}	  
//--------------------------------------------------------------------------------
//������:OLED_ShowGBK
//�������ã���ʾһ������                       
//�β�x,y:�������  
//�β�size:�����С 
//�β�num :��ʾ�ֿ��еĵڼ�����
//�β�:mode:ģʽ	0,���ģʽ;1,����ģʽ
//--------------------------------------------------------------------------------
void OLED_ShowGBK(u8 x, u8 y,  u8 num, u8 size,u8 mode)
{
    u8 temp,t,t1;
    u8 y0=y;
    u8 csize=(size/8 + ((size%8)?1:0)) * size; //�õ�ÿһ���ַ�����Ӧ���󼯵��ֽ���
    for(t=0;t<csize;t++)
    {  
        if(size==12)      temp = gbk_1212[num][t];    //����1212����
        else if(size==16) temp = gbk_1616[num][t];    //����1616����
				else if(size==24) temp = gbk_2412[num][t];    //����2412����
        else return;                                  //û�е�����
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
//������:OLED_ShowGBKString
//�������ã���ʾһ������                       
//�β�x,y:�������  
//�β�size:�����С 
//�β�*p :�ַ������������ֿ��е�˳��
//�β�:mode:ģʽ	0,���ģʽ;1,����ģʽ
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
//������:OLED_DrawBMP
//�������ã���ʾһ��ͼƬ                       
//�β�x0,y0:�������,x1,y1:�յ����� 
//�β�BMP[] :ͼƬ��
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

//x0,y0���������
//x1,y1���յ�����
//BMP[]��Ҫд���ͼƬ����
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
//��ʼ��SSD1306,����GPIO��ģ��SPIͨ��			    
void OLED_Init(void)
{ 
	Oled_Gpio_Init();
//	OLED_DC=1;
//	OLED_RST=0;
//	delay_ms(50);
//	OLED_RST=1;
	
//	OLED_WR_Byte(0xAE,OLED_CMD); //�ر���ʾ
//	OLED_WR_Byte(0xD5,OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
//	OLED_WR_Byte(80,OLED_CMD);   //[3:0],��Ƶ����;[7:4],��Ƶ��
//	OLED_WR_Byte(0xA8,OLED_CMD); //��������·��
//	OLED_WR_Byte(0X3F,OLED_CMD); //Ĭ��0X3F(1/64) 
//	OLED_WR_Byte(0xD3,OLED_CMD); //������ʾƫ��
//	OLED_WR_Byte(0X00,OLED_CMD); //Ĭ��Ϊ0
//	
//	OLED_WR_Byte(0x40,OLED_CMD); //������ʾ��ʼ�� [5:0],����.
//	

													    
	OLED_WR_Byte(0x8D,OLED_CMD); //��ɱ�����
	OLED_WR_Byte(0x14,OLED_CMD); //bit2������/�ر�
	OLED_WR_Byte(0x20,OLED_CMD); //�����ڴ��ַģʽ
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	
	OLED_WR_Byte(0xA1,OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;                                     0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xC8,OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·�� 0xc0���·��� 0xc8���� 
	
	OLED_WR_Byte(0xDA,OLED_CMD); //����COMӲ����������
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]����

	OLED_WR_Byte(0x81,OLED_CMD); //�Աȶ�����
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED_WR_Byte(0xD9,OLED_CMD); //����Ԥ�������
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //����VCOMH ��ѹ����
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_Byte(0xA6,OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //������ʾ	 



//	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
//	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
//	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
//	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
//	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
//	OLED_WR_Byte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
//	
//	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
//	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
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

