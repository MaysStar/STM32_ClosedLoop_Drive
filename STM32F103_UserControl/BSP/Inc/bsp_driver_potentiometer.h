#ifndef BSP_DRIVER_POTENTIOMETER_H
#define BSP_DRIVER_POTENTIOMETER_H

#include "stm32f1xx_hal.h"
#include "bsp_common.h"

#include "string.h"
#define BSP_POTENTIOMETER_MEAS_RESOLUTION 4095U
#define BSP_POTENTIOMETER_BUFFER_RESOLUTION 32U

DevStatus_t BSP_POTENTIOMETER_Init(ADC_HandleTypeDef* padc1);

DevStatus_t BSP_POTENTIOMETER_GetMotorTargetSpeed(uint32_t* motor_target_speed_percent);
DevStatus_t BSP_POTENTIOMETER_RegisterDmaRxCpltCallbak(void (*callback_fun)(void));

#endif /* BSP_DRIVER_POTENTIOMETER_H */
