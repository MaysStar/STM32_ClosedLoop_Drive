#include "app_user_control.h"

/* private variables */
static TaskHandle_t motor_target_speed_handle = NULL;
static void motor_target_speed_task (void* pvParameters);

/* Initialize BSP user speed control */
void APP_USER_CONTROL_Init(ADC_HandleTypeDef* padc1)
{
	/* BSP potentiometer initialize */
	for(uint32_t i = 0; i < 3; ++i)
	{
		if(BSP_POTENTIOMETER_Init(padc1) == DRV_OK)
		{
			break;
		}
	}

	/* Create local task */
	xTaskCreate(motor_target_speed_task, "motor_target_speed_task", 512, NULL, 4, &motor_target_speed_handle);
	configASSERT(motor_target_speed_handle != NULL);
}

/* Main task for get user target motor speed */
static void motor_target_speed_task (void* pvParameters)
{
	static DevStatus_t res;
	static uint32_t motor_target_speed;

	while(1)
	{
		res = OSAL_GET_MOTOR_TARGET_SPEED(&motor_target_speed);
		if(res == DRV_OK)
		{
			APP_STATE_Set_MotorTargetSpeed(motor_target_speed);
		}

		vTaskDelay(pdMS_TO_TICKS(10));
	}
}
