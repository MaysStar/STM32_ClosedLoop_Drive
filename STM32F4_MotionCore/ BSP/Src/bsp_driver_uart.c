#include <bsp_driver_uart.h>

/* Private values for UART process */
static UART_HandleTypeDef* local_puart1;
static UART_HandleTypeDef* local_puart2;
static UART_HandleTypeDef* local_puart3;

typedef void (*UART_TxRxCpltCallbak)(void);

static UART_TxRxCpltCallbak UART3TxCpltCallbak;

/* SET local pointer into the peripherals */
void BSP_UART_Init(UART_HandleTypeDef* huart1, UART_HandleTypeDef* huart2, UART_HandleTypeDef* huart3)
{
	local_puart1 = huart1;
	local_puart2 = huart2;
	local_puart3 = huart3;
}

/* Send logs through UART2 */
void BSP_UART3_SendData(char* tx_buffer, uint32_t len)
{
	HAL_UART_Transmit_DMA(local_puart3, (uint8_t*)tx_buffer, len);
}

/* Register task notify function */
void BSP_UART2_RegisterTxCpltCallbak(void (*callback_fun)(void))
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

