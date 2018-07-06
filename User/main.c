#include "FreeRTOS.h"  
#include "task.h"  
#include "queue.h"  
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h" 
#include "myusart.h"

#define START_TASK_PRIO		5
#define START_STK_SIZE 		128  
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	GPIO_init();
	MX_CAN_Init();
	MX_USART2_UART_Init();
	MX_USART1_UART_Init();
	Key_inti();
	TimeX_Inti(72-1,50000-1);

	//������ʼ����
	xTaskCreate((TaskFunction_t )start_task,            //������
				(const char*    )"start_task",          //��������
				(uint16_t       )START_STK_SIZE,        //�����ջ��С
				(void*          )NULL,                  //���ݸ��������Ĳ���
				(UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
				(TaskHandle_t*  )&StartTask_Handler);   //������              
	vTaskStartScheduler();          //�����������
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
