#ifndef BSP_DRIVER_I2C_H
#define BSP_DRIVER_I2C_H

#include "stm32f4xx_hal.h"

typedef struct
{
	float power_W;
	float voltage_V;
}PowerVoltage_t;

#define BSP_I2C_INA219_ADDR 0x40

#define INA219_REG_CONFIG 0x00
#define INA219_REG_SHUNTVOLT 0x01
#define INA219_REG_BUSVOLT 0x02
#define INA219_REG_POWER 0x03
#define INA219_REG_CURRENT 0x04
#define INA219_REG_CALIBRATION 0x05

void BSP_I2C_Init(I2C_HandleTypeDef* phi2c1);

/* Read and get electricity measurements */
void BSP_I2C_ReadCurrent(void);
float BSP_I2C_GetCurrent(void);

void BSP_I2C_ReadPower(void);
float BSP_I2C_GetPower(void);

void BSP_I2C_ReadVoltage(void);
float BSP_I2C_GetVoltage(void);

/* Register callback */
void BSP_I2C1_RegisterRxCpltCallbak(void (*callback_fun)(void));

#endif /* BSP_DRIVER_I2C_H */
