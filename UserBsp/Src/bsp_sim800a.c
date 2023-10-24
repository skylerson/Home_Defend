#include "bsp_sim800a.h"
#include <stdio.h>  
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "delay.h"



#include "FreeRTOS.h"
#include "task.h"


//static void                   SIM800A_GPIO_Config                 ( void );
static void                   SIM800A_USART_Config                ( void );
static void                   SIM800A_USART_NVIC_Configuration    ( void );



#define SIM800A_Delay_ms(ms) vTaskDelay(ms)



struct  STRUCT_USARTx_Fram strSim800a_Fram_Record = { 0 };


/**
  * @brief  SIM800A初始化函数
  * @param  无
  * @retval 无
  */
  uint8_t SIM800A_Init ( void )
{

	SIM800A_USART_Config (); 
  SIM800A_USART_NVIC_Configuration();
	
	if( SIM800A_Cmd ( "AT\r\n", "OK", 500 ) != GSM_TRUE)
	{
		return GSM_FALSE;
	}
	
	if(SIM800A_Cmd("AT+CGMM\r\n","OK", 500) != GSM_TRUE)
	{
			return GSM_FALSE;
	}
	
	return GSM_TRUE;
	
}


//检测是否有卡
//0表示成功，1表示失败
uint8_t SIM800A_IsInsertCard(void)
{
	return SIM800A_Cmd("AT+CNUM\r","OK",200);
   
}



/**
  * @brief  初始化SIM800A用到的 USART
  * @param  无
  * @retval 无
  */
static void SIM800A_USART_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	
	/* config USART clock */
	macSIM800A_USART_APBxClock_FUN ( macSIM800A_USART_CLK, ENABLE );
	macSIM800A_USART_GPIO_APBxClock_FUN ( macSIM800A_USART_GPIO_CLK, ENABLE );
	
	/* USART GPIO config */
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin =  macSIM800A_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(macSIM800A_USART_TX_PORT, &GPIO_InitStructure);  
  
	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = macSIM800A_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(macSIM800A_USART_RX_PORT, &GPIO_InitStructure);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = macSIM800A_USART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(macSIM800A_USARTx, &USART_InitStructure);
	
	
	/* 中断配置 */
	USART_ITConfig ( macSIM800A_USARTx, USART_IT_RXNE, ENABLE ); //使能串口接收中断 
	USART_ITConfig ( macSIM800A_USARTx, USART_IT_IDLE, ENABLE ); //使能串口总线空闲中断 	

	SIM800A_USART_NVIC_Configuration ();
	
	
	USART_Cmd(macSIM800A_USARTx, ENABLE);
	
	
}


/**
  * @brief  配置 SIM800A USART 的 NVIC 中断
  * @param  无
  * @retval 无
  */
static void SIM800A_USART_NVIC_Configuration ( void )
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	
	/* Configure the NVIC Preemption Priority Bits */  
	//NVIC_PriorityGroupConfig ( NVIC_PriorityGroup_4 );

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = macSIM800A_USART_IRQ;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

/**
  * @brief  This function handles macSIM800A_USARTx Handler.
  * @param  None
  * @retval None
  */
void macSIM800A_USART_INT_FUN ( void )
{	
	uint8_t ucCh;
	
	if ( USART_GetITStatus ( macSIM800A_USARTx, USART_IT_RXNE ) != RESET )
	{
		ucCh  = USART_ReceiveData( macSIM800A_USARTx );
		
		if ( strSim800a_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //预留1个字节写结束符
			   strSim800a_Fram_Record .Data_RX_BUF [ strSim800a_Fram_Record .InfBit .FramLength ++ ]  = ucCh;

	}
	 	 
	if ( USART_GetITStatus( macSIM800A_USARTx, USART_IT_IDLE ) == SET )                                         //数据帧接收完毕
	{
    strSim800a_Fram_Record .InfBit .FramFinishFlag = 1;
		
		ucCh = USART_ReceiveData( macSIM800A_USARTx );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
	
//		ucTcpClosedFlag = strstr ( strSim800a_Fram_Record .Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0;
		
  }	

}

void USART2_Send_Bytes(uint8_t *Data ,uint16_t length)
{
	u16 i;
	for(i=0;i<length;i++)							//循环发送数据
	{
		while((USART2->SR&0X40)==0);			//循环发送,直到发送完毕   
		USART2->DR=Data[i];  
	} 
}

/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART2_printf()调用
 */
static char * itoa( int value, char *string, int radix )
{
	int     i, d;
	int     flag = 0;
	char    *ptr = string;

	/* This implementation only works for decimal numbers. */
	if (radix != 10)
	{
		*ptr = 0;
		return string;
	}

	if (!value)
	{
		*ptr++ = 0x30;
		*ptr = 0;
		return string;
	}

	/* if this is a negative value insert the minus sign. */
	if (value < 0)
	{
		*ptr++ = '-';

		/* Make the value positive. */
		value *= -1;
		
	}

	for (i = 10000; i > 0; i /= 10)
	{
		d = value / i;

		if (d || flag)
		{
			*ptr++ = (char)(d + 0x30);
			value -= (d * i);
			flag = 1;
		}
	}

	/* Null terminate the string. */
	*ptr = 0;

	return string;

} /* NCL_Itoa */




/*
 * 函数名：USART2_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口2，即USART2
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用USART2_printf( USART2, "\r\n this is a demo \r\n" );
 *            		 USART2_printf( USART2, "\r\n %d \r\n", i );
 *            		 USART2_printf( USART2, "\r\n %s \r\n", j );
 */
void USART_printf ( USART_TypeDef * USARTx, char * Data, ... )
{
	const char *s;
	int d;   
	char buf[16];

	
	va_list ap;
	va_start(ap, Data);

	while ( * Data != 0 )     // 判断是否到达字符串结束符
	{				                          
		if ( * Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
				USART_SendData(USARTx, 0x0d);
				Data ++;
				break;

				case 'n':							          //换行符
				USART_SendData(USARTx, 0x0a);	
				Data ++;
				break;

				default:
				Data ++;
				break;
			}			 
		}
		
		else if ( * Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
				s = va_arg(ap, const char *);
				
				for ( ; *s; s++) 
				{
					USART_SendData(USARTx,*s);
					while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				}
				
				Data++;
				
				break;

				case 'd':			
					//十进制
				d = va_arg(ap, int);
				
				itoa(d, buf, 10);
				
				for (s = buf; *s; s++) 
				{
					USART_SendData(USARTx,*s);
					while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				}
				Data++;
				
				break;
				
				default:
				Data++;
				
				break;
				
			}		 
		}
		
		else USART_SendData(USARTx, *Data++);
		
		while ( USART_GetFlagStatus ( USARTx, USART_FLAG_TXE ) == RESET );
		
	}
}


/*
 * 函数名：SIM800A_Rst
 * 描述  ：重启WF-SIM800A模块
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被 SIM800A_AT_Test 调用
 */
void SIM800A_Rst ( void )
{
	#if 1
	 SIM800A_Cmd ( "AT+RST", "OK",  2500 );   	
	
	#else
//	 macSIM800A_RST_LOW_LEVEL();
//	 SIM800A_Delay_ms ( 500 ); 
//	 macSIM800A_RST_HIGH_LEVEL();
	#endif

}

//0表示成功，1表示失败
uint8_t SIM800A_Cmd_Check(char *reply)
{
    uint16_t len;
    uint8_t n;
    uint8_t off;
	  char *redata;
   
    redata = SIM800A_GetRecBuffer(&len); 
	  *(redata+len) = '\0';
	//  printf("Degug Rep:%s\r\n", strSim800a_Fram_Record .Data_RX_BUF );
	
    n = 0;
    off = 0;
    while((n + off)<len)
    {
        if(reply[n] == 0)                 //数据为空或者比较完毕
        {
            return GSM_TRUE;
        }
        
        if( strSim800a_Fram_Record .Data_RX_BUF[ n + off]== reply[n])
        {
            n++;                //移动到下一个接收数据
        }
        else
        {
            off++;              //进行下一轮匹配
            n=0;                //重来
        }
    }
		
//		GSM_DEBUG ( "\r\nREPLY%s\r\n", strSim800a_Fram_Record .Data_RX_BUF );

    if(reply[n]==0)   //刚好匹配完毕
    {
        return GSM_TRUE;
    }
    
    return GSM_FALSE;       //跳出循环表示比较完毕后都没有相同的数据，因此跳出
}
/*
 * 函数名：SIM800A_Cmd
 * 描述  ：对WF-SIM800A模块发送AT指令
 * 输入  ：cmd，待发送的指令
 *         reply1，reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
 *         waittime，等待响应的时间
 * 返回  : 1，指令发送成功
 *         0，指令发送失败
 * 调用  ：被外部调用
 */
bool SIM800A_Cmd ( char * cmd, char * reply, u32 waittime )
{    
	SIM800A_ClrRecBuffer();
	
	macSIM800A_Usart ( "%s", cmd );
//	GSM_DEBUG("Send cmd:%s",cmd);	

	if ( reply  == 0 )                      //不需要接收数据
		return GSM_TRUE;
	
	SIM800A_Delay_ms ( waittime );                 //延时

	printf("Degug Rep:%s\r\n", strSim800a_Fram_Record .Data_RX_BUF );
	return SIM800A_Cmd_Check(reply);
	
}

//本机号码
//0表示成功，1表示失败
uint8_t SIM800A_Get_Number(char *num)
{
    char *redata;
    uint16_t len;
  
	
    if(SIM800A_Cmd("AT+CNUM\r\n","OK", 500) != GSM_TRUE)
    {
        return GSM_FALSE;
    }
 
		redata = SIM800A_GetRecBuffer(&len); 
		GSM_DEBUG_ARRAY((uint8_t *)redata,len);
    //第一个逗号后面的数据为:"本机号码"
    while(*redata != ',')
    {
        len--;
        if(len==0)
        {
            return GSM_FALSE;
        }
        redata++;
    }
    redata+=2;
    
    while(*redata != '"')
    {
        *num++ = *redata++;
    }
    *num = 0;               //字符串结尾需要清0
    return GSM_TRUE;
}


void SIM800A_AT_Test ( void )
{
	char count=0;
	SIM800A_Delay_ms ( 100 );
	while ( count < 10 )
	{
		if( SIM800A_Cmd ( "AT", "OK", 500 ) ) return;
		SIM800A_Rst();
		++ count;
	}
}

char *SIM800A_GetRecBuffer(uint16_t *length)
{
  *length = strSim800a_Fram_Record .InfBit .FramLength;
	 return (char *)strSim800a_Fram_Record.Data_RX_BUF;
}

void  SIM800A_ClrRecBuffer(void)
{
  uint16_t i = RX_BUF_MAX_LEN+1;
	
	strSim800a_Fram_Record .InfBit .FramLength = 0;
	strSim800a_Fram_Record .InfBit .FramFinishFlag = 0;
	
	while(i)
	{
		 strSim800a_Fram_Record.Data_RX_BUF[--i] = 0;
	}
}



//查询是否接收到新短信
//0:无新短信，非0：新短信地址
uint8_t SIM800A_IsReceiveMS(void)
{
//	char address[3]={0};
	uint8_t addressnum=0;
	char *redata;
	char *redata_off;
  uint16_t len;
	

/*------------- 查询是否有新短信并提取存储位置 ----------------------------*/
    if(SIM800A_Cmd_Check("+CMTI:") != GSM_TRUE)
    {
        return 0;
    }
   
    redata = SIM800A_GetRecBuffer(&len);   //接收数据
		
		//偏移至响应的字符串地址
		redata_off = strstr(redata,"+CMTI: \"SM\",");
		printf("redata_off=%s",redata_off);

		if(redata_off == NULL)
			return 0;
		
		//计算偏移后剩余的长度
		len = len - (redata_off - redata); 
		
		printf("CMTI:redata:%s,len=%d\n",redata,len);
    if(len == 0)
    {
        return 0;
    }
    
	  //分割字符串
		strtok(redata_off,",");		
	
		//strtok分割，第二段作为atoi输入，若转换成功则为短信地址，否则atoi返回0
		addressnum = atoi(strtok(NULL,","));
					
		printf("\r\naddressnum=%d",addressnum);

		return addressnum;

}	

//发起拨打电话（不管接不接通）
void SIM800A_Call(char *num)
{
    SIM800A_ClrRecBuffer();                 //清空了接收缓冲区数据
    
    USART_printf(macSIM800A_USARTx,"ATD%s;\r",num);

    //拨打后是不返回数据的
    //不管任何应答，开头都是"\r\n",即真正有效的数据是从第3个字节开始
    //对方挂掉电话（没接通），返回：BUSY
    //对方接听了电话：+COLP: "555",129,"",0,"9648674F98DE"   OK
    //对方接听了电话,然后挂掉：NO CARRIER
    //对方超时没接电话：NO ANSWER
}

//来电电话
//0表示成功，1表示失败
uint8_t SIM800A_IsRing(char *num)
{
    char *redata;
		char * redata_off;
    uint16_t len;
    		

    if(SIM800A_Cmd_Check("RING") != GSM_TRUE)
    {
        return GSM_FALSE;
    }

    
    redata = SIM800A_GetRecBuffer(&len);   //接收数据
		

//不使用CLIP使能来电显示的话，响应只有：
//		RING
//
		
//使用CLIP使能来电显示后，响应为：
		
//		RING
//
//+CLIP: "电话号码",161,"",0,"",0
		
		//偏移至响应的字符串地址
		redata_off = strstr(redata,"RING");
		
		if(redata_off == NULL)
			return GSM_FALSE;
		
		//计算偏移后剩余的长度
		len = len - (redata_off - redata); 
		
    if(len == 0)
    {
        return GSM_FALSE;
    }
    
    //第一个冒号后面的数据为:”号码“
    while(*redata != ':')
    {
        len--;
        if(len==0)
        {
            return GSM_FALSE;
        }
        redata++;
    }
    redata+=3;
    while(*redata != '"')
    {
        *num++ = *redata++;
    }
    *num = 0;               //字符串结尾需要清0
   	SIM800A_ClrRecBuffer();
    return GSM_TRUE;
}


/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/
/**
 * @brief  IsASSIC 判断是否纯ASSIC编码
 * @param  str: 字符串指针
 * @retval 纯ASSIC编码返回TRUE，非纯ASSIC编码返回FALSE
 */
uint8_t IsASSIC(char * str)
{
    while(*str)
    {
        if(*str>0x7F)
        {
            return GSM_FALSE;
        }
        str++;
    }
    
    return GSM_TRUE;
}


//发送短信（支持中英文,中文为GBK码）
/**
 * @brief  gsm_sms 发送短信（支持中英文,中文为GBK码）
 * @param  num: 电话号码，smstext：短信内容
 * @retval 无
 */
uint8_t SIM800A_SendSMS(char *num,char *smstext)
{
    char converbuff[160]={0};
	  char cmdbuff[80];
	  char end= 0x1A;
	  uint8_t testSend = 0;

    SIM800A_ClrRecBuffer();                 //清空了接收缓冲区数据
	
    if(IsASSIC(smstext) == GSM_TRUE)
    {
			//英文
			//"GSM"字符集
			if(SIM800A_Cmd("AT+CSCS=\"GSM\"\r","OK", 100) != GSM_TRUE)
			{
				end = 0x1B;
				SIM800A_Cmd(&end,0,0);	//ESC,取消发送 
				return GSM_FALSE;		
			}

			//文本模式
			if(SIM800A_Cmd("AT+CMGF=1\r","OK", 100) != GSM_TRUE)
			{
				end = 0x1B;
				SIM800A_Cmd(&end,0,0);	//ESC,取消发送 
				return GSM_FALSE;		
			
			}

			//生成电话号码命令 
			sprintf(cmdbuff,"AT+CMGS=\"%s\"\r",num);
			//复制短信内容
			strcpy(converbuff,smstext);

    }
    else
    {
//			//中文
//			//"UCS2"字符集
//			if(SIM800A_Cmd("AT+CSCS=\"UCS2\"\r","OK", 100) != GSM_TRUE) goto sms_failure;
//			//文本模式
//			if(SIM800A_Cmd("AT+CMGF=1\r","OK", 100) != GSM_TRUE) goto sms_failure;

//			if(SIM800A_Cmd("AT+CSMP=17,167,0,8\r","OK", 100) != GSM_TRUE) goto sms_failure;

//			//生成电话号码命令，UCS2的电话号码(需要转成 ucs2码)
//			gsm_gbk2ucs2hex(converbuff,num);//转换为UCS2
//			sprintf(cmdbuff,"AT+CMGS=\"%s\"\r",converbuff);  
//			
//			//转换短信内容
//			gsm_gbk2ucs2hex(converbuff,smstext);
		}
		
	//发送电话号码	
	if(SIM800A_Cmd(cmdbuff,">",500) == GSM_TRUE)
	{	
		SIM800A_ClrRecBuffer();
		USART_printf(macSIM800A_USARTx,"%s",converbuff);//发送短信内容
		
		SIM800A_Delay_ms(500); 	
//	GSM_DEBUG("Send String:%s",converbuff);
		SIM800A_Cmd(&end,0,100);		//0x1A结束符
		//检测是否发送完成
		while(SIM800A_Cmd_Check("OK") != GSM_TRUE )
		{		
			if(++testSend >200)//最长等待10秒
			{
				end = 0x1B;
				SIM800A_Cmd(&end,0,0);	//ESC,取消发送 
				return GSM_FALSE;		
			}
			SIM800A_Delay_ms(100); 		
		}	
		return GSM_TRUE;
	}
	else
	{				
		end = 0x1B;
		SIM800A_Cmd(&end,0,0);	//ESC,取消发送 
		return GSM_FALSE;		
	}		
}

uint8_t SIM800A_Get_Net_Register_Status(void) 
{
	char *token = NULL;
	uint8_t data[2];
	
	char *redata;
	
	if(SIM800A_Cmd ( "AT+CREG?\r", "OK", 500 ) == GSM_TRUE)
	{
			token = strtok(strstr((const char*)strSim800a_Fram_Record.Data_RX_BUF, "+CREG"), ":");
		  token = strtok(NULL, ",");
		  data[0] = atoi(token);
		  token = strtok(NULL, "\r\n");
		  data[1] = atoi(token);
		 //Mn316_StringToI32Array(Mn316_Fram_Record.Data_RX_BUF,"+VBAT:",strlen(Mn316_Fram_Record.Data_RX_BUF),1,data,',',' ');
		 // printf("CEREG = %d\r\n",data[1]);
	}
	else
	{
		return 0;// ERROR
	}
	SIM800A_ClrRecBuffer();
 return data[1];
}

int32_t SIM800A_Get_Singal_Quality(void)
{
	char *token = NULL;
	int32_t data[2];
	
	if(SIM800A_Cmd ( "AT+CSQ\r", "OK", 500 )== GSM_TRUE )
	{
		  token = strtok(strstr((const char*)strSim800a_Fram_Record.Data_RX_BUF, "+CSQ"), ":");
		  token = strtok(NULL, ",");
		  data[0] = atoi(token);
			token = strtok(NULL, "\r\n");
		  data[1] = atoi(token);
     // Mn316_StringToI32Array(Mn316_Fram_Record.Data_RX_BUF,"+CSQ:",strlen(Mn316_Fram_Record.Data_RX_BUF),2,data,',',0x0D );
	}
	else
	{
		SIM800A_ClrRecBuffer();
		return 100;// ERROR
	}
  SIM800A_ClrRecBuffer();
 return data[0];
}
static uint8_t MaxMessAdd=50;
//读取短信内容
//形参：	messadd：	短信地址
//			num：		保存发件人号码(unicode编码格式的字符串)
//			str：		保存短信内容(unicode编码格式的字符串)
//返回值：	0表示失败
//			1表示成功读取到短信，该短信未读（此处是第一次读，读完后会自动被标准为已读）
//			2表示成功读取到短信，该短信已读
uint8_t SIM800A_ReadMessage(uint8_t messadd,char *num,char *str)
{
	char *redata,cmd[20]={0};
  uint16_t len;
	char result=0;
	
	
	SIM800A_ClrRecBuffer();                 //清空了接收缓冲区数据
	if(messadd>MaxMessAdd)return 0;
	
/*------------- 读取短信内容 ----------------------------*/
	sprintf(cmd,"AT+CMGR=%d\r",messadd);	
		  
	if(SIM800A_Cmd(cmd,"+CMGR:",500) != GSM_TRUE)
	{
		printf("GSM_FALSE");
		return 0;
	}
	
	redata = SIM800A_GetRecBuffer(&len);   //接收数据

	if(len == 0)
	{
		return 0;
	}
//	printf("CMGR:redata:%s\nlen=%d\n",redata,len);

	if(strstr(redata,"UNREAD")==0)result=2;
	else	result=1;
	//第一个逗号后面的数据为:”发件人号码“
	while(*redata != ',')
	{
		len--;
		if(len==0)
		{
			return 0;
		}
		redata++;
		
	}
	redata+=2;//去掉',"'
	while(*redata != '"')
	{
		*num++ = *redata++;
		len--;
	}
	*num = 0;               //字符串结尾需要清0
	
	while(*redata != '+')
	{
		len--;
		if(len==0)
		{
			return 0;
		}
		redata++;
	}
	redata+=6;//去掉'+32"\r'
	while(*redata != '\r')
	{
		*str++ = *redata++;
	}
	*str = 0;               //字符串结尾需要清0
	
//	printf("CMGR:result:%d\n",result);
	return result;
}
