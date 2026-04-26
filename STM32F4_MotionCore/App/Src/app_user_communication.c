#include "app_user_communication.h"

/* private variables */
static TaskHandle_t user_communication_handle = NULL;
static void user_communication_task(void* pvParameters);

static void APP_USER_COMMUNICATION_ConvertU32_to_U8(uint8_t* u8_buf, uint32_t u32_val)
{
	u8_buf[0] = (uint8_t)(u32_val & 0xFF);
	u8_buf[1] = (uint8_t)((u32_val >> 8) & 0xFF);
	u8_buf[2] = (uint8_t)((u32_val >> 16) & 0xFF);
	u8_buf[3] = (uint8_t)((u32_val >> 24) & 0xFF);
}

static void APP_USER_COMMUNICATION_ConvertI32_to_U8(uint8_t* u8_buf, int32_t i32_val)
{
	u8_buf[0] = (uint8_t)(i32_val & 0xFF);
	u8_buf[1] = (uint8_t)((i32_val >> 8) & 0xFF);
	u8_buf[2] = (uint8_t)((i32_val >> 16) & 0xFF);
	u8_buf[3] = (uint8_t)((i32_val >> 24) & 0xFF);
}

static uint32_t APP_USER_COMMUNICATION_ConvertU8_to_U32(uint8_t* u8_buf)
{
	uint32_t u32_val = 0;

	u32_val = (uint32_t)((u8_buf[3] << 24) | (u8_buf[2] << 16) | (u8_buf[1] << 8) | u8_buf[0]);

	return u32_val;
}

/* Initialize CAN bus */
void APP_USER_COMMUNICATION_Init(CAN_HandleTypeDef* pcan)
{
	APP_STATE_Update_Error_BeforeRTOSStart(ERR_CAN, ERR_ACTIVE);
	if(pcan != NULL)
	{
		for(uint32_t i = 0; i < 3; ++i)
		{
			if(BSP_MCU_COMMUNICATION_Init(pcan) == DRV_OK)
			{
				APP_STATE_Update_Error_BeforeRTOSStart(ERR_CAN, ERR_NOT_ACTIVE);
				break;
			}
		}
	}

	xTaskCreate(user_communication_task, "user_communication_task", 1024, NULL, 3, &user_communication_handle);
	configASSERT(user_communication_handle != NULL);
}

/* Main task for USER and Motor Control communication */
static void user_communication_task(void* pvParameters)
{
	/* Create CAN messages and headers */
	static CAN_TxMessage_t can_tx_buf[APP_CAN_TX_BUF_LEN];
	static CAN_TxHeaderTypeDef tx_headers[APP_CAN_TX_BUF_LEN];
	for (int i = 0; i < APP_CAN_TX_BUF_LEN; i++)
	{
	    can_tx_buf[i].ptx_header = &tx_headers[i];
	}

	static CAN_RxMessage_t can_rx_buf[APP_CAN_RX_BUF_LEN];

	static GlobalData_t g_data_state;

	const TickType_t xFrequency = pdMS_TO_TICKS(100);

	while(1)
	{
		TickType_t xLastWakeTime = xTaskGetTickCount();
		g_data_state = APP_STATE_Get_Data();

		/* Make transmit messages */
		APP_USER_COMMUNICATION_ConvertU32_to_U8(can_tx_buf[0].data, g_data_state.motor_state);
		can_tx_buf[0].data[0] = 0x01;
		can_tx_buf[0].ptx_header->DLC = 4;
		can_tx_buf[0].ptx_header->ExtId = 0;
		can_tx_buf[0].ptx_header->IDE = CAN_ID_STD;
		can_tx_buf[0].ptx_header->RTR = CAN_RTR_DATA;
		can_tx_buf[0].ptx_header->StdId = APP_CAN_MOTOR_STATE_ID;
		can_tx_buf[0].ptx_header->TransmitGlobalTime = DISABLE;

		APP_USER_COMMUNICATION_ConvertU32_to_U8(can_tx_buf[1].data, (uint32_t)(g_data_state.current_A * 1000.0f));
		can_tx_buf[1].data[0] = 0x02;
		can_tx_buf[1].ptx_header->DLC = 4;
		can_tx_buf[1].ptx_header->ExtId = 0;
		can_tx_buf[1].ptx_header->IDE = CAN_ID_STD;
		can_tx_buf[1].ptx_header->RTR = CAN_RTR_DATA;
		can_tx_buf[1].ptx_header->StdId = APP_CAN_CURRENT_mA_ID;
		can_tx_buf[1].ptx_header->TransmitGlobalTime = DISABLE;

		APP_USER_COMMUNICATION_ConvertU32_to_U8(can_tx_buf[2].data, (uint32_t)g_data_state.voltage_V);
		can_tx_buf[2].data[0] = 0x03;
		can_tx_buf[2].ptx_header->DLC = 4;
		can_tx_buf[2].ptx_header->ExtId = 0;
		can_tx_buf[2].ptx_header->IDE = CAN_ID_STD;
		can_tx_buf[2].ptx_header->RTR = CAN_RTR_DATA;
		can_tx_buf[2].ptx_header->StdId = APP_CAN_VOLTAGE_V_ID;
		can_tx_buf[2].ptx_header->TransmitGlobalTime = DISABLE;

		APP_USER_COMMUNICATION_ConvertI32_to_U8(can_tx_buf[3].data, (int32_t)g_data_state.temp);
		can_tx_buf[3].ptx_header->DLC = 4;
		can_tx_buf[3].ptx_header->ExtId = 0;
		can_tx_buf[3].ptx_header->IDE = CAN_ID_STD;
		can_tx_buf[3].ptx_header->RTR = CAN_RTR_DATA;
		can_tx_buf[3].ptx_header->StdId = APP_CAN_TEMPERATURE_ID;
		can_tx_buf[3].ptx_header->TransmitGlobalTime = DISABLE;

		APP_USER_COMMUNICATION_ConvertU32_to_U8(can_tx_buf[4].data, (uint32_t)g_data_state.real_motor_speed);
		can_tx_buf[4].data[0] = 0x05;
		can_tx_buf[4].ptx_header->DLC = 4;
		can_tx_buf[4].ptx_header->ExtId = 0;
		can_tx_buf[4].ptx_header->IDE = CAN_ID_STD;
		can_tx_buf[4].ptx_header->RTR = CAN_RTR_DATA;
		can_tx_buf[4].ptx_header->StdId = APP_CAN_MOTOR_SPEED_ID;
		can_tx_buf[4].ptx_header->TransmitGlobalTime = DISABLE;

		APP_USER_COMMUNICATION_ConvertU32_to_U8(can_tx_buf[5].data, (uint32_t)g_data_state.dev_state);
		can_tx_buf[5].data[0] = 0x06;
		can_tx_buf[5].ptx_header->DLC = 4;
		can_tx_buf[5].ptx_header->ExtId = 0;
		can_tx_buf[5].ptx_header->IDE = CAN_ID_STD;
		can_tx_buf[5].ptx_header->RTR = CAN_RTR_DATA;
		can_tx_buf[5].ptx_header->StdId = APP_CAN_LOGS_STATE_ID;
		can_tx_buf[5].ptx_header->TransmitGlobalTime = DISABLE;

		DevStatus_t ret = OSAL_USER_COMMUNICATION_SendMessage(can_tx_buf, APP_CAN_TX_BUF_LEN);

		if(ret != DRV_OK)
		{
			APP_STATE_Update_Error(ERR_CAN, ERR_ACTIVE);
		}
		else
		{
			APP_STATE_Update_Error(ERR_CAN, ERR_NOT_ACTIVE);
		}

		ret = OSAL_USER_COMMUNICATION_ReceiveMessage(can_rx_buf, APP_CAN_RX_BUF_LEN);

		if(ret != DRV_OK)
		{
			APP_STATE_Update_Error(ERR_CAN, ERR_ACTIVE);
		}
		else
		{
			APP_STATE_Update_Error(ERR_CAN, ERR_NOT_ACTIVE);
			uint32_t target_motor_speed = APP_USER_COMMUNICATION_ConvertU8_to_U32(can_rx_buf[0].data);
			APP_STATE_Set_Motor_TargetSpeed((float)target_motor_speed);
		}

		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}
