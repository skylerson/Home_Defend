#include "startTask.h"
#include "userTask.h"
#include "oledTask.h"
#include "sim800aTask.h"

#include "FreeRTOS.h"
#include "task.h"


#define START_TASK_PRIO 1
#define START_STK_SIZE 512
static TaskHandle_t StartTask_Handler;


#define USER_TASK_PRIO 3
#define USER_STK_SIZE 128
static TaskHandle_t UserTask_Handler;


#define OLED_TASK_PRIO 2
#define OLED_STK_SIZE 512
static TaskHandle_t OledTask_Handler;

#define SIM800A_TASK_PRIO 4
#define SIM800A_STK_SIZE 512
static TaskHandle_t Sim800aTask_Handler;




void start_task(void *pvParameters)
{
		taskENTER_CRITICAL();
	 xTaskCreate((TaskFunction_t)user_task,
					(const char *)"user_task",
					(uint16_t)USER_STK_SIZE,
					(void *)NULL,
					(UBaseType_t)USER_TASK_PRIO,
					(TaskHandle_t *)&UserTask_Handler);
						
	 xTaskCreate((TaskFunction_t)oled_task,
					(const char *)"oled_task",
					(uint16_t)OLED_STK_SIZE,
					(void *)NULL,
					(UBaseType_t)OLED_TASK_PRIO,
					(TaskHandle_t *)&OledTask_Handler);		


	 xTaskCreate((TaskFunction_t)sim800a_task,
					(const char *)"sim800a_task",
					(uint16_t)SIM800A_STK_SIZE,
					(void *)NULL,
					(UBaseType_t)SIM800A_TASK_PRIO,
					(TaskHandle_t *)&Sim800aTask_Handler);				
	
	
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
							
    taskEXIT_CRITICAL();            //�˳��ٽ���,�����ٽ���������Ƕ��,�жϼ�����
}

void startTaskCreate(void)
{						
							
    xTaskCreate((TaskFunction_t)start_task,          //������
                (const char *)"start_task",          //��������
                (uint16_t)START_STK_SIZE,            //�����ջ��С
                (void *)NULL,                        //���ݸ��������Ĳ���
                (UBaseType_t)START_TASK_PRIO,        //�������ȼ�
                (TaskHandle_t *)&StartTask_Handler); //������
}

















