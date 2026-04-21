#include "osal_freertos.h"

/* private variables */
static TaskHandle_t curr_housekeeping_task = NULL;
static TaskHandle_t curr_spi_task = NULL;

static SemaphoreHandle_t m_adc1 = NULL;

static SemaphoreHandle_t m_rtc = NULL;

static SemaphoreHandle_t m_spi1 = NULL;

volatile static uint8_t fill_buf[FILL_BUF_SIZE];

/* SPI1 DMA transmit callback function */
static void SPI1_DmaTxCpltCallback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if (curr_spi_task != NULL)
	{
		vTaskNotifyGiveFromISR(curr_spi_task , &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

/* ADC1 Mutex register, take and give */
static void ADC1_MutexTake(void)
{
	if(m_adc1 != NULL)
	{
		xSemaphoreTake(m_adc1, portMAX_DELAY);
	}
}

static void ADC1_MutexGive(void)
{
	if(m_adc1 != NULL)
	{
		xSemaphoreGive(m_adc1);
	}
}

/* RTC take and give semaphore */
static void RTC_MutexTake(void)
{
	if(m_rtc!= NULL)
	{
		xSemaphoreTake(m_rtc, portMAX_DELAY);
	}
}

static void RTC_MutexGive(void)
{
	if(m_rtc != NULL)
	{
		xSemaphoreGive(m_rtc);
	}
}

/* SPI1 take and give semaphore */
static void SPI1_MutexTake(void)
{
	if(m_spi1!= NULL)
	{
		xSemaphoreTake(m_spi1, portMAX_DELAY);
	}
}

static void SPI1_MutexGive(void)
{
	if(m_spi1 != NULL)
	{
		xSemaphoreGive(m_spi1);
	}
}

static void ADC1_RegisterMutex(void)
{
	m_adc1 = xSemaphoreCreateMutex();
}

static void RTC_RegisterMutex(void)
{
	m_rtc = xSemaphoreCreateMutex();
}

static void SPI1_RegisterMutex(void)
{
	m_spi1 = xSemaphoreCreateMutex();
}


/* Initialize ALL freeRTOS structures and callback for BSP */
DevStatus_t OSAL_Init(void)
{
	ADC1_RegisterMutex();
	RTC_RegisterMutex();
	SPI1_RegisterMutex();

	DevStatus_t ret = BSP_ST7735S_RegisterDmaTxCpltCallback(SPI1_DmaTxCpltCallback);

	return ret;
}

/* Getting target motor speed from potentiometer */
DevStatus_t OSAL_GET_MOTOR_TARGET_SPEED(uint32_t* motor_target_speed)
{
	DevStatus_t ret;
	/* Take Mutex*/
	ADC1_MutexTake();

	/* BSP get calculated target motor speed */
	ret = BSP_POTENTIOMETER_GetMotorTargetSpeed(motor_target_speed);

	/* Give Mutex*/
	ADC1_MutexGive();

	return ret;
}

/* Function for thread save data get for tasks. The function guarantees RTC operation will be completed */
DevStatus_t OSAL_RTC_GetDataDateTime(RTC_DateTypeDef* pdate, RTC_TimeTypeDef* ptime)
{
	/* Take Mutex and set curr_task value */
	RTC_MutexTake();

	curr_housekeeping_task = xTaskGetCurrentTaskHandle();

	DevStatus_t ret = BSP_RTC_GetDataDateTime(pdate, ptime);

	/* Give Mutex and set curr_task NULL */
	curr_housekeeping_task = NULL;

	RTC_MutexGive();

	return ret;
}

// map for change symbol fomat for display
static const CharMap_t font5x7[] =
{
    {' ', {0x00, 0x00, 0x00, 0x00, 0x00}},
    {'.', {0x00, 0x60, 0x60, 0x00, 0x00}},
    {':', {0x00, 0x36, 0x36, 0x00, 0x00}},
    {'%', {0x23, 0x13, 0x08, 0x64, 0x62}},
    {'0', {0x3e, 0x51, 0x49, 0x45, 0x3e}},
    {'1', {0x00, 0x42, 0x7f, 0x40, 0x00}},
    {'2', {0x42, 0x61, 0x51, 0x49, 0x46}},
    {'3', {0x21, 0x41, 0x45, 0x4b, 0x31}},
    {'4', {0x18, 0x14, 0x12, 0x7f, 0x10}},
    {'5', {0x27, 0x45, 0x45, 0x45, 0x39}},
    {'6', {0x3c, 0x4a, 0x49, 0x49, 0x30}},
    {'7', {0x01, 0x71, 0x09, 0x05, 0x03}},
    {'8', {0x36, 0x49, 0x49, 0x49, 0x36}},
    {'9', {0x06, 0x49, 0x49, 0x29, 0x1e}},
    {'A', {0x7e, 0x11, 0x11, 0x11, 0x7e}},
    {'B', {0x7f, 0x49, 0x49, 0x49, 0x36}},
    {'C', {0x3e, 0x41, 0x41, 0x41, 0x22}},
    {'D', {0x7f, 0x41, 0x41, 0x22, 0x1c}},
    {'E', {0x7f, 0x49, 0x49, 0x49, 0x41}},
    {'F', {0x7f, 0x09, 0x09, 0x09, 0x01}},
    {'G', {0x3e, 0x41, 0x49, 0x49, 0x7a}},
    {'H', {0x7f, 0x08, 0x08, 0x08, 0x7f}},
    {'I', {0x00, 0x41, 0x7f, 0x41, 0x00}},
    {'J', {0x20, 0x40, 0x41, 0x3f, 0x01}},
    {'K', {0x7f, 0x08, 0x14, 0x22, 0x41}},
    {'L', {0x7f, 0x40, 0x40, 0x40, 0x40}},
    {'M', {0x7f, 0x02, 0x0c, 0x02, 0x7f}},
    {'N', {0x7f, 0x04, 0x08, 0x10, 0x7f}},
    {'O', {0x3e, 0x41, 0x41, 0x41, 0x3e}},
    {'P', {0x7f, 0x09, 0x09, 0x09, 0x06}},
    {'Q', {0x3e, 0x41, 0x51, 0x21, 0x5e}},
    {'R', {0x7f, 0x09, 0x19, 0x29, 0x46}},
    {'S', {0x46, 0x49, 0x49, 0x49, 0x31}},
    {'T', {0x01, 0x01, 0x7f, 0x01, 0x01}},
    {'U', {0x3f, 0x40, 0x40, 0x40, 0x3f}},
    {'V', {0x1f, 0x20, 0x40, 0x20, 0x1f}},
    {'W', {0x3f, 0x40, 0x38, 0x40, 0x3f}},
    {'X', {0x63, 0x14, 0x08, 0x14, 0x63}},
    {'Y', {0x07, 0x08, 0x70, 0x08, 0x07}},
    {'Z', {0x61, 0x51, 0x49, 0x45, 0x43}},
    {'a', {0x20, 0x54, 0x54, 0x54, 0x78}},
    {'b', {0x7f, 0x48, 0x44, 0x44, 0x38}},
    {'c', {0x38, 0x44, 0x44, 0x44, 0x20}},
    {'d', {0x38, 0x44, 0x44, 0x48, 0x7f}},
    {'e', {0x38, 0x54, 0x54, 0x54, 0x18}},
    {'f', {0x08, 0x7e, 0x09, 0x01, 0x02}},
    {'g', {0x18, 0xa4, 0xa4, 0xa4, 0x7c}},
    {'h', {0x7f, 0x08, 0x04, 0x04, 0x78}},
    {'i', {0x00, 0x44, 0x7d, 0x40, 0x00}},
    {'j', {0x40, 0x80, 0x84, 0x7d, 0x00}},
    {'k', {0x7f, 0x10, 0x28, 0x44, 0x00}},
    {'l', {0x00, 0x41, 0x7f, 0x40, 0x00}},
    {'m', {0x7c, 0x04, 0x18, 0x04, 0x78}},
    {'n', {0x7c, 0x08, 0x04, 0x04, 0x78}},
    {'o', {0x38, 0x44, 0x44, 0x44, 0x38}},
    {'p', {0xfc, 0x24, 0x24, 0x24, 0x18}},
    {'q', {0x18, 0x24, 0x24, 0x18, 0xfc}},
    {'r', {0x7c, 0x08, 0x04, 0x04, 0x08}},
    {'s', {0x48, 0x54, 0x54, 0x54, 0x24}},
    {'t', {0x04, 0x3f, 0x44, 0x40, 0x20}},
    {'u', {0x3c, 0x40, 0x40, 0x20, 0x7c}},
    {'v', {0x1c, 0x20, 0x40, 0x20, 0x1c}},
    {'w', {0x3c, 0x40, 0x30, 0x40, 0x3c}},
    {'x', {0x44, 0x28, 0x10, 0x28, 0x44}},
    {'y', {0x1c, 0xa0, 0xa0, 0xa0, 0x7c}},
    {'z', {0x44, 0x64, 0x54, 0x4c, 0x44}},
    {'-', {0x08, 0x08, 0x08, 0x08, 0x08}},
};

/* Get array for writing */
static const uint8_t* OSAL_ST7735_GetBitmap(char symbol)
{
	uint32_t map_size = sizeof(font5x7) / sizeof(font5x7[0]);
	for(uint32_t i = 0; i < map_size; ++i)
	{
		if(font5x7[i].symbol == symbol)
		{
			return font5x7[i].bytes;
		}
	}

	return font5x7[0].bytes;
}

/* Send command in OSAL layer */
static DevStatus_t OSAL_ST7735_SendCommand(ST7735S_Command cmd)
{
	/* Wait when SPI became ready */
	while(BSP_ST7735S_State() != DRV_OK) {
		taskYIELD();
	}

	/* Polling API, so don't use DMA */
	DevStatus_t ret = BSP_ST7735_SendCommand(cmd);

	return ret;
}

/* Send data in OSAL layer */
static DevStatus_t OSAL_ST7735_SendData(uint8_t* data_buf, uint32_t data_len)
{
	/* Clear previous notify */
	ulTaskNotifyTake(pdTRUE, 0);

	/* Wait when SPI became ready */
	while(BSP_ST7735S_State() != DRV_OK) {
		taskYIELD();
	}

	DevStatus_t ret = BSP_ST7735_SendData(data_buf, data_len);

	if(ulTaskNotifyTake(pdTRUE, 300) == 0)
	{
		BSP_ST7735S_STOPOperation();
		return DRV_ERROR;
	}

	return ret;
}

/* Address some area for color filling */
static DevStatus_t OSAL_ST7732_AddressWindow(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	DevStatus_t ret;

	/* Address column */
	ST7735S_Command cmd = CASET;
	ret = OSAL_ST7735_SendCommand(cmd);
	if(ret != DRV_OK)
	{
		return ret;
	}

	 // offset and coordinate
	static uint8_t column_cord_buf[4];
	column_cord_buf[0] = 0x00;
	column_cord_buf[1] = x1 + defXSTART;
	column_cord_buf[2] = 0x00;
	column_cord_buf[3] = x2 + defXSTART;

	ret = OSAL_ST7735_SendData(column_cord_buf, sizeof(column_cord_buf));
	if(ret != DRV_OK)
	{
		return ret;
	}

	/* Address rows */
	cmd = RASET;
	ret = OSAL_ST7735_SendCommand(cmd);
	if(ret != DRV_OK)
	{
		return ret;
	}

	 // offset and coordinate
	static uint8_t row_cord_buf[4];
	row_cord_buf[0] = 0x00;
	row_cord_buf[1] = y1 + defYSTART;
	row_cord_buf[2] = 0x00;
	row_cord_buf[3] = y2 + defYSTART;

	ret = OSAL_ST7735_SendData(row_cord_buf, sizeof(row_cord_buf));
	if(ret != DRV_OK)
	{
		return ret;
	}

	/* Memory write */
	cmd = RAMWR;
	ret = OSAL_ST7735_SendCommand(cmd);
	return ret;
}

DevStatus_t OSAL_DISPLAY_WriteRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color)
{
	/* Take Mutex and set current handle */
	SPI1_MutexTake();
	curr_spi_task = xTaskGetCurrentTaskHandle();

	DevStatus_t ret;

	/* Start communication */
	BSP_ST7735S_CS_LOW();

	ret = OSAL_ST7732_AddressWindow(x1, y1, x2, y2);
	if(ret != DRV_OK)
	{
		/* Stop communication */
		BSP_ST7735S_STOPOperation();
		BSP_ST7735S_CS_HIGH();

		curr_spi_task = NULL;
		SPI1_MutexGive();
		return ret;
	}

	/* Create and fill buffer appropriate color */
	uint32_t total_bytes = (uint32_t)(((abs((int)x2 - (int)x1) + 1) * (abs((int)y2 - (int)y1) + 1)) * 2);

	/* Set color high and low byte */
	uint8_t color_h = (color >> 8);
	uint8_t color_l = (color & 0x00FF);

	for(uint32_t i = 0; i < FILL_BUF_SIZE; i += 2)
	{
		fill_buf[i] = color_h;
		fill_buf[i + 1] = color_l;
	}

	uint32_t bytes_sent = 0;

	while(bytes_sent < total_bytes)
	{
		/* How many bytes send */
		uint32_t bytes_to_send = total_bytes - bytes_sent;

		/* Avoid stack overflow */
		if(bytes_to_send > FILL_BUF_SIZE)
		{
			bytes_to_send = FILL_BUF_SIZE;
		}

		/* Send color bytes */
		ret = OSAL_ST7735_SendData((uint8_t*)fill_buf, bytes_to_send);
		if(ret != DRV_OK)
		{
			/* Stop communication */
			BSP_ST7735S_STOPOperation();
			BSP_ST7735S_CS_HIGH();

			curr_spi_task = NULL;
			SPI1_MutexGive();
			return ret;
		}
		bytes_sent += bytes_to_send;
	}

	/* Wait when SPI send last bit */
	while(BSP_ST7735S_State() != DRV_OK) {
		taskYIELD();
	}

	/* Stop communication */
	BSP_ST7735S_CS_HIGH();

	/* Give Mutex and set current handle NULL */
	while(BSP_ST7735S_GetFlagBUSY() != DRV_OK)
	{
		taskYIELD();
		/* Wait until physical flag BUSY will be reset */
	}

	curr_spi_task = NULL;
	SPI1_MutexGive();

	return ret;
}

/* Draw string in display */
DevStatus_t OSAL_DISPLAY_WriteString(char* str, uint32_t str_len, uint32_t x, uint32_t y, uint16_t font_color, uint16_t bg_color)
{
	/* Take Mutex and set current handle */
	SPI1_MutexTake();
	curr_spi_task = xTaskGetCurrentTaskHandle();

	DevStatus_t ret;

	/* Start communication */
	BSP_ST7735S_CS_LOW();

	/* Start coordinates and offsets */
	uint32_t y_offset = 30;
	uint32_t current_x = 0;

	/* Calculate colors */
	uint8_t font_color_h = (font_color >> 8);
	uint8_t font_color_l = (font_color & 0x00FF);

	uint8_t bg_color_h = (bg_color >> 8);
	uint8_t bg_color_l = (bg_color & 0x00FF);

	uint32_t symbols_sent = 0;

	while(symbols_sent < str_len)
	{
		ret = OSAL_ST7732_AddressWindow(x + current_x, y + y_offset, x + current_x + ( FONT_WIDTH - 1), y + y_offset + (FONT_HEIGHT - 1));
		if(ret != DRV_OK)
		{
			/* Stop communication */
			BSP_ST7735S_STOPOperation();
			BSP_ST7735S_CS_HIGH();

			curr_spi_task = NULL;
			SPI1_MutexGive();
			return ret;
		}

		/* Get bitmap array */
		const uint8_t* bit_map = OSAL_ST7735_GetBitmap(str[symbols_sent++]);

		uint32_t fill_buf_idx = 0;

		/* Fill buffer bs color */
		for(uint32_t i = 0; i < FILL_BUF_SIZE; i += 2)
		{
			fill_buf[i] = bg_color_h;
			fill_buf[i + 1] = bg_color_l;
		}

		/* Fill buffer proper colors */
		for(uint32_t i = 0; i < FONT_HEIGHT; ++i)
		{
			for(uint32_t j = 0; j < FONT_WIDTH; ++j)
			{
				if(((bit_map[j] >> i) & 0x1) == FONT_BIT_SET)
				{
					fill_buf[fill_buf_idx] = font_color_h;
					fill_buf[fill_buf_idx + 1] = font_color_l;
				}
				else
				{
					fill_buf[fill_buf_idx] = bg_color_h;
					fill_buf[fill_buf_idx + 1] = bg_color_l;
				}
				fill_buf_idx += 2;
			}
		}

		/* Send color bytes */
		ret = OSAL_ST7735_SendData((uint8_t*)fill_buf, (FONT_WIDTH * FONT_HEIGHT) * 2);
		if(ret != DRV_OK)
		{
			/* Stop communication */
			BSP_ST7735S_STOPOperation();
			BSP_ST7735S_CS_HIGH();

			curr_spi_task = NULL;
			SPI1_MutexGive();
			return ret;
		}

		current_x += FONT_WIDTH;
	}

	/* Wait when SPI send last bit */
	while(BSP_ST7735S_State() != DRV_OK) {
		taskYIELD();
	}

	/* Stop communication */
	BSP_ST7735S_CS_HIGH();

	/* Give Mutex and set current handle NULL */
	while(BSP_ST7735S_GetFlagBUSY() != DRV_OK)
	{
		taskYIELD();
		/* Wait until physical flag BUSY will be reset */
	}

	curr_spi_task = NULL;
	SPI1_MutexGive();

	return ret;
}
