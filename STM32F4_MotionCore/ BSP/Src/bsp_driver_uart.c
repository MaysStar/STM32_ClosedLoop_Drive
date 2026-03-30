#include <bsp_driver_uart.h>

/* private UART BSP variables */
typedef void (*UART_TxRxCpltCallbak)(void);

static UART_TxRxCpltCallbak UART3TxCpltCallbak;

static UART_HandleTypeDef* local_ptask_uart2 = NULL;
static UART_HandleTypeDef* local_ptask_uart3 = NULL;

/* SET local pointer into the peripherals */
void BSP_UART_Init(UART_HandleTypeDef* huart2, UART_HandleTypeDef* huart3)
{
	local_ptask_uart2 = huart2;
	local_ptask_uart3 = huart3;
}

/* Send logs through UART2 */
void BSP_UART3_SendData(char* tx_buffer, uint32_t len)
{
	HAL_UART_Transmit_DMA(local_ptask_uart3, (uint8_t*)tx_buffer, len);
}

/* Register task notify function */
void BSP_UART3_RegisterTxCpltCallbak(void (*callback_fun)(void))
{
	UART3TxCpltCallbak = callback_fun;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{
		UART3TxCpltCallbak();
	}
}

