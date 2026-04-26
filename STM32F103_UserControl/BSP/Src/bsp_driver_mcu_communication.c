#include "bsp_driver_mcu_communication.h"

/* private variables and functions */
static CAN_HandleTypeDef* local_pcan = NULL;

static bsp_callback_fun CAN_TxCpltCallbak = NULL;
static bsp_callback_fun CAN_RxFifo0MsgPendingCallback = NULL;

/* Initialize local CAN pointer and start communication */
DevStatus_t BSP_MCU_COMMUNICATION_Init(CAN_HandleTypeDef* pcan)
{
	if(pcan != NULL)
	{
		local_pcan = pcan;

		/* Register FIFO0 for normal messages and FIFO1 for ERRORS */
		CAN_FilterTypeDef sFilterConfigFifo0 = {0};
		sFilterConfigFifo0.FilterActivation = CAN_FILTER_ENABLE;
		sFilterConfigFifo0.FilterBank = 0;
		sFilterConfigFifo0.FilterMode = CAN_FILTERMODE_IDMASK;
		sFilterConfigFifo0.FilterScale = CAN_FILTERSCALE_32BIT;
		sFilterConfigFifo0.FilterIdHigh = (0x0 << 5);
		sFilterConfigFifo0.FilterIdLow = 0x0000;
		sFilterConfigFifo0.FilterMaskIdHigh = (0x7F8 << 5);
		sFilterConfigFifo0.FilterMaskIdLow = 0x0000;
		sFilterConfigFifo0.FilterFIFOAssignment = CAN_RX_FIFO0;

		DevStatus_t ret = (DevStatus_t)HAL_CAN_ConfigFilter(local_pcan, &sFilterConfigFifo0);
		if(ret != DRV_OK)
		{
			return DRV_INIT_NEEDED;
		}

		/* Start communication */
		ret = (DevStatus_t)HAL_CAN_Start(local_pcan);
		if(ret != DRV_OK)
		{
			return DRV_INIT_NEEDED;
		}

		ret = (DevStatus_t)HAL_CAN_ActivateNotification(local_pcan, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING);

		return ret;
	}

	return DRV_INIT_NEEDED;
}

/* Send data into queue and wait until it transmit */
DevStatus_t BSP_MCU_COMMUNICATION_SendMessage(CAN_TxMessage_t* can_msg)
{
	uint32_t TxMailbox;
	if(can_msg != NULL)
	{
		DevStatus_t ret = (DevStatus_t)HAL_CAN_AddTxMessage(local_pcan, can_msg->ptx_header, can_msg->data, &TxMailbox);

		return ret;
	}

	return DRV_ERROR;
}

/* Get message from other MCU */
DevStatus_t BSP_MCU_COMMUNICATION_ReceiveMessage(CAN_RxMessage_t* rx_msg, uint32_t msg_fifo_flag)
{
	if(rx_msg != NULL)
	{
		return (DevStatus_t)HAL_CAN_GetRxMessage(local_pcan, msg_fifo_flag,  &rx_msg->rx_header_data,  rx_msg->data);
	}
	return DRV_ERROR;
}

/* Get free mainbox */
uint32_t BSP_MCU_COMMUNICATION_GetTxMailbox(void)
{
	return HAL_CAN_GetTxMailboxesFreeLevel(local_pcan);
}

/* Register callback */
DevStatus_t BSP_MCU_COMMUNICATION_RegisterTxCpltCallback(void(*callback_fun)(void))
{
	if(callback_fun != NULL)
	{
		CAN_TxCpltCallbak = callback_fun;
		return DRV_OK;
	}
	return DRV_INIT_NEEDED;
}

DevStatus_t BSP_MCU_COMMUNICATION_RegisterRxFifo0MsgPendingCallback(void(*callback_fun)(void))
{
	if(callback_fun != NULL)
	{
		CAN_RxFifo0MsgPendingCallback = callback_fun;
		return DRV_OK;
	}
	return DRV_INIT_NEEDED;
}

/* Callback */
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan->Instance == CAN1)
	{
		if(CAN_TxCpltCallbak != NULL)
		{
			CAN_TxCpltCallbak();
		}
	}
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan->Instance == CAN1)
	{
		if(CAN_TxCpltCallbak != NULL)
		{
			CAN_TxCpltCallbak();
		}
	}
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan->Instance == CAN1)
	{
		if(CAN_TxCpltCallbak != NULL)
		{
			CAN_TxCpltCallbak();
		}
	}
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan->Instance == CAN1)
	{
		if(CAN_RxFifo0MsgPendingCallback != NULL)
		{
			CAN_RxFifo0MsgPendingCallback();
		}
	}
}
