#include "CanRx.h"
#include "Fifo.h"
#include <string.h>
#include "CanTx.h"
#include "NewCanTx.h"
#define can_test
#ifdef can_test
#define MAX_RECEIVE_QUEUE_LENGTH  1000U
struct fifo_data ReceiveFIFO;
u8 ReceiveQueue[MAX_RECEIVE_QUEUE_LENGTH];
u8 Data[1024];
#define MAX_TRANSMIT_QUEUE_LENGTH 1000U
u8 TransmitQueue[MAX_TRANSMIT_QUEUE_LENGTH];
struct fifo_data TransmitFIFO;

enum
{
	NBF_MIDDLE = 0 << 6,					//Pack's Middle CAN data frame 0x00
	NBF_HEAD   = 1 << 6,					//Pack's Head CAN data frame 0x40
	NBF_TAIL   = 2 << 6,					//Pack's Tail CAN data frame 0x80
	NBF_SINGLE = 3 << 6,					//Single frame,means Pack's length less than 8 0xc0
};
extern CAN_HandleTypeDef hcan;
extern uint8_t Rxflag;
uint8_t RxBuf[1024];
uint16_t RxIndex=0;
void Init_FIFO()
{
	fifo_Init(&ReceiveFIFO, ReceiveQueue, MAX_RECEIVE_QUEUE_LENGTH);
	fifo_Init(&TransmitFIFO, TransmitQueue, MAX_TRANSMIT_QUEUE_LENGTH);
}
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{   
	Recivce();
	HAL_CAN_Receive_IT(hcan,CAN_FIFO0);//回调中要重开接收中断。注意CAN_FIFO0。
}
void Recivce()
{
	if(hcan.pRxMsg->Data[0] == 0x40)//head
	{
		memcpy(&RxBuf[0],&(hcan.pRxMsg->Data[1]),hcan.pRxMsg->DLC - 1);
		RxIndex = hcan.pRxMsg->DLC - 1;		
	}
	else if(hcan.pRxMsg->Data[0] == 0x00)//middle
	{
		memcpy(&RxBuf[RxIndex],&(hcan.pRxMsg->Data[1]),hcan.pRxMsg->DLC - 1);
		RxIndex += hcan.pRxMsg->DLC -1;
	}
	else if(hcan.pRxMsg->Data[0] == 0x80)//end
	{
		memcpy(&RxBuf[RxIndex],&(hcan.pRxMsg->Data[1]),hcan.pRxMsg->DLC - 1);
		RxIndex += hcan.pRxMsg->DLC -1;
		fifo_puts(&ReceiveFIFO, RxBuf, RxIndex);
		Rxflag = 1;	//一条完整的指令
	}
}
extern CAN_HandleTypeDef hcan;
extern  CanTxMsgTypeDef        TxMessage;
uint16_t TransmitIndex=0,TransmitLength=0;
uint8_t  TransmitBuf[1000];
HAL_StatusTypeDef CAN_Transmit_Remain(CAN_HandleTypeDef* hcan)//传输2-last
{
	CanTxMsgTypeDef txMsg;
    int16_t Length;
    
    if (TransmitIndex >= TransmitLength)
    {
        return HAL_OK;
    }
    
    Length = TransmitLength - TransmitIndex;
	if (Length < 8)							//The last Frame
	{
		txMsg.Data[0] = NBF_TAIL;
		txMsg.StdId = 0;
		memcpy(&txMsg.Data[1], &TransmitBuf[TransmitIndex], Length);
		txMsg.DLC = Length + 1;
        TransmitIndex += Length;
	}
	else
	{
        Length = 7;
        
		txMsg.Data[0] = NBF_MIDDLE;			//Middle Frame
		txMsg.StdId = 0;
		memcpy(&txMsg.Data[1], &TransmitBuf[TransmitIndex], Length);
		TransmitIndex += Length;
		txMsg.DLC = 8;
	}
//	txMsg.StdId = 0x125;
	txMsg.IDE = CAN_ID_STD;
	txMsg.RTR = CAN_RTR_DATA;
	hcan->pTxMsg = &txMsg;
	
	if(HAL_CAN_Transmit(hcan, 0) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	return HAL_OK;
}
HAL_StatusTypeDef CAN_Transmit(CAN_HandleTypeDef* hcan, uint8_t *pData, uint16_t Length)//传输地一包
{
	CanTxMsgTypeDef txMsg;

	if (0 == pData)
	{
		return HAL_ERROR;
	}
    
//    TransmitIsBusy = TRUE;                  //Set busy flag
    
	if (Length < 8)							//Single Frame
	{
		txMsg.Data[0] = NBF_SINGLE;
		txMsg.StdId = 0x124;
		memcpy(&txMsg.Data[1], pData, Length);
		txMsg.DLC = Length + 1;
        TransmitIndex = Length;
	}
	else
	{
		memcpy(TransmitBuf, pData, Length);
		TransmitLength = Length;

		txMsg.Data[0] = NBF_HEAD;			//The First Frame
		txMsg.StdId = 0x123;
		memcpy(&txMsg.Data[1], pData, 7);
		TransmitIndex = 7;
		txMsg.DLC = 8;
	}

	txMsg.IDE = CAN_ID_STD;
	txMsg.RTR = CAN_RTR_DATA;
	hcan->pTxMsg = &txMsg;
	
	if(HAL_CAN_Transmit(hcan, 0) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	return HAL_OK;
}
u8 data2[1024];
u8 command;
u16 Serial;
u32 Length2,addr;
void c_t()
{
	u8 dat[] = "can test info test";
	u32 Length;

	if (!fifo_empty(&ReceiveFIFO))
	{
		if (fifo_get_frame(&ReceiveFIFO, Data, &Length))
		{
			;//进行解包分析命令,不带校验
			UnPack(&addr,&command, &Serial,data2, &Length2, &Data, Length);
			if(command == 0xa0)//执行命令
				Pack(0xa0,0x00,0x00,dat,sizeof(dat));//打包数据
			else if(command == 0xa1)
				Pack(0xa1,0x00,0x00,NULL,NULL);//打包数据
		}
	}
	if (!fifo_empty(&TransmitFIFO))
	{
		/*Get Data*/
		if (fifo_get_frame(&TransmitFIFO, Data, &Length))
        {
            CAN_Transmit(&hcan, Data, Length);
        }
	}
}
void huadle()
{
	;//CAN_Transmit_Remain(&hcan);
}


#endif






