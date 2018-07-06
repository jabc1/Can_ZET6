#include "myusart.h"
extern unsigned char temp;
extern UART_HandleTypeDef huart2;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2)
	{
		HAL_UART_Receive_IT(&huart2,&temp,1);
		HAL_UART_Transmit_IT(&huart2,&temp,1);
	}
}








