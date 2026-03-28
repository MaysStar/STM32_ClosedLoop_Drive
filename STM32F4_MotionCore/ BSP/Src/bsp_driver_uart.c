#include <bsp_deiver_uart.h>

/* Private values for UART process */
static UART_HandleTypeDef* local_puart1;
static UART_HandleTypeDef* local_puart2;

typedef void (*UART_TxRxCpltCallbak)(void);

static UART_TxRxCpltCallbak UART2TxCpltCallbak;

/* SET local pointer into the peripherals */
void BSP_UART_Init(UART_HandleTypeDef* huart1, UART_HandleTypeDef* huart2)
{
	local_puart1 = huart1;
	local_puart2 = huart2;
}

/* Send logs through UART2 */
void BSP_UART2_SendData(char* tx_buffer, uint32_t len)
{
	HAL_UART_Transmit_DMA(local_puart2, (uint8_t*)tx_buffer, len);
}

/* Register task notify function */
void BSP_UART2_RegisterTxCpltCallbak(void (*callback_fun)(void))
{
	UART2TxCpltCallbak = callback_fun;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2)
	{
		UART2TxCpltCallbak();
	}
}

