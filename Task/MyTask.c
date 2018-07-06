#include "MyTask.h"
#include "FreeRTOS.h"  
#include "task.h"  
#include "queue.h" 
#include "stm32f1xx.h"
#include "stm32f1xx_hal_usart.h"
#include "myusart.h"
#include <stdlib.h>
#include <string.h>
#include "stdio.h"
#include "CanRx.h"
#include "myfifo.h"
extern TaskHandle_t StartTask_Handler;

#define KEY_TASK_PRIO		2
#define KEY_STK_SIZE 		128  
TaskHandle_t KeyTask_Handler;
void key_task(void *pvParameters);

#define TASK1_TASK_PRIO		3
#define TASK1_STK_SIZE 		128  
TaskHandle_t Task1Task_Handler;
void task1_task(void *pvParameters);

#define TASK2_TASK_PRIO		4
#define TASK2_STK_SIZE 		128  
TaskHandle_t Task2Task_Handler;
void task2_task(void *pvParameters);

#define CAN_TASK_PRIO		1
#define CAN_STK_SIZE 		128  
TaskHandle_t CanTask_Handler;
void can_task(void *pvParameters);

#if 1
typedef struct data_type{
    int age;
    char name[20];
}data;
  
#endif



//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
	//����KEY����
	xTaskCreate((TaskFunction_t )key_task,             
                (const char*    )"key_task",           
                (uint16_t       )KEY_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )KEY_TASK_PRIO,        
                (TaskHandle_t*  )&KeyTask_Handler);  
    //����TASK1����
    xTaskCreate((TaskFunction_t )task1_task,             
                (const char*    )"task1_task",           
                (uint16_t       )TASK1_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASK1_TASK_PRIO,        
                (TaskHandle_t*  )&Task1Task_Handler);   
//    //����TASK2����
//    xTaskCreate((TaskFunction_t )task2_task,     
//                (const char*    )"task2_task",   
//                (uint16_t       )TASK2_STK_SIZE,
//                (void*          )NULL,
//                (UBaseType_t    )TASK2_TASK_PRIO,
//                (TaskHandle_t*  )&Task2Task_Handler); 

    //����CAN����
    xTaskCreate((TaskFunction_t )can_task,     
                (const char*    )"can_task",   
                (uint16_t       )CAN_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )CAN_TASK_PRIO,
                (TaskHandle_t*  )&CanTask_Handler); 

    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}
void ts()
{
	data*bob=NULL;
	bob=(data*)malloc(sizeof(data));
	if(bob!=NULL)
	{
		bob->age=22;
		strcpy(bob->name,"Robert");
		printf("%sis%dyearsold\n",bob->name,bob->age);
	}
	else
	{
		printf("mallocerror!\n");
		exit(-1);
	}
	free(bob);
	bob=NULL;
}
//key������
void key_task(void *pvParameters)
{
	u8 key,statflag=0,falg=0;
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case WKUP_PRES:
				statflag=!statflag;
				if(statflag==1)
				{
					ts();
					vTaskSuspend(Task1Task_Handler);//��������
					printf("��������1������!\r\n");
				}
				else
				{
					vTaskResume(Task1Task_Handler);	//�ָ�����1
					printf("�ָ�����1������!\r\n");
				}		
				break;
			case KEY1_PRES:
				falg = !falg;
				if(falg)
				{
					vTaskSuspend(Task2Task_Handler);//��������2
					printf("��������2������!\r\n");
				}
				else
				{
					vTaskResume(Task2Task_Handler);	//�ָ�����1
					printf("�ָ�����2������!\r\n");
				}
				break;
		}
		vTaskDelay(10);
	}
}
extern UART_HandleTypeDef huart2;
//task1������
void task1_task(void *pvParameters)
{
	u8 task1_num=0;
	while(1)
	{
		task1_num++;
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,(GPIO_PinState)!HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_1));
		vTaskDelay(200);
	}
}

//task2������
void task2_task(void *pvParameters)
{
	u8 task2_num=0;
	while(1)
	{
		task2_num++;
		vTaskDelay(200);
	}
}
uint8_t Rxflag;
//can������
void can_task(void *pvParameters)
{
//	test_q();
	while(1)
	{
		//if(Rxflag)
		{
			//Rxflag = 0;
			c_t();
			//can_test1();
		}
		vTaskDelay(1);
	}
}




