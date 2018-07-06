#ifndef _can_tx_h
#define _can_tx_h
#include "stm32f1xx.h"
#define PACK_HEAD 					0x7E
#define PACK_TAIL 					0xE7
#define NULL 						0
#define PROTOCOL_VERSION 			0x01
#define CURRENT_DEVICE_TYPE 		0x0f

#define MAX_PROTOCOL_EX_LEN	 		21				/*One pack's Max Length(exclude data region)*/
#define DATALEN_FIX_LENGTH   		15             /*Fixed length of data fields*/
#define CRC_REGION_OFFSET			(MAX_PROTOCOL_EX_LEN - 3)
#define PACK_TAIL_REGION_OFFSET 	(MAX_PROTOCOL_EX_LEN - 1)

#pragma pack(push, 1)
typedef struct
{
	u8 PackHead;
	u16 DataLength;
	u8 ProcotolVer;
	u8 DeviceType;
	u8 CmdCode;
	u32 SrcAddr;
	u32 DesAddr;
	u16 SerialNO;
	u8 TotalPackNum;
	u8 CurrentPackNO;
	u8 *Data;
}NB_ProtocolTypeDef;
#pragma pack(pop)

HAL_StatusTypeDef Pack(u8 CMD, u16 SerialNO,u32 STDAddr1,const void *pData, u32 DataLength);
HAL_StatusTypeDef UnPack(u32 *Addr,u8 *CMD, u16 *SerialNO, u8 *pCMDData, u32 *pCMDDataLength,\
const void *pData, int DataLength);
#endif

