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
  * @brief  SIM800A��ʼ������
  * @param  ��
  * @retval ��
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


//����Ƿ��п�
//0��ʾ�ɹ���1��ʾʧ��
uint8_t SIM800A_IsInsertCard(void)
{
	return SIM800A_Cmd("AT+CNUM\r","OK",200);
   
}



/**
  * @brief  ��ʼ��SIM800A�õ��� USART
  * @param  ��
  * @retval ��
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
	
	
	/* �ж����� */
	USART_ITConfig ( macSIM800A_USARTx, USART_IT_RXNE, ENABLE ); //ʹ�ܴ��ڽ����ж� 
	USART_ITConfig ( macSIM800A_USARTx, USART_IT_IDLE, ENABLE ); //ʹ�ܴ������߿����ж� 	

	SIM800A_USART_NVIC_Configuration ();
	
	
	USART_Cmd(macSIM800A_USARTx, ENABLE);
	
	
}


/**
  * @brief  ���� SIM800A USART �� NVIC �ж�
  * @param  ��
  * @retval ��
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
		
		if ( strSim800a_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //Ԥ��1���ֽ�д������
			   strSim800a_Fram_Record .Data_RX_BUF [ strSim800a_Fram_Record .InfBit .FramLength ++ ]  = ucCh;

	}
	 	 
	if ( USART_GetITStatus( macSIM800A_USARTx, USART_IT_IDLE ) == SET )                                         //����֡�������
	{
    strSim800a_Fram_Record .InfBit .FramFinishFlag = 1;
		
		ucCh = USART_ReceiveData( macSIM800A_USARTx );                                                              //�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR)
	
//		ucTcpClosedFlag = strstr ( strSim800a_Fram_Record .Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0;
		
  }	

}

void USART2_Send_Bytes(uint8_t *Data ,uint16_t length)
{
	u16 i;
	for(i=0;i<length;i++)							//ѭ����������
	{
		while((USART2->SR&0X40)==0);			//ѭ������,ֱ���������   
		USART2->DR=Data[i];  
	} 
}

/*
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  ����
 * ����  ����
 * ����  ����USART2_printf()����
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
 * ��������USART2_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-USARTx ����ͨ��������ֻ�õ��˴���2����USART2
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			   -...    ��������
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ����
 *         ����Ӧ��USART2_printf( USART2, "\r\n this is a demo \r\n" );
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

	while ( * Data != 0 )     // �ж��Ƿ񵽴��ַ���������
	{				                          
		if ( * Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //�س���
				USART_SendData(USARTx, 0x0d);
				Data ++;
				break;

				case 'n':							          //���з�
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
				case 's':										  //�ַ���
				s = va_arg(ap, const char *);
				
				for ( ; *s; s++) 
				{
					USART_SendData(USARTx,*s);
					while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				}
				
				Data++;
				
				break;

				case 'd':			
					//ʮ����
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
 * ��������SIM800A_Rst
 * ����  ������WF-SIM800Aģ��
 * ����  ����
 * ����  : ��
 * ����  ���� SIM800A_AT_Test ����
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

//0��ʾ�ɹ���1��ʾʧ��
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
        if(reply[n] == 0)                 //����Ϊ�ջ��߱Ƚ����
        {
            return GSM_TRUE;
        }
        
        if( strSim800a_Fram_Record .Data_RX_BUF[ n + off]== reply[n])
        {
            n++;                //�ƶ�����һ����������
        }
        else
        {
            off++;              //������һ��ƥ��
            n=0;                //����
        }
    }
		
//		GSM_DEBUG ( "\r\nREPLY%s\r\n", strSim800a_Fram_Record .Data_RX_BUF );

    if(reply[n]==0)   //�պ�ƥ�����
    {
        return GSM_TRUE;
    }
    
    return GSM_FALSE;       //����ѭ����ʾ�Ƚ���Ϻ�û����ͬ�����ݣ��������
}
/*
 * ��������SIM800A_Cmd
 * ����  ����WF-SIM800Aģ�鷢��ATָ��
 * ����  ��cmd�������͵�ָ��
 *         reply1��reply2���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
 *         waittime���ȴ���Ӧ��ʱ��
 * ����  : 1��ָ��ͳɹ�
 *         0��ָ���ʧ��
 * ����  �����ⲿ����
 */
bool SIM800A_Cmd ( char * cmd, char * reply, u32 waittime )
{    
	SIM800A_ClrRecBuffer();
	
	macSIM800A_Usart ( "%s", cmd );
//	GSM_DEBUG("Send cmd:%s",cmd);	

	if ( reply  == 0 )                      //����Ҫ��������
		return GSM_TRUE;
	
	SIM800A_Delay_ms ( waittime );                 //��ʱ

	printf("Degug Rep:%s\r\n", strSim800a_Fram_Record .Data_RX_BUF );
	return SIM800A_Cmd_Check(reply);
	
}

//��������
//0��ʾ�ɹ���1��ʾʧ��
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
    //��һ�����ź��������Ϊ:"��������"
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
    *num = 0;               //�ַ�����β��Ҫ��0
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



//��ѯ�Ƿ���յ��¶���
//0:���¶��ţ���0���¶��ŵ�ַ
uint8_t SIM800A_IsReceiveMS(void)
{
//	char address[3]={0};
	uint8_t addressnum=0;
	char *redata;
	char *redata_off;
  uint16_t len;
	

/*------------- ��ѯ�Ƿ����¶��Ų���ȡ�洢λ�� ----------------------------*/
    if(SIM800A_Cmd_Check("+CMTI:") != GSM_TRUE)
    {
        return 0;
    }
   
    redata = SIM800A_GetRecBuffer(&len);   //��������
		
		//ƫ������Ӧ���ַ�����ַ
		redata_off = strstr(redata,"+CMTI: \"SM\",");
		printf("redata_off=%s",redata_off);

		if(redata_off == NULL)
			return 0;
		
		//����ƫ�ƺ�ʣ��ĳ���
		len = len - (redata_off - redata); 
		
		printf("CMTI:redata:%s,len=%d\n",redata,len);
    if(len == 0)
    {
        return 0;
    }
    
	  //�ָ��ַ���
		strtok(redata_off,",");		
	
		//strtok�ָ�ڶ�����Ϊatoi���룬��ת���ɹ���Ϊ���ŵ�ַ������atoi����0
		addressnum = atoi(strtok(NULL,","));
					
		printf("\r\naddressnum=%d",addressnum);

		return addressnum;

}	

//���𲦴�绰�����ܽӲ���ͨ��
void SIM800A_Call(char *num)
{
    SIM800A_ClrRecBuffer();                 //����˽��ջ���������
    
    USART_printf(macSIM800A_USARTx,"ATD%s;\r",num);

    //������ǲ��������ݵ�
    //�����κ�Ӧ�𣬿�ͷ����"\r\n",��������Ч�������Ǵӵ�3���ֽڿ�ʼ
    //�Է��ҵ��绰��û��ͨ�������أ�BUSY
    //�Է������˵绰��+COLP: "555",129,"",0,"9648674F98DE"   OK
    //�Է������˵绰,Ȼ��ҵ���NO CARRIER
    //�Է���ʱû�ӵ绰��NO ANSWER
}

//����绰
//0��ʾ�ɹ���1��ʾʧ��
uint8_t SIM800A_IsRing(char *num)
{
    char *redata;
		char * redata_off;
    uint16_t len;
    		

    if(SIM800A_Cmd_Check("RING") != GSM_TRUE)
    {
        return GSM_FALSE;
    }

    
    redata = SIM800A_GetRecBuffer(&len);   //��������
		

//��ʹ��CLIPʹ��������ʾ�Ļ�����Ӧֻ�У�
//		RING
//
		
//ʹ��CLIPʹ��������ʾ����ӦΪ��
		
//		RING
//
//+CLIP: "�绰����",161,"",0,"",0
		
		//ƫ������Ӧ���ַ�����ַ
		redata_off = strstr(redata,"RING");
		
		if(redata_off == NULL)
			return GSM_FALSE;
		
		//����ƫ�ƺ�ʣ��ĳ���
		len = len - (redata_off - redata); 
		
    if(len == 0)
    {
        return GSM_FALSE;
    }
    
    //��һ��ð�ź��������Ϊ:�����롰
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
    *num = 0;               //�ַ�����β��Ҫ��0
   	SIM800A_ClrRecBuffer();
    return GSM_TRUE;
}


/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/
/**
 * @brief  IsASSIC �ж��Ƿ�ASSIC����
 * @param  str: �ַ���ָ��
 * @retval ��ASSIC���뷵��TRUE���Ǵ�ASSIC���뷵��FALSE
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


//���Ͷ��ţ�֧����Ӣ��,����ΪGBK�룩
/**
 * @brief  gsm_sms ���Ͷ��ţ�֧����Ӣ��,����ΪGBK�룩
 * @param  num: �绰���룬smstext����������
 * @retval ��
 */
uint8_t SIM800A_SendSMS(char *num,char *smstext)
{
    char converbuff[160]={0};
	  char cmdbuff[80];
	  char end= 0x1A;
	  uint8_t testSend = 0;

    SIM800A_ClrRecBuffer();                 //����˽��ջ���������
	
    if(IsASSIC(smstext) == GSM_TRUE)
    {
			//Ӣ��
			//"GSM"�ַ���
			if(SIM800A_Cmd("AT+CSCS=\"GSM\"\r","OK", 100) != GSM_TRUE)
			{
				end = 0x1B;
				SIM800A_Cmd(&end,0,0);	//ESC,ȡ������ 
				return GSM_FALSE;		
			}

			//�ı�ģʽ
			if(SIM800A_Cmd("AT+CMGF=1\r","OK", 100) != GSM_TRUE)
			{
				end = 0x1B;
				SIM800A_Cmd(&end,0,0);	//ESC,ȡ������ 
				return GSM_FALSE;		
			
			}

			//���ɵ绰�������� 
			sprintf(cmdbuff,"AT+CMGS=\"%s\"\r",num);
			//���ƶ�������
			strcpy(converbuff,smstext);

    }
    else
    {
//			//����
//			//"UCS2"�ַ���
//			if(SIM800A_Cmd("AT+CSCS=\"UCS2\"\r","OK", 100) != GSM_TRUE) goto sms_failure;
//			//�ı�ģʽ
//			if(SIM800A_Cmd("AT+CMGF=1\r","OK", 100) != GSM_TRUE) goto sms_failure;

//			if(SIM800A_Cmd("AT+CSMP=17,167,0,8\r","OK", 100) != GSM_TRUE) goto sms_failure;

//			//���ɵ绰�������UCS2�ĵ绰����(��Ҫת�� ucs2��)
//			gsm_gbk2ucs2hex(converbuff,num);//ת��ΪUCS2
//			sprintf(cmdbuff,"AT+CMGS=\"%s\"\r",converbuff);  
//			
//			//ת����������
//			gsm_gbk2ucs2hex(converbuff,smstext);
		}
		
	//���͵绰����	
	if(SIM800A_Cmd(cmdbuff,">",500) == GSM_TRUE)
	{	
		SIM800A_ClrRecBuffer();
		USART_printf(macSIM800A_USARTx,"%s",converbuff);//���Ͷ�������
		
		SIM800A_Delay_ms(500); 	
//	GSM_DEBUG("Send String:%s",converbuff);
		SIM800A_Cmd(&end,0,100);		//0x1A������
		//����Ƿ������
		while(SIM800A_Cmd_Check("OK") != GSM_TRUE )
		{		
			if(++testSend >200)//��ȴ�10��
			{
				end = 0x1B;
				SIM800A_Cmd(&end,0,0);	//ESC,ȡ������ 
				return GSM_FALSE;		
			}
			SIM800A_Delay_ms(100); 		
		}	
		return GSM_TRUE;
	}
	else
	{				
		end = 0x1B;
		SIM800A_Cmd(&end,0,0);	//ESC,ȡ������ 
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
//��ȡ��������
//�βΣ�	messadd��	���ŵ�ַ
//			num��		���淢���˺���(unicode�����ʽ���ַ���)
//			str��		�����������(unicode�����ʽ���ַ���)
//����ֵ��	0��ʾʧ��
//			1��ʾ�ɹ���ȡ�����ţ��ö���δ�����˴��ǵ�һ�ζ����������Զ�����׼Ϊ�Ѷ���
//			2��ʾ�ɹ���ȡ�����ţ��ö����Ѷ�
uint8_t SIM800A_ReadMessage(uint8_t messadd,char *num,char *str)
{
	char *redata,cmd[20]={0};
  uint16_t len;
	char result=0;
	
	
	SIM800A_ClrRecBuffer();                 //����˽��ջ���������
	if(messadd>MaxMessAdd)return 0;
	
/*------------- ��ȡ�������� ----------------------------*/
	sprintf(cmd,"AT+CMGR=%d\r",messadd);	
		  
	if(SIM800A_Cmd(cmd,"+CMGR:",500) != GSM_TRUE)
	{
		printf("GSM_FALSE");
		return 0;
	}
	
	redata = SIM800A_GetRecBuffer(&len);   //��������

	if(len == 0)
	{
		return 0;
	}
//	printf("CMGR:redata:%s\nlen=%d\n",redata,len);

	if(strstr(redata,"UNREAD")==0)result=2;
	else	result=1;
	//��һ�����ź��������Ϊ:�������˺��롰
	while(*redata != ',')
	{
		len--;
		if(len==0)
		{
			return 0;
		}
		redata++;
		
	}
	redata+=2;//ȥ��',"'
	while(*redata != '"')
	{
		*num++ = *redata++;
		len--;
	}
	*num = 0;               //�ַ�����β��Ҫ��0
	
	while(*redata != '+')
	{
		len--;
		if(len==0)
		{
			return 0;
		}
		redata++;
	}
	redata+=6;//ȥ��'+32"\r'
	while(*redata != '\r')
	{
		*str++ = *redata++;
	}
	*str = 0;               //�ַ�����β��Ҫ��0
	
//	printf("CMGR:result:%d\n",result);
	return result;
}
