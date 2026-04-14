#include <bsp_driver_uart_log.h>

/* private UART_LOG BSP variables */
typedef void (*UART_TxRxCpltCallbak)(void);

static UART_TxRxCpltCallbak UART3TxCpltCallbak;

static UART_HandleTypeDef* local_puart3 = NULL;

/* SET local pointer into the peripherals */
DevStatus_t BSP_UART_LOG_Init(UART_HandleTypeDef* huart3)
{
	if((huart3 == NULL))
	{
		return DRV_INIT_NEEDED;
	}
	local_puart3 = huart3;

	return DRV_OK;
}

/* Send logs through UART3 */
DevStatus_t BSP_UART_LOG_SendData(char* tx_buffer, uint32_t len)
{
	return(DevStatus_t)HAL_UART_Transmit_DMA(local_puart3, (uint8_t*)tx_buffer, len);
}

/* Register task notify function */
DevStatus_t BSP_UART_LOG_RegisterTxCpltCallbak(void (*callback_fun)(void))
{
	if(callback_fun == NULL)
	{
		return DRV_INIT_NEEDED;
	}
	UART3TxCpltCallbak = callback_fun;

	return DRV_OK;
}

/* Overwrite HAL Tx/RxCpltCallback */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{
		if(UART3TxCpltCallbak != NULL)
		{
			UART3TxCpltCallbak();
		}
	}
}
