#ifndef __BSP_OLED_H
#define __BSP_OLED_H




#include "main.h"
						        
#define OLED_CMD  0	//Ð´ÃüÁî
#define OLED_DATA 1	//Ð´Êý¾Ý

//OLED¿ØÖÆÓÃº¯Êý
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);  		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y, u32 num , u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,const char *p,u8 size,u8 mode);//----------------------------Ð´×Ö·û´®º¯Êý	 
void OLED_ShowLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);//-------------------------×Ô¼ºÐ´µÄ»­Ïßº¯Êý
void OLED_Line_B(int x1, int y1, int x2, int y2,u8 color);//--------------------------»­Ïßº¯Êý
void draw_fill_circle(u8 x0,u8 y0,u8 r,u8 dot);//---------------------------------»­ÊµÐÄÔ²º¯Êý
void OLED_ShowGBK(u8 x, u8 y,  u8 num, u8 size,u8 mode);//--------------------------Ð´ºº×Öº¯Êý
void OLED_ShowGBKString(u8 x,u8 y,const u8 *p,u8 size,u8 mode);//-------------------Ð´ºº¾äº¯Êý
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);//-----»­BMPÍ¼Æ¬º¯Êý
void OLED_ShowPicture(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t BMP[]);//-------------------------------------»­BMPÍ¼Ïñ












#endif



