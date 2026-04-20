#ifndef BSP_DRIVER_ST7735S_H
#define BSP_DRIVER_ST7735S_H

#include "stm32f1xx_hal.h"
#include "bsp_common.h"

/*
 * Schematic ST7735S
 *
 * BLK -> 3.3V
 * CS  -> A10
 * DC  -> B7
 * RES -> B6
 * SDA -> A7
 * SCL -> A5
 *
 */

/* ST7735S pins definition */
#define BSP_ST7735S_CS_PORT		GPIOA
#define BSP_ST7735S_CS_PIN		GPIO_PIN_10

#define BSP_ST7735S_DC_PORT		GPIOB
#define BSP_ST7735S_DC_PIN		GPIO_PIN_7

#define BSP_ST7735S_RES_PORT	GPIOB
#define BSP_ST7735S_RES_PIN		GPIO_PIN_6

#define BSP_ST7735S_SDA_PORT	GPIOA
#define BSP_ST7735S_SDA_PIN		GPIO_PIN_7

#define BSP_ST7735S_SCL_PORT	GPIOA
#define BSP_ST7735S_SCL_PIN		GPIO_PIN_5

#define BSP_ST7735S_BLK_PORT	GPIOB
#define BSP_ST7735S_BLK_PIN		GPIO_PIN_11

/* St7735S pins state definition */

#define BSP_ST7735S_CS_HIGH()	HAL_GPIO_WritePin(BSP_ST7735S_CS_PORT, BSP_ST7735S_CS_PIN, GPIO_PIN_SET)
#define BSP_ST7735S_CS_LOW()	HAL_GPIO_WritePin(BSP_ST7735S_CS_PORT, BSP_ST7735S_CS_PIN, GPIO_PIN_RESET)

#define BSP_ST7735S_DC_HIGH()	HAL_GPIO_WritePin(BSP_ST7735S_DC_PORT, BSP_ST7735S_DC_PIN, GPIO_PIN_SET)
#define BSP_ST7735S_DC_LOW()	HAL_GPIO_WritePin(BSP_ST7735S_DC_PORT, BSP_ST7735S_DC_PIN, GPIO_PIN_RESET)

#define BSP_ST7735S_RES_HIGH()	HAL_GPIO_WritePin(BSP_ST7735S_RES_PORT, BSP_ST7735S_RES_PIN, GPIO_PIN_SET)
#define BSP_ST7735S_RES_LOW()	HAL_GPIO_WritePin(BSP_ST7735S_RES_PORT, BSP_ST7735S_RES_PIN, GPIO_PIN_RESET)

#define BSP_ST7735S_BLK_HIGH()	HAL_GPIO_WritePin(BSP_ST7735S_BLK_PORT, BSP_ST7735S_BLK_PIN, GPIO_PIN_SET)

/* from https://github.com/bersch/ST7735S/tree/master */
/* this may differ from the default 80x160 */
#define defWIDTH   160
#define defHEIGHT  80
#define defXSTART  0
#define defYSTART  24

#define RGB255(r, g, b) (uint16_t)(((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r & 0xF8) >> 3)

/* from https://github.com/bersch/ST7735S/tree/master */
typedef enum {
    NOP       = 0x00,
	SWRESET   = 0x01, /* Software Reset */
	RDDID     = 0x04, /* Read Display ID */
	RDDST     = 0x09, /* Read Display Status */
	RDDPM     = 0x0a, /* Read Display Power Mode */
	RDDMADCTL = 0x0b, /* Read Display MADCTL */
	RDDCOLMOD = 0x0c, /* Read Display Pixel Format */
	RDDIM     = 0x0d, /* Read Display Image Mode */
	RDDSM     = 0x0e, /* Read Display Signal Mode */
	RDDSDR    = 0x0f, /* Read Display Self-Diagnostic Result */
	SLPIN     = 0x10, /* Sleep In */
	SLPOUT    = 0x11, /* Sleep Out */
	PTLON     = 0x12, /* Partial Display Mode On */
	NORON     = 0x13, /* Normal Display Mode On */
	INVOFF    = 0x20, /* Display Inversion Off */
	INVON     = 0x21, /* Display Inversion On */
	GAMSET    = 0x26, /* Gamma Set */
	DISPOFF   = 0x28, /* Display Off */
	DISPON    = 0x29, /* Display On */
	CASET     = 0x2a, /* Column Address Set */
	RASET     = 0x2b, /* Row Address Set */
	RAMWR     = 0x2c, /* Memory Write */
	RGBSET    = 0x2d, /* Color Setting 4k, 65k, 262k */
	RAMRD     = 0x2e, /* Memory Read */
	PTLAR     = 0x30, /* Partial Area */
	SCRLAR    = 0x33, /* Scroll Area Set */
	TEOFF     = 0x34, /* Tearing Effect Line OFF */
	TEON      = 0x35, /* Tearing Effect Line ON */
	MADCTL    = 0x36, /* Memory Data Access Control */
	VSCSAD    = 0x37, /* Vertical Scroll Start Address of RAM */
	IDMOFF    = 0x38, /* Idle Mode Off */
	IDMON     = 0x39, /* Idle Mode On */
	COLMOD    = 0x3a, /* Interface Pixel Format */
	RDID1     = 0xda, /* Read ID1 Value */
	RDID2     = 0xdb, /* Read ID2 Value */
	RDID3     = 0xdc, /* Read ID3 Value */
	FRMCTR1   = 0xb1, /* Frame Rate Control in normal mode, full colors */
	FRMCTR2   = 0xb2, /* Frame Rate Control in idle mode, 8 colors */
	FRMCTR3   = 0xb3, /* Frame Rate Control in partial mode, full colors */
	INVCTR    = 0xb4, /* Display Inversion Control */
	PWCTR1    = 0xc0, /* Power Control 1 */
	PWCTR2    = 0xc1, /* Power Control 2 */
	PWCTR3    = 0xc2, /* Power Control 3 in normal mode, full colors */
	PWCTR4    = 0xc3, /* Power Control 4 in idle mode 8colors */
	PWCTR5    = 0xc4, /* Power Control 5 in partial mode, full colors */
	VMCTR1    = 0xc5, /* VCOM Control 1 */
	VMOFCTR   = 0xc7, /* VCOM Offset Control */
	WRID2     = 0xd1, /* Write ID2 Value */
	WRID3     = 0xd2, /* Write ID3 Value */
	NVFCTR1   = 0xd9, /* NVM Control Status */
	NVFCTR2   = 0xde, /* NVM Read Command */
	NVFCTR3   = 0xdf, /* NVM Write Command */
	GMCTRP1   = 0xe0, /* Gamma '+'Polarity Correction Characteristics Setting */
	GMCTRN1   = 0xe1, /* Gamma '-'Polarity Correction Characteristics Setting */
	GCV       = 0xfc, /* Gate Pump Clock Frequency Variable */
} ST7735S_Command;

void delay_us(uint32_t us);

DevStatus_t BSP_ST7735S_Init(SPI_HandleTypeDef* pspi1);
DevStatus_t BSP_ST7735S_STOPOperation(void);
DevStatus_t BSP_ST7735S_State(void);
DevStatus_t BSP_ST7735S_GetFlagBUSY(void);

DevStatus_t BSP_ST7735_SendCommand(ST7735S_Command cmd);
DevStatus_t BSP_ST7735_SendData(uint8_t* data_buf, uint32_t data_len);
DevStatus_t BSP_ST7735_SetPixelColor(uint16_t color);

DevStatus_t BSP_ST7735S_RegisterDmaTxCpltCallback(void (*callback_fun)(void));

#endif /* BSP_DRIVER_ST7735S_H */
