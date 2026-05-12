#ifndef BSP_DRIVER_INA219_H
#define BSP_DRIVER_INA219_H

#include <bsp_common.h>
#include "stm32f4xx_hal.h"

#define BSP_INA219_ADDR 0x40

#define INA219_REG_CONFIG 0x00
#define INA219_REG_SHUNTVOLT 0x01
#define INA219_REG_BUSVOLT 0x02
#define INA219_REG_POWER 0x03
#define INA219_REG_CURRENT 0x04
#define INA219_REG_CALIBRATION 0x05

DevStatus_t BSP_INA219_Init(I2C_HandleTypeDef* phi2c1);

/* Read and get electricity measurements */
DevStatus_t BSP_INA219_ReadCurrent(void);
float BSP_INA219_GetCurrent(void);

DevStatus_t BSP_INA219_ReadPower(void);
float BSP_INA219_GetPower(void);

DevStatus_t BSP_INA219_ReadVoltage(void);
float BSP_INA219_GetVoltage(void);

/* Register callback */
DevStatus_t BSP_INA219_RegisterRxCpltCallbak(void (*callback_fun)(void));

#endif /* BSP_DRIVER_INA219_H */
