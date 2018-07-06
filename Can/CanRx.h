#ifndef _can_rx_h
#define _can_rx_h
#include "stm32f1xx.h"
typedef struct{
	uint8_t   command;//√¸¡Ó
}Drug_Type;
HAL_StatusTypeDef CAN_Transmit_Remain(CAN_HandleTypeDef* hcan);
void Recivce(void);
void c_t(void);
void Init_FIFO(void);
#endif

