#ifndef BSP_DRIVER_DS18B20_H_
#define BSP_DRIVER_DS18B20_H_

#include <stdio.h>
#include "stm32f4xx_hal.h"

void BSP_1WireDS18B20(UART_HandleTypeDef* huart1);

#endif /* BSP_DRIVER_DS18B20_H_ */
