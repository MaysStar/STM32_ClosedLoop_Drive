#ifndef APP_STATE_H
#define APP_STATE_H

/* std includes */
#include "stdio.h"
#include "stdint.h"

/* OSAL include */
#include "osal_freertos.h"

/* third party includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define EER_ACTIVE			0
#define EER_NOT_ACTIVE		1

#define ERR_OSAL_INIT 		(1 << 0)
#define ERR_POTENTIOMETER	(1 << 1)

typedef struct{
	uint32_t motor_target_speed;
}GlobalData_t;

void APP_STATE_Init(void);
GlobalData_t APP_STATE_Get_Data(void);

void APP_STATE_Set_MotorTargetSpeed(uint32_t motor_target_speed);

#endif /* APP_STATE_H */
