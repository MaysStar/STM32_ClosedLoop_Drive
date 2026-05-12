#include <bsp_driver_uart_log.h>

/* private UART_LOG BSP variables */
typedef void (*UART_TxRxCpltCallbak)(void);

static UART_TxRxCpltCallbak UART1TxCpltCallbak;

static UART_HandleTypeDef* local_puart1 = NULL;

/* SET local pointer into the peripherals */
DevStatus_t BSP_UART_LOG_Init(UART_HandleTypeDef* huart1)
{
	if((huart1 == NULL))
	{
		return DRV_INIT_NEEDED;
	}
	local_puart1 = huart1;

	return DRV_OK;
}

/* Send logs through UART3 */
DevStatus_t BSP_UART_LOG_SendData(char* tx_buffer, uint32_t len)
{
	return(DevStatus_t)HAL_UART_Transmit_DMA(local_puart1, (uint8_t*)tx_buffer, len);
}

/* Register task notify function */
DevStatus_t BSP_UART_LOG_RegisterTxCpltCallbak(void (*callback_fun)(void))
{
	if(callback_fun == NULL)
	{
		return DRV_INIT_NEEDED;
	}
	UART1TxCpltCallbak = callback_fun;

	return DRV_OK;
}

/* Overwrite HAL Tx/RxCpltCallback */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		if(UART1TxCpltCallbak != NULL)
		{
			UART1TxCpltCallbak();
		}
	}
}
