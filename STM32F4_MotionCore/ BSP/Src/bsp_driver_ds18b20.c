#include "bsp_driver_ds18b20.h"

/* BSP for 1-Wire DS18B20 */

static UART_HandleTypeDef* local_puart4_DS18B20 = NULL;

typedef void (*UART_TxRxCpltCallbak)(void);

static UART_TxRxCpltCallbak TxCpltCallbak;
static UART_TxRxCpltCallbak RxCpltCallbak;

static uint8_t tx_buf_read[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t rx_buf_read[8];

/* Change UART baudrate */
static DevStatus_t BSP_UART_SetBaudrate(uint32_t baudrate)
{
	local_puart4_DS18B20->Instance = UART4;
	local_puart4_DS18B20->Init.BaudRate = baudrate;
	local_puart4_DS18B20->Init.WordLength = UART_WORDLENGTH_8B;
	local_puart4_DS18B20->Init.StopBits = UART_STOPBITS_1;
	local_puart4_DS18B20->Init.Parity = UART_PARITY_NONE;
	local_puart4_DS18B20->Init.Mode = UART_MODE_TX_RX;
	local_puart4_DS18B20->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	local_puart4_DS18B20->Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_HalfDuplex_Init(local_puart4_DS18B20) != HAL_OK)
	{
		return DRV_INIT_NEEDED;
	}

	return DRV_OK;
}

/* Function for write at the same time as Initialization using polling mode UART transmit */
static DevStatus_t BSP_DS18B20_InitWrite(uint8_t data)
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

	return (DevStatus_t)HAL_UART_Transmit(local_puart4_DS18B20, tx_buf, 8, 100);
}

/* Function for read at the same time as Initialization using polling mode UART transmit and receive */
static DS18B20_ReadStatus_t BSP_DS18B20_InitRead(void)
{
	DS18B20_ReadStatus_t ret;
	ret.data = 0;

	uint8_t tx_init_byte = 0xFF;
	uint8_t rx_init_buf[8];

	/* Set data receiving and receive data */
	for(uint32_t i = 0; i < 8; ++i)
	{
		/* Transmit and receive at the same time */
		ret.state = (DevStatus_t)HAL_UART_Transmit(local_puart4_DS18B20, &tx_init_byte, 1, 10);
		if(ret.state != DRV_OK)
		{
			ret.state = DRV_INIT_NEEDED;
			return ret;
		}

		HAL_UART_Receive(local_puart4_DS18B20, &rx_init_buf[i], 1, 10);
		if(ret.state != DRV_OK)
		{
			ret.state = DRV_INIT_NEEDED;
			return ret;
		}
	}

	/* Change bytes into bits */
	for(uint32_t i = 0; i < 8; ++i)
	{
		if(rx_init_buf[i] >= 0xFE)
		{
			ret.data |= (1 << i);
		}
	}

	return ret;
}

/*Function which uses for setting Reset and Presence */
DevStatus_t BSP_DS18B20_ResetPresence(void)
{
	DevStatus_t ret;
	/* Special byte to use this two operation */
	uint8_t data_for_reset_presence = 0xF0;

	/* Set baudrate 9600 for timings */
	ret = BSP_UART_SetBaudrate(9600);
	if(ret != DRV_OK)
	{
		return DRV_INIT_NEEDED;
	}

	/* Reset */
	ret = (DevStatus_t)HAL_UART_Transmit(local_puart4_DS18B20, &data_for_reset_presence, 1, 100); // Low for 500+ ms
	if(ret != DRV_OK)
	{
		return ret;
	}

	/* Presence */
	ret = (DevStatus_t)HAL_UART_Receive(local_puart4_DS18B20, &data_for_reset_presence, 1, 100);

	if(ret != DRV_OK)
	{
		return ret;
	}

	ret = BSP_UART_SetBaudrate(115200);
	if(ret != DRV_OK)
	{
		return DRV_INIT_NEEDED;
	}

	if(data_for_reset_presence == 0xF0)
	{
		return DRV_ERROR;
	}

	return ret;
}

/* Function for use with OSAL. UART transmit with DMA */
DevStatus_t BSP_DS18B20_Write(uint8_t data)
{
	DevStatus_t ret;
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

	ret = (DevStatus_t)HAL_UART_Receive_DMA(local_puart4_DS18B20, rx_buf_read, 8);
	if(ret != DRV_OK)
	{
		return ret;
	}

	return (DevStatus_t)HAL_UART_Transmit_DMA(local_puart4_DS18B20, tx_buf, 8);
}

/* Couple of functions for use with OSAL. UART transmit and receive with DMA */
DevStatus_t BSP_DS18B20_ReadStart(void)
{
	DevStatus_t ret;
	/* Listening Rx and the same time transmit Tx */
	ret = (DevStatus_t)HAL_UART_Receive_DMA(local_puart4_DS18B20, rx_buf_read, 8);
	if(ret != DRV_OK)
	{
		return ret;
	}

	return (DevStatus_t)HAL_UART_Transmit_DMA(local_puart4_DS18B20, tx_buf_read, 8);
}

uint8_t BSP_DS18B20_ReadEnd(void)
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
uint8_t BSP_DS18B20_CalculateCRC(uint8_t* data, uint32_t len)
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
DevStatus_t BSP_DS18B20_Init(UART_HandleTypeDef* huart4)
{
	DevStatus_t ret;
	uint8_t command;

	if(huart4 == NULL)
	{
		return DRV_INIT_NEEDED;
	}
	local_puart4_DS18B20 = huart4;

	uint8_t scratchpad_buffer[9];

	ret = BSP_DS18B20_ResetPresence();
	if(ret != DRV_OK)
	{
		return DRV_INIT_NEEDED;
	}

	command = BSP_DS18B20_SKIP_ROM;
	ret = BSP_DS18B20_InitWrite(command); /* Skip ROM */
	if(ret != DRV_OK)
	{
		return DRV_INIT_NEEDED;
	}

	command = BSP_DS18B20_READ_SCRATCHPAD;
	ret = BSP_DS18B20_InitWrite(command); /* Read Scratchpad */
	if(ret != DRV_OK)
	{
		return DRV_INIT_NEEDED;
	}

	for(uint32_t i = 0; i < 9; ++i)
	{
		DS18B20_ReadStatus_t init_read = BSP_DS18B20_InitRead();
		if(init_read.state != DRV_OK)
		{
			return DRV_INIT_NEEDED;
		}
		scratchpad_buffer[i] = init_read.data;
	}

	/* TL and TH leave the same and change resolution into 9 bit */
	scratchpad_buffer[4] &= ~(1 << 5);
	scratchpad_buffer[4] &= ~(1 << 6);

	ret = BSP_DS18B20_ResetPresence();
	if(ret != DRV_OK)
	{
		return DRV_INIT_NEEDED;
	}

	command = BSP_DS18B20_SKIP_ROM;
	ret = BSP_DS18B20_InitWrite(command); /* Skip ROM */
	if(ret != DRV_OK)
	{
		return DRV_INIT_NEEDED;
	}

	command = BSP_DS18B20_WRITE_SCRATCHPAD;
	ret = BSP_DS18B20_InitWrite(command); /* Write Scratchpad */
	if(ret != DRV_OK)
	{
		return DRV_INIT_NEEDED;
	}

	for(uint32_t i = 2; i < 5; ++i)
	{
		ret = BSP_DS18B20_InitWrite(scratchpad_buffer[i]);
		if(ret != DRV_OK)
		{
			return DRV_INIT_NEEDED;
		}
	}

	ret = BSP_DS18B20_ResetPresence();
	if(ret != DRV_OK)
	{
		return DRV_INIT_NEEDED;
	}
	/*Skip first data check CRC */

	command = BSP_DS18B20_SKIP_ROM;
	ret = BSP_DS18B20_InitWrite(command); /* Skip ROM */
	if(ret != DRV_OK)
	{
		return DRV_INIT_NEEDED;
	}

	command = BSP_DS18B20_COPY_SCRATCHPAD;
	ret = BSP_DS18B20_InitWrite(command); /* Copy Scratchpad */
	if(ret != DRV_OK)
	{
		return DRV_INIT_NEEDED;
	}

	/* After issuing this command, the master must wait 10 ms for copy operation to complete */
	//HAL_Delay(10);

	ret = BSP_DS18B20_ResetPresence();
	if(ret != DRV_OK)
	{
		return DRV_INIT_NEEDED;
	}

	return ret;
}

/* Register callback functions */
DevStatus_t BSP_DS18B20_RegisterTxCpltCallbak(void (*callback_fun)(void))
{
	if(callback_fun == NULL)
	{
		return DRV_INIT_NEEDED;
	}
	TxCpltCallbak = callback_fun;

	return DRV_OK;
}

DevStatus_t BSP_DS18B20_RegisterRxCpltCallbak(void (*callback_fun)(void))
{
	if(callback_fun == NULL)
	{
		return DRV_INIT_NEEDED;
	}
	RxCpltCallbak = callback_fun;

	return DRV_OK;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == UART4)
	{
		if(RxCpltCallbak != NULL)
		{
			RxCpltCallbak();
		}
	}
}
