#include "CanTx.h"
#include "CRC.h"
#include "stdlib.h"
#include "fifo.h"
#include "nubolib.h"

u8 STDSerialNO = 0;
extern struct fifo_data TransmitFIFO;


//打包一帧数据,放入发送队列
HAL_StatusTypeDef Pack(u8 CMD, u16 SerialNO,u32 STDAddr1,const void *pData, u32 DataLength)
{
	NB_ProtocolTypeDef *pPack=NULL;
	u16 CRCValue;
	u16 *pCRCValueRegion;
	u8 *pPackTail;

	pPack = (NB_ProtocolTypeDef *)malloc(DataLength + MAX_PROTOCOL_EX_LEN);
    if(0 == pPack)
    {
        return HAL_ERROR;
    }
	pPack->PackHead = PACK_HEAD;
	pPack->DataLength = DataLength + MAX_PROTOCOL_EX_LEN - 6;
	pPack->ProcotolVer = PROTOCOL_VERSION;
	pPack->DeviceType = (u8)CURRENT_DEVICE_TYPE;
	pPack->CmdCode = CMD;
	pPack->SrcAddr = STDAddr1;
	pPack->DesAddr = 0x1000000;
	pPack->TotalPackNum = 1;
	pPack->CurrentPackNO = 1;
    if (0 == SerialNO)
    {
        pPack->SerialNO = STDSerialNO;
    }
    else
    {
        pPack->SerialNO = SerialNO;
    }
	
    if (pData != NULL)
    {
        memcpy(&(pPack->Data), pData, DataLength);
    }

	CRCValue = ChkCrcPack(&(pPack->ProcotolVer), pPack->DataLength,0xFFFF);
	pCRCValueRegion = (u16 *)((u8 *)pPack + DataLength + CRC_REGION_OFFSET); 
	*pCRCValueRegion = CRCValue;

	pPackTail = (u8 *)((u8 *)pPack + DataLength + PACK_TAIL_REGION_OFFSET);
	*pPackTail = PACK_TAIL;

	/*Add to Transmit FIFO*/
    if (!fifo_puts(&TransmitFIFO, (u8 *)pPack, DataLength + MAX_PROTOCOL_EX_LEN))
    {
        free(pPack);
        return HAL_ERROR;
    }
    
	free(pPack);
	return HAL_OK;
}
//解析一帧数据
HAL_StatusTypeDef UnPack(u32 *Addr,u8 *CMD, u16 *SerialNO, u8 *pCMDData, u32 *pCMDDataLength,\
const void *pData, int DataLength)
{
	NB_ProtocolTypeDef *pPack;
	u16 CRCValue;
	u16 *pCRCValueRegion;
    u8 AddrTrueflag = 0;
    static u32 STDAddr1;
    
	if (CMD == NULL || pCMDData == NULL || pCMDDataLength== NULL || 
		pData == NULL || DataLength < MAX_PROTOCOL_EX_LEN)
	{
		return HAL_ERROR;
	}

	pPack = (NB_ProtocolTypeDef *)pData;
    STDAddr1 = pPack->DesAddr;
    STDAddr1 &=0x00FF0000;
    //if((STDAddr == STDAddr1) || (SubgroupSTDAddr == STDAddr1))
	if(1)
    {
        AddrTrueflag = 1;
    }
	if ((PACK_HEAD != pPack->PackHead) 				                                ||
		(PROTOCOL_VERSION != pPack->ProcotolVer) 	                                ||
		(CURRENT_DEVICE_TYPE != pPack->DeviceType) 	                                ||
        (AddrTrueflag != 1)                                                         ||
		(PACK_TAIL != *((u8 *)((u8 *)pPack + PACK_TAIL_REGION_OFFSET + pPack->DataLength - DATALEN_FIX_LENGTH))))
	{
		return HAL_ERROR;
	}

	CRCValue = ChkCrcPack(&(pPack->ProcotolVer), (pPack->DataLength),0xFFFF);
	pCRCValueRegion = (u16 *)((u8*)pPack + CRC_REGION_OFFSET
    +(pPack->DataLength - DATALEN_FIX_LENGTH));
	if (*pCRCValueRegion != CRCValue)
	{
		return HAL_ERROR;
	}
    *SerialNO = pPack->SerialNO;
	memcpy(pCMDData, &(pPack->Data), pPack->DataLength - DATALEN_FIX_LENGTH);
	*pCMDDataLength =pPack->DataLength - DATALEN_FIX_LENGTH;
	*CMD = pPack->CmdCode;
    *Addr = pPack->DesAddr;
	
	return HAL_OK;
}








