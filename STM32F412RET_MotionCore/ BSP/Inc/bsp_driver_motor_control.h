#ifndef BSP_DRIVER_MOTOR_CONTROL_H
#define BSP_DRIVER_MOTOR_CONTROL_H

#include "stm32f4xx_hal.h"
#include "bsp_common.h"

#include "stdint.h"

#define MOTOR_FORWARD 			0
#define MOTOR_BACKWARD 			1
#define MOTOR_ORDINARY_STOP		2
#define MOTOR_EMERGENCY_STOP	3

#define MOTOR_GPIO_PORT GPIOA

#define MOTOR_GPIO_PIN_FORWARD_EN GPIO_PIN_4
#define MOTOR_GPIO_PIN_BACKWARD_EN GPIO_PIN_7

#define MOTOR_GPIO_PIN_FORWARD GPIO_PIN_8
#define MOTOR_GPIO_PIN_BACKWARD GPIO_PIN_9

DevStatus_t BSP_MOTOR_CONTROL_Init(TIM_HandleTypeDef* ptim1, TIM_HandleTypeDef* ptim2);

int32_t BSP_MOTOR_CONTROL_GetEncoderDifference(void);
DevStatus_t BSP_MOTOR_ChangePWM_State(float pwm_percent, uint8_t FOR_BACK_WARD);

#endif /* BSP_DRIVER_MOTOR_CONTROL_H */
