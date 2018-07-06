#include "SysCan.h"
#include "CanRx.h"
CAN_HandleTypeDef 		hcan;
CAN_FilterConfTypeDef  	sFilterConfig;//������
CanTxMsgTypeDef       	TxMessage;
CanRxMsgTypeDef        	RxMessage;

/*
*	PCLK1 = 36M
*	bps = 36M/(BRP[9:0]*(BS1+BS2+SYNC_SEG))
*	����SYNC_SEG�̶�Ϊ1��ʱ�䵥Ԫ���� bps = 36M/(BRP[9:0]*(BS1+BS2+1))��
*	��ʱע���ͬ����ת���SJWû��ϵ��������Ҳһֱ����Ϊ1.
*	����CAN������
*	������λ��ʱ���1��ʱ���2֮�䡣����CIA�Ƽ������㣬���������85%~90%��
*	�����ϲ��˺ö࣬�����϶�����ô���� ��BS1+1��/��1+BS1+BS2����
*	�ó��Ľ������BS1�Ƚϴ��BS2�Ƚ�С������Ϊ1.
*/


void MX_CAN_Init(void)//can������1M
{
	hcan.Instance = CAN1;
	hcan.Init.Prescaler = 16;
	hcan.Init.Mode = CAN_MODE_NORMAL;
	hcan.Init.SJW = CAN_SJW_1TQ;
	hcan.Init.BS1 = CAN_BS1_7TQ;
	hcan.Init.BS2 = CAN_BS2_1TQ;
	hcan.Init.TTCM = DISABLE;
	hcan.Init.ABOM = DISABLE;
	hcan.Init.AWUM = DISABLE;
	hcan.Init.NART = DISABLE;
	hcan.Init.RFLM = DISABLE;
	hcan.Init.TXFP = DISABLE;
	if (HAL_CAN_Init(&hcan) != HAL_OK)
	{
		;//_Error_Handler(__FILE__, __LINE__);
	}
	can_sFilterconfig();
}

void can_sFilterconfig()//����������  
{
	sFilterConfig.FilterNumber = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.BankNumber = 14;
	HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);
	
	hcan.pTxMsg = &TxMessage;//can ����
	hcan.pRxMsg = &RxMessage;//can ����
	HAL_CAN_Receive_IT(&hcan,CAN_FIFO0);//�����жϽ���	
	//read_t();
	Init_FIFO();
}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	if(canHandle->Instance==CAN1)
	{
		/* CAN1 clock enable */
		__HAL_RCC_CAN1_CLK_ENABLE();

		/**CAN GPIO Configuration    
		PA11     ------> CAN_RX
		PA12     ------> CAN_TX 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_11;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_12;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* CAN1 interrupt Init */
		HAL_NVIC_SetPriority(USB_HP_CAN1_TX_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
		HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
		HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
	}
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{
	if(canHandle->Instance==CAN1)
	{
		/* Peripheral clock disable */
		__HAL_RCC_CAN1_CLK_DISABLE();

		/**CAN GPIO Configuration    
		PA11     ------> CAN_RX
		PA12     ------> CAN_TX 
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

		/* CAN1 interrupt Deinit */
		HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
		HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
	}
} 
void read_t()
{
	hcan.pTxMsg=&TxMessage;
	hcan.pTxMsg->StdId=0x0135;         //����ID��
	hcan.pTxMsg->RTR=CAN_RTR_DATA;     //���͵�����֡
	hcan.pTxMsg->IDE=CAN_ID_STD;       //��׼����֡
	hcan.pTxMsg->DLC=8;                //���ݳ���
	hcan.pTxMsg->Data[0] = 'a';       //����
	hcan.pTxMsg->Data[1] = 'b';
	hcan.pTxMsg->Data[2] = 'c';
	hcan.pTxMsg->Data[3] = 'd';
	hcan.pTxMsg->Data[4] = '5';
	hcan.pTxMsg->Data[5] = '6';
	hcan.pTxMsg->Data[6] = '7';
	hcan.pTxMsg->Data[7] = '8';

}
void can_test1()
{
	HAL_CAN_Transmit(&hcan,10);
}




