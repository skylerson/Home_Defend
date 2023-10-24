#include "oledTask.h"

#include "FreeRTOS.h"
#include "task.h"

#include "bsp_key.h"
#include "bsp_oled.h"

#include "oledfont.h"
#include "userTask.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "timer.h"
#include "sim800aTask.h"

extern int16_t CSQ ;
extern uint8_t NetStatus;


uint8_t page = 0;

//函数指针，指向要显示的内容的函数
static void (*ui_page)(void);

//获取到当前按下的按键值
K_control key_value = 0;
//整个任务操作这个数组，将内容刷新到UI,长度可以改变。
char list[10][PAGE_MAX]= {0};


static void System_Page(void);
static void Set_Page(void);
static void Home_Page(void);
static void Page_Refresh(void);
static void OLED_PageShow(ui_coordinate_t *ui_coordinate, char (*list)[PAGE_MAX], uint8_t list_length);



/**
	* @name					  Ui_Oled_Init
  * @brief          UI界面初始化函数
  * @author         yangchen
	* @param[in]	    void
  * @retval         void
	* @note           null
  */
void Ui_Oled_Init(void)
{
    ui_page = Home_Page;
    key_value=K_MAX;
}


/**
	* @name					  OLED_CubeRotation
  * @brief          CubeRotation
  * @author         Zak Kemble
	* @param[in]	    void
  * @retval         void
	* @note           null
  */
void OLED_CubeRotation(uint16_t x_offest,uint8_t y_offest,uint8_t rotate_speed)
{
	uint8_t  iAng = 0;
  int Cnt, Cntv;
	uint8_t vert[8][3];
	uint8_t x1=0, x2=0, y1=0, y2=0; //,statu=0;
	while(1)
	{
		     iAng+=rotate_speed;
		     if(iAng == 180) iAng = 0;
		
		   // Get vertices for the current angle
				//获取当前角度的顶点
	  		for(Cntv = 0; Cntv < nvert; ++Cntv)
		    {
	  			vert[Cntv][0] = vertices[iAng*nvert + Cntv][0];
	  			vert[Cntv][1] = vertices[iAng*nvert + Cntv][1];
	  			vert[Cntv][2] = vertices[iAng*nvert + Cntv][2];
	  		}
	  		// Draw wireframe for each face of the cube
				//为立方体的每个面绘制线框
				//一个面有四条边，画六个面就是 4x6 =24次就可以完整显示
	  		for(Cnt = 0;Cnt < nfaces; ++Cnt)
				{

	  			x1 = vert[faces[Cnt][0]][0]-34+x_offest;
	  			y1 = vert[faces[Cnt][0]][1]-1+y_offest;
	  			x2 = vert[faces[Cnt][1]][0]-34+x_offest;
	  			y2 = vert[faces[Cnt][1]][1]-1+y_offest;
          OLED_ShowLine(x1, y1, x2, y2,1);
					
	  			x1 = vert[faces[Cnt][1]][0]-34+x_offest;
	  			y1 = vert[faces[Cnt][1]][1]-1+y_offest;
	  			x2 = vert[faces[Cnt][2]][0]-34+x_offest;
	  			y2 = vert[faces[Cnt][2]][1]-1+y_offest;
					OLED_ShowLine(x1, y1, x2, y2,1);

	  			x1 = vert[faces[Cnt][2]][0]-34+x_offest;
	  			y1 = vert[faces[Cnt][2]][1]-1+y_offest;
	  			x2 = vert[faces[Cnt][3]][0]-34+x_offest;
	  			y2 = vert[faces[Cnt][3]][1]-1+y_offest;
					OLED_ShowLine(x1, y1, x2, y2,1);


	  			x1 = vert[faces[Cnt][3]][0]-34+x_offest;
	  			y1 = vert[faces[Cnt][3]][1]-1+y_offest;
	  			x2 = vert[faces[Cnt][0]][0]-34+x_offest;
	  			y2 = vert[faces[Cnt][0]][1]-1+y_offest;
					OLED_ShowLine(x1, y1, x2, y2,1);

	  		}

	
		  OLED_Refresh_Gram();
	    vTaskDelay(90);
		  OLED_Clear();
		  key_value=getKey(K_TIME_SHORT, K_TIME_LONG);
			if(key_value == Key_Esc)
			{
				break;
			}
				
				 
	}
}

/**
	* @name					  ui_oled_task
  * @brief          UI显示任务
  * @author         ghyll
  * @param[in]	    void *pvParameters :typeless pointer, can point to any type of data
  * @retval         void 
	* @note           null
  */
void oled_task(void *pvParameters)
{

	vTaskDelay(500);
	OLED_Init();
	Ui_Oled_Init();
//  OLED_CubeRotation(0,0,5);
	while(1)
	{	

		Page_Refresh();
	  vTaskDelay(100);

	}
}




/**
	* @name					  Home_Page
  * @brief          主界面内容显示函数
  * @author         yangchen
	* @param[in]	    void
  * @retval         void
	* @note           null
  */

void Home_Page(void)
{
    static ui_coordinate_t home_coordinate= {0,0,0};
    sprintf(list[0],"System Pgae ");
    sprintf(list[1],"Set Page" ); 
    sprintf(list[2],"Control = %d ",Control);
    sprintf(list[3],"Ladar = %d  T:%3d",Ladar,SendCount);
    sprintf(list[4],"Shake = %d  S:%3d",Shake,ShakeCount);
    OLED_PageShow(&home_coordinate,list,5);
    if(key_value==Key_Enter)
    {
        switch(home_coordinate.ui_y)
        {
        case 0:
					 ui_page = System_Page;
            break;
        case 1:
            ui_page = Set_Page;
            break;
        case 2:
	
            break;
        case 3:
		    
            break;
				case 4:
		       
            break;
        }
    }
		else if(key_value==Key_Left)
		{
			 switch(home_coordinate.ui_y)
        {
        case 0:
					 send_flag = 1;
            break;
        case 1:
          
            break;
        case 2:
					Control = Control?0:1;
            break;
        case 3:
		    
            break;
				case 4:
		       
            break;
        }
			send_flag = 1;
		}
		else if(key_value==Key_Right)
		{
			 switch(home_coordinate.ui_y)
        {
        case 0:
					 send_flag = 1;
            break;
        case 1:
           
            break;
        case 2:
		        Control = Control?0:1;
            break;
        case 3:
		    
            break;
				case 4:
		       
            break;
        }
			
		}
    page = 0;

}

void System_Page(void)
{
    static ui_coordinate_t home_coordinate= {0,0,0};
		sprintf(list[0],"CSQ = %3d ",CSQ);
    sprintf(list[1],"Net = %3d ",NetStatus);
    sprintf(list[2],"NULL ");
    sprintf(list[3],"S:%3d T:%3d ",ShakeCount,SendCount);
    sprintf(list[4],"NULL ");
    OLED_PageShow(&home_coordinate,list,5);


		if(key_value==Key_Esc)
    {
			ui_page = Home_Page;
		}
		page = 1;

}


void Set_Page(void)
{
    static ui_coordinate_t home_coordinate= {0,0,0};
		sprintf(list[0],"ShakeCNum = %3d",ShakeCountNum);
    sprintf(list[1],"SendCNum  = %3d",SendCountNum);
    sprintf(list[2],"BeepCNum  = %3d",BeepCountNum);
    sprintf(list[3],"NULL ");
    sprintf(list[4],"NULL  ");
    OLED_PageShow(&home_coordinate,list,5);
    if(key_value==Key_Right)
    {
        switch(home_coordinate.ui_y)
        {
        case 0:
            if(ShakeCountNum <1000) ShakeCountNum++;
            break;
        case 1:
              if(SendCountNum <100000) SendCountNum++;
            break;
        case 2:
					   if(BeepCountNum <1000) BeepCountNum++;
            break;
        case 3:
		        
            break;
				case 4:
		      
            break;
        }
    }
    else if(key_value==Key_Left)
    {
				switch(home_coordinate.ui_y)
        {
        case 0:
						if(ShakeCountNum > 2) ShakeCountNum--;
            break;
        case 1:
              if(SendCountNum >20) SendCountNum--;
            break;
        case 2:
					   if(BeepCountNum >5) BeepCountNum--;
            break;
        case 3:
		       
            break;
				case 4:
		       
            break;
        }
    }
		else if(key_value==Key_Esc)
    {
			ui_page = Home_Page;
		}

}



/**
	* @name					  Page_Refresh
  * @brief          UI界面刷新函数
  * @author         yangchen
	* @param[in]	    void
  * @retval         void
	* @note           null
  */
u8 i=0;
void Page_Refresh(void)
{
    static void (*page_late)(void);
    if(page_late!=ui_page)
    {
        OLED_Fill(0,0,128,64,0);
    }
    page_late=ui_page;
    key_value=getKey(K_TIME_SHORT, K_TIME_LONG);
    ui_page();
    OLED_Refresh_Gram();

}

/**
	* @name					  OLED_PageShow
  * @brief          UI界面显示函数
  * @author         yangchen
	* @param[in]	    ui_coordinate 当前页面的光标结构体信息
	* @param[in]	    list          需要显示的列表数组
	* @param[in]	    list_length   需要显示的列表数组的长度
  * @retval         void
	* @note           null
  */
void OLED_PageShow(ui_coordinate_t *ui_coordinate, char (*list)[PAGE_MAX], uint8_t list_length)
{
    unsigned char i;
//---------------------------control coordinate------------------------
    if(key_value == Key_Down)
    {
        ui_coordinate->ui_y++;
    }
    else if(key_value == Key_Up)
    {
        ui_coordinate->ui_y--;
    }
//---------------------------------------------------------------------
    if(ui_coordinate->ui_y<0)
    {
        ui_coordinate->ui_y=list_length-1;
    }
    else if(ui_coordinate->ui_y>=list_length)
    {
        ui_coordinate->ui_y=0;
    }
//---------------------------------------------------------------------
    for(i=0; i<5; i++)
    {
        if(ui_coordinate->ui_y/5*5+i>list_length-1)
        {
            OLED_ShowString(0,12*i,"                   ",12,1);
        }
        else
        {
            if(ui_coordinate->ui_y==ui_coordinate->ui_y/5*5+i)
            {
                OLED_ShowString(0,12*i,list[ui_coordinate->ui_y/5*5+i],12,0);
            }
            else
            {
                OLED_ShowString(0,12*i,list[ui_coordinate->ui_y/5*5+i],12,1);
            }
        }
    }
}


