#include "bsp_my1680u.h"
#include "sys.h"
#include "main.h"

my1680u_info_s my1680u_info;
 	
u8 USART3_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.

u16 USART3_RX_STA=0;       //����״̬���	

void uart3_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART1��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART3_TX   GPIOB10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);
   
  //USART3_RX	  GPIOB11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART3, &USART_InitStructure); //��ʼ������3
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3

}

void MY1680U_Init(void)
{
	 uart3_init(9600);
   my1680u_info.valume = 2;
		//��������
	 MY1680U_Transmit_Cmd(MY1680U_CMD_SET_VOLUME,MY1680U_CMD_PARAM_ONE,my1680u_info.valume,0);
		
}

void uart2_write_bytes(uint8_t *pData,uint8_t length)
{
	uint8_t i = 0;
	for(i=0;i<length;i++)
	{
		while((USART3->SR&0X40)==0){;}//ѭ������,ֱ���������   
	   USART3->DR = (u8)pData[i];   
	//	USART_SendData(USART2,pData[i]);
			
	}
}


//��ʼ�� ���� ������ ���� У���� ������
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

void USART3_IRQHandler(void)                	//����2�жϷ������
	{
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
		
		if((USART3_RX_STA&0x8000)==0)//����δ���
			{
			if(USART3_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART3_RX_STA=0;//���մ���,���¿�ʼ
				else 
				{
					USART3_RX_STA|=0x8000;	//���������
					
					USART3_RX_STA = 0;
					
				}
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART3_RX_STA|=0x4000;
				else
					{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART_REC_LEN-1))USART3_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 

} 

