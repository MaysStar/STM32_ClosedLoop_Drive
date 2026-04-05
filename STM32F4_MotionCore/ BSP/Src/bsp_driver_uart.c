#include <bsp_driver_uart.h>

/* private UART BSP variables */
typedef void (*UART_TxRxCpltCallbak)(void);

static UART_TxRxCpltCallbak UART3TxCpltCallbak;

static UART_HandleTypeDef* local_puart2 = NULL;
static UART_HandleTypeDef* local_puart3 = NULL;

/* SET local pointer into the peripherals */
void BSP_UART_Init(UART_HandleTypeDef* huart2, UART_HandleTypeDef* huart3)
{
	local_puart2 = huart2;
	local_puart3 = huart3;
}

/* Send logs through UART2 */
void BSP_UART3_SendData(char* tx_buffer, uint32_t len)
{
	HAL_UART_Transmit_DMA(local_puart3, (uint8_t*)tx_buffer, len);
}

/* Register task notify function */
void BSP_UART3_RegisterTxCpltCallbak(void (*callback_fun)(void))
{
	UART3TxCpltCallbak = callback_fun;
}


/* BSP for 1-Wire DS18B20 */

static UART_HandleTypeDef* local_puart4_1wire = NULL;

static UART_TxRxCpltCallbak UART_1WireTxCpltCallbak;
static UART_TxRxCpltCallbak UART_1WireRxCpltCallbak;

static uint8_t tx_buf_read[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t rx_buf_read[8];

/* Change UART baudrate */
static void BSP_UART_1Wire_SetBaudrate(uint32_t baudrate)
{
	local_puart4_1wire->Instance = UART4;
	local_puart4_1wire->Init.BaudRate = baudrate;
	local_puart4_1wire->Init.WordLength = UART_WORDLENGTH_8B;
	local_puart4_1wire->Init.StopBits = UART_STOPBITS_1;
	local_puart4_1wire->Init.Parity = UART_PARITY_NONE;
	local_puart4_1wire->Init.Mode = UART_MODE_TX_RX;
	local_puart4_1wire->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	local_puart4_1wire->Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_HalfDuplex_Init(local_puart4_1wire) != HAL_OK)
	{
		Error_Handler();
	}
}

/* Function for write at the same time as Initialization using polling mode UART transmit */
static void BSP_UART_1WireDS18B20_InitWrite(uint8_t data)
{
	uint8_t tx_buf[8];

	/* Change bits into bytes */
	for(uint32_t i = 0; i < 8; ++i)
	{
		if(((data >> i) & 0x1 ) == 0x1)
		{
			tx_buf[i] = 0xFF;
		}
		else
		{
			tx_buf[i] = 0x00;
		}
	}

	HAL_UART_Transmit(local_puart4_1wire, tx_buf, 8, 100);
}

/* Function for read at the same time as Initialization using polling mode UART transmit and receive */
static uint8_t BSP_UART_1WireDS18B20_InitRead(void)
{
	uint8_t res_value = 0;
	uint8_t tx_init_byte = 0xFF;
	uint8_t rx_init_buf[8];

	/* Set data receiving and receive data */
	for(uint32_t i = 0; i < 8; ++i)
	{
		/* Transmit and receive at the same time */
		HAL_UART_Transmit(local_puart4_1wire, &tx_init_byte, 1, 10);
		HAL_UART_Receive(local_puart4_1wire, &rx_init_buf[i], 1, 10);
	}

	/* Change bytes into bits */
	for(uint32_t i = 0; i < 8; ++i)
	{
		if(rx_init_buf[i] >= 0xFE)
		{
			res_value |= (1 << i);
		}
	}

	return res_value;
}

/*Function which uses for setting Reset and Presence
 * return 0 response detected
 * 		  1 failed
 * 		  2 no response
 */
int BSP_UART_1WireDS18B20_ResetPresence(void)
{
	/* Special byte to use this two operation */
	uint8_t data_for_reset_presence = 0xF0;

	/* Set baudrate 9600 for timings */
	BSP_UART_1Wire_SetBaudrate(9600);

	/* Reset */
	HAL_UART_Transmit(local_puart4_1wire, &data_for_reset_presence, 1, 100); // Low for 500+ ms

	/* Presence */
	if(HAL_UART_Receive(local_puart4_1wire, &data_for_reset_presence, 1, 1000) != HAL_OK)
	{
		return 1;
	}

	BSP_UART_1Wire_SetBaudrate(115200);
	if(data_for_reset_presence == 0xF0) return 2;

	return 0;
}

/* Function for use with OSAL. UART transmit with DMA */
void BSP_UART_1WireDS18B20_Write(uint8_t data)
{
	static uint8_t tx_buf[8];

	/* Change bits into bytes */
	for(uint32_t i = 0; i < 8; ++i)
	{
		if(((data >> i) & 0x1 ) == 0x1)
		{
			tx_buf[i] = 0xFF;
		}
		else
		{
			tx_buf[i] = 0x00;
		}
	}

	HAL_UART_Receive_DMA(local_puart4_1wire, rx_buf_read, 8);
	HAL_UART_Transmit_DMA(local_puart4_1wire, tx_buf, 8);
}

/* Couple of functions for use with OSAL. UART transmit and receive with DMA */
void BSP_UART_1WireDS18B20_ReadStart(void)
{
	/* Listening Rx and the same time transmit Tx */
	HAL_UART_Receive_DMA(local_puart4_1wire, rx_buf_read, 8);
	HAL_UART_Transmit_DMA(local_puart4_1wire, tx_buf_read, 8);
}

uint8_t BSP_UART_1WireDS18B20_ReadEnd(void)
{
	uint8_t res_value = 0;
	/* Change bytes into bits */
	for(uint32_t i = 0; i < 8; ++i)
	{
		if(rx_buf_read[i] == 0xFF)
		{
			res_value |= (1 << i);
		}
	}

	return res_value;
}

/* Function to calculate the CRC */
uint8_t BSP_UART_1WireDS18B20_CalculateCRC(uint8_t* data, uint32_t len)
{
	/* Start value */
	uint8_t crc = 0x00;

	for(uint32_t i = 0; i < len; ++i)
	{
		uint8_t inbyte = data[i];
		for(uint32_t j = 0; j < 8; ++j)
		{
			uint8_t mix = ((crc ^ inbyte) & 0x1);
			crc >>= 1;
			if(mix == 1)
			{
				crc ^= 0x8C;
			}
			inbyte >>= 1;
		}
	}

	return crc;
}

/* Initialize DS18B20 module set approximately 100ms measurement temperature */
void BSP_UART_1WireDS18B20_Init(UART_HandleTypeDef* huart4)
{
	local_puart4_1wire = huart4;

	uint8_t scratchpad_buffer[9];

	if(BSP_UART_1WireDS18B20_ResetPresence() != 0)
	{
		/* Start fail */
		return;
	}

	BSP_UART_1WireDS18B20_InitWrite(0xCC); /* Skip ROM */
	BSP_UART_1WireDS18B20_InitWrite(0xBE); /* Read Scratchpad */

	for(uint32_t i = 0; i < 9; ++i)
	{
		scratchpad_buffer[i] = BSP_UART_1WireDS18B20_InitRead();
	}

	/* TL and TH leave the same and change resolution into 9 bit */
	scratchpad_buffer[4] &= ~(1 << 5);
	scratchpad_buffer[4] &= ~(1 << 6);

	if(BSP_UART_1WireDS18B20_ResetPresence() != 0)
	{
		/* Start fail */
		return;
	}

	BSP_UART_1WireDS18B20_InitWrite(0xCC); /* Skip ROM */
	BSP_UART_1WireDS18B20_InitWrite(0x4E); /* Write Scratchpad */

	BSP_UART_1WireDS18B20_InitWrite(scratchpad_buffer[2]);
	BSP_UART_1WireDS18B20_InitWrite(scratchpad_buffer[3]);
	BSP_UART_1WireDS18B20_InitWrite(scratchpad_buffer[4]);

	if(BSP_UART_1WireDS18B20_ResetPresence() != 0)
	{
		/* Start fail */
		return;
	}

	/*Skip first data check CRC */

	BSP_UART_1WireDS18B20_InitWrite(0xCC); /* Skip ROM */
	BSP_UART_1WireDS18B20_InitWrite(0x48); /* Copy Scratchpad */

	/* After issuing this command, the master must wait 10 ms for copy operation to complete */
	HAL_Delay(10);

	BSP_UART_1WireDS18B20_ResetPresence();
}

/* Register callback functions */
void BSP_UART_1Wire_RegisterTxCpltCallbak(void (*callback_fun)(void))
{
	UART_1WireTxCpltCallbak = callback_fun;
}

void BSP_UART_1Wire_RegisterRxCpltCallbak(void (*callback_fun)(void))
{
	UART_1WireRxCpltCallbak = callback_fun;
}

/* Overwrite HAL Tx/RxCpltCallback */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{
		UART3TxCpltCallbak();
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == UART4)
	{
		UART_1WireRxCpltCallbak();
	}
}
