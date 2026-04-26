#ifndef APP_USER_COMMUNICATION_H
#define APP_USER_COMMUNICATION_H

/* BSP includes */
#include "bsp_driver_mcu_communication.h"

/* std includes */
#include "stdio.h"
#include "stdint.h"

/* OSAL include */
#include "osal_freertos.h"

/* third party includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* else includes */
#include "app_state.h"

/* CAN id for each message */
#define APP_CAN_MOTOR_STATE_ID	0x01U
#define APP_CAN_CURRENT_mA_ID	0x02U
#define APP_CAN_VOLTAGE_V_ID	0x03U
#define APP_CAN_TEMPERATURE_ID	0x04U
#define APP_CAN_MOTOR_SPEED_ID	0x05U
#define APP_CAN_LOGS_STATE_ID	0x06U

#define APP_CAN_TARGET_SPEED_ID	0x50U

#define APP_CAN_TX_BUF_LEN 		1U
#define APP_CAN_RX_BUF_LEN 		6U

void APP_USER_COMMUNICATION_Init(CAN_HandleTypeDef* pcan);

#endif /* APP_USER_COMMUNICATION_H */
