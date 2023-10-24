#include "bsp_my1680u.h"
#include "sys.h"
#include "main.h"

my1680u_info_s my1680u_info;
 	
u8 USART3_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

u16 USART3_RX_STA=0;       //接收状态标记	

void uart3_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART1，GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART3_TX   GPIOB10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);
   
  //USART3_RX	  GPIOB11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART3, &USART_InitStructure); //初始化串口3
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART3, ENABLE);                    //使能串口3

}

void MY1680U_Init(void)
{
	 uart3_init(9600);
   my1680u_info.valume = 2;
		//设置音量
	 MY1680U_Transmit_Cmd(MY1680U_CMD_SET_VOLUME,MY1680U_CMD_PARAM_ONE,my1680u_info.valume,0);
		
}

void uart2_write_bytes(uint8_t *pData,uint8_t length)
{
	uint8_t i = 0;
	for(i=0;i<length;i++)
	{
		while((USART3->SR&0X40)==0){;}//循环发送,直到发送完毕   
	   USART3->DR = (u8)pData[i];   
	//	USART_SendData(USART2,pData[i]);
			
	}
}


//起始码 长度 操作码 参数 校验码 结束码
uint8_t MY1680U_Cmd_Buff[7];

void MY1680U_Transmit_Cmd(uint8_t cmd,uint8_t paramExistNum,uint8_t param1,uint8_t param2)
{
	uint8_t length = 5+paramExistNum;
	MY1680U_Cmd_Buff[0] = MY1680U_CMD_START_CHAR;

	switch(paramExistNum)
	{
		case MY1680U_CMD_PARAM_NO:
			MY1680U_Cmd_Buff[1] = 0X03;
			MY1680U_Cmd_Buff[2] = cmd;
		  MY1680U_Cmd_Buff[3] = MY1680U_Cmd_Buff[1]^MY1680U_Cmd_Buff[2];
			MY1680U_Cmd_Buff[4] = MY1680U_CMD_STOP_CHAR;
			break;
		case MY1680U_CMD_PARAM_ONE:
			MY1680U_Cmd_Buff[1] = 0X04;
			MY1680U_Cmd_Buff[2] = cmd;
		  MY1680U_Cmd_Buff[3] = param1;
		  MY1680U_Cmd_Buff[4] = MY1680U_Cmd_Buff[1]^MY1680U_Cmd_Buff[2]^MY1680U_Cmd_Buff[3];
			MY1680U_Cmd_Buff[5] = MY1680U_CMD_STOP_CHAR;
			break;
		case MY1680U_CMD_PARAM_TWO:
			MY1680U_Cmd_Buff[1] = 0X05;
			MY1680U_Cmd_Buff[2] = cmd;
		  MY1680U_Cmd_Buff[3] = param1;
			MY1680U_Cmd_Buff[4] = param2;
		  MY1680U_Cmd_Buff[5] = MY1680U_Cmd_Buff[1]^MY1680U_Cmd_Buff[2]^MY1680U_Cmd_Buff[3]^MY1680U_Cmd_Buff[4];
			MY1680U_Cmd_Buff[6] = MY1680U_CMD_STOP_CHAR;
			break;
	
	}
	uart2_write_bytes(MY1680U_Cmd_Buff,length);
}

void USART3_IRQHandler(void)                	//串口2中断服务程序
	{
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
		
		if((USART3_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART3_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART3_RX_STA=0;//接收错误,重新开始
				else 
				{
					USART3_RX_STA|=0x8000;	//接收完成了
					
					USART3_RX_STA = 0;
					
				}
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART3_RX_STA|=0x4000;
				else
					{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART_REC_LEN-1))USART3_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 

} 

