#ifndef BSP_DRIVER_MCU_COMMUNICATION_H
#define BSP_DRIVER_MCU_COMMUNICATION_H

#include "bsp_common.h"
#include "stm32f4xx_hal.h"

#define BSP_CAN_RECEIVE_FIFO0 CAN_RX_FIFO0
#define BSP_CAN_RECEIVE_FIFO1 CAN_RX_FIFO1

typedef struct
{
	CAN_TxHeaderTypeDef* ptx_header;
	uint8_t data[8];
}CAN_TxMessage_t;

typedef struct
{
	CAN_RxHeaderTypeDef rx_header_data;
	uint8_t data[8];
}CAN_RxMessage_t;

DevStatus_t BSP_MCU_COMMUNICATION_Init(CAN_HandleTypeDef* pcan);
DevStatus_t BSP_MCU_COMMUNICATION_SendMessage(CAN_TxMessage_t* can_msg);
DevStatus_t BSP_MCU_COMMUNICATION_ReceiveMessage(CAN_RxMessage_t* rx_msg, uint32_t msg_fifo_flag);

uint32_t BSP_MCU_COMMUNICATION_GetTxMailbox(void);

DevStatus_t BSP_MCU_COMMUNICATION_RegisterTxCpltCallback(void(*callback_fun)(void));
DevStatus_t BSP_MCU_COMMUNICATION_RegisterRxFifo0MsgPendingCallback(void(*callback_fun)(void));

#endif /* BSP_DRIVER_MCU_COMMUNICATION_H */
