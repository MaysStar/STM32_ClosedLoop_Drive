#include "bsp_driver_st7735s.h"

/* private variables and functions */
static SPI_HandleTypeDef* local_pspi1 = NULL;

static bsp_callback_fun SPI1_DMA_TxCpltCallbak;

static ST7735S_Command last_cmd;

/* from https://github.com/bersch/ST7735S/tree/master */
/* columns: 1 = # of params, 2 = command, 3 .. = params */
static  const uint8_t init_cmd[] = {
    1, SWRESET, /* software reset */
    1,  SLPOUT, /* sleep out, turn off sleep mode */
	1, DISPOFF,  /*  output from frame mem disabled */
    4, FRMCTR1, 0x00, 0b111111, 0b111111, /* frame frequency normal mode (highest frame rate in normal mode) */
    4, FRMCTR2, 0b1111, 0x01, 0x01, /* frame frequency idle mode */
    7, FRMCTR3, 0x05, 0x3c, 0x3c, 0x05, 0x3c, 0x3c,  /* frame freq partial mode: 1-3 dot inv, 4-6 col inv */
    2,  INVCTR, 0x03, /* display inversion control: 3-bit 0=dot, 1=col */

    4,  PWCTR1, 0b11111100, 0x08, 0b10, /* power control */
    2,  PWCTR2, 0xc0,
    3,  PWCTR3, 0x0d, 0x00,
    3,  PWCTR4, 0x8d, 0x2a,
    3,  PWCTR5, 0x8d, 0xee, /* partial */

	/* display brightness and gamma */
    2,     GCV, 0b11011000, /* auto gate pump freq, max power save */
    2, NVFCTR1, 0b01000000, /* automatic adjust gate pumping clock for saving power consumption */
	2,  VMCTR1, 0b001111,  /* VCOM voltage setting */
    2, VMOFCTR, 0b10000, /* ligthness of black color 0-0x1f */
    2,  GAMSET, 0x08, /* gamma 1, 2, 4, 8 */

    2,  MADCTL, 0b01100000, /* row oder, col order, row colum xchange, vert refr order, rgb/bgr, hor refr order, 0, 0 */
    2,  COLMOD, 0x05, /* 3=12bit, 5=16-bit, 6=18-bit  pixel color mode */
    17, GMCTRP1,0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2c,
                0x29, 0x25, 0x2b, 0x39, 0x00, 0x01, 0x03, 0x10,
    17, GMCTRN1,0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2c,
                0x2e, 0x2e, 0x37, 0x3f, 0x00, 0x00, 0x02, 0x10,
    5,   CASET, 0, 0, 0, defHEIGHT-1,
    5,   RASET, 0, 0, 0, defWIDTH-1,
    1,   INVOFF, /* display inversion on/off */
    1,  IDMOFF, /* idle mode off */
    1,   NORON,  /* normal display mode on */
    1,  DISPON,  /* recover from display off, output from frame mem enabled */
};

/* Delay in us using processor register */
void delay_us(uint32_t us)
{
	uint32_t start_tick = DWT->CYCCNT;

	uint32_t delay_tick = us * (SystemCoreClock / 1000000U);

	while((DWT->CYCCNT - start_tick) < delay_tick)
	{
		__NOP();
	}
}

/* Send all initial command */
static DevStatus_t BSP_ST7735S_Init_Command(void)
{
	DevStatus_t ret;
	uint8_t args = 0;

	/* Start communication */
	BSP_ST7735S_CS_LOW();

	for(uint32_t i = 0; i < sizeof(init_cmd); i += args + 1)
	{
		args = init_cmd[i];
		uint8_t command = init_cmd[i + 1];

		BSP_ST7735S_DC_LOW();
		ret = (DevStatus_t)HAL_SPI_Transmit(local_pspi1, &command, 1, 100);
		if(ret != DRV_OK)
		{
			return DRV_INIT_NEEDED;
		}

		BSP_ST7735S_DC_HIGH();

		if(args > 1)
		{
			ret = (DevStatus_t)HAL_SPI_Transmit(local_pspi1, &init_cmd[i + 2], (args - 1), 100);
			if(ret != DRV_OK)
			{
				return DRV_INIT_NEEDED;
			}
		}

		if (command == SWRESET) HAL_Delay(120);
		if (command == SLPOUT)  HAL_Delay(120);
	}

	/* Finish communication */
	BSP_ST7735S_CS_HIGH();

	return ret;
}

/* Appropriate local SPI pointer and send initialize commands for display */
DevStatus_t BSP_ST7735S_Init(SPI_HandleTypeDef* pspi1)
{
	DevStatus_t ret;
	if(pspi1 != NULL)
	{
		local_pspi1 = pspi1;

		/* hard reset */
		BSP_ST7735S_BLK_HIGH();
		BSP_ST7735S_RES_HIGH();
		BSP_ST7735S_CS_LOW();

		delay_us(20);
		BSP_ST7735S_RES_LOW();

		delay_us(20);
		BSP_ST7735S_RES_HIGH();

		HAL_Delay(120);

		ret = BSP_ST7735S_Init_Command();

		return ret;
	}
	return DRV_INIT_NEEDED;
}

DevStatus_t BSP_ST7735S_STOPOperation(void)
{
	return (DevStatus_t)HAL_SPI_DMAStop(local_pspi1);;
}

DevStatus_t BSP_ST7735S_State(void)
{
	uint32_t state = HAL_SPI_GetState(local_pspi1);

	if(state == HAL_SPI_STATE_RESET) return DRV_INIT_NEEDED;

	return (DevStatus_t)(state - 1);
}

DevStatus_t BSP_ST7735S_GetFlagBUSY(void)
{
	return (DevStatus_t) __HAL_SPI_GET_FLAG(local_pspi1, SPI_FLAG_BSY);
}

/* Commands and data write */
DevStatus_t BSP_ST7735_SendCommand(ST7735S_Command cmd)
{
	last_cmd = cmd;

	BSP_ST7735S_DC_LOW();
	DevStatus_t ret = (DevStatus_t)HAL_SPI_Transmit(local_pspi1, (uint8_t*)&last_cmd, 1, 50);
	return ret;
}

DevStatus_t BSP_ST7735_SendData(uint8_t* data_buf, uint32_t data_len)
{
	if(data_buf != NULL)
	{
		BSP_ST7735S_DC_HIGH();
		DevStatus_t ret = (DevStatus_t)HAL_SPI_Transmit_DMA(local_pspi1, data_buf, data_len);
		return ret;
	}

	return DRV_ERROR;
}

/* Color fill in one pixel and only when chose window address */
DevStatus_t BSP_ST7735_SetPixelColor(uint16_t color)
{
	DevStatus_t ret;

	uint8_t color_u8[2];
	color_u8[0] = (uint8_t)(color >> 8);
	color_u8[1] = (uint8_t)(color & 0xFF);

	ret = BSP_ST7735_SendData(color_u8, 2);
	return ret;
}

/* Register callback for SPI1 DMA transmit complete */
DevStatus_t BSP_ST7735S_RegisterDmaTxCpltCallback(void (*callback_fun)(void))
{
	if(callback_fun != NULL)
	{
		SPI1_DMA_TxCpltCallbak = callback_fun;
		return DRV_OK;
	}

	return DRV_INIT_NEEDED;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI1)
	{
		if(SPI1_DMA_TxCpltCallbak != NULL)
		{
			SPI1_DMA_TxCpltCallbak();
		}
	}
}
