#include "app_motor_control.h"

/* private variables */
volatile static float curr_speed = 0.0f;

static TaskHandle_t motor_control_handle = NULL;
static void motor_control_task(void* pvParameters);

/* Initialize timer1 PWM and check errors */
void APP_MOTOR_CONTROL_init(TIM_HandleTypeDef* ptim1)
{
	APP_STATE_Update_Error_BeforeRTOSStart(ERR_TIM_PWM, EER_ACTIVE);

	for(uint32_t i = 0; i < 3; ++i)
	{
<<<<<<< HEAD
		if(BSP_TIM_Init(ptim1) == DRV_OK)
=======
		if(BSP_MOTOR_CONTROL_Init(ptim1) == DRV_OK)
>>>>>>> 0e76a80 (refactor: improve BSP architecture layer)
		{
			APP_STATE_Update_Error_BeforeRTOSStart(ERR_TIM_PWM, EER_NOT_ACTIVE);
			break;
		}
	}

	xTaskCreate(motor_control_task, "motor_control_task", 1024, NULL, 2, &motor_control_handle);
	configASSERT(motor_control_handle != NULL);
}

/* Main task for Motor Control using State machine, PID and PWM */
static void motor_control_task(void* pvParameters)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 10;

	while(1)
	{
		xLastWakeTime = xTaskGetTickCount();

		GlobalData_t global_state = APP_STATE_Get_Data();

		switch(global_state.motor_state)
		{
			case MOTOR_RUN:
			{
				curr_speed = 70.0f;
<<<<<<< HEAD
				if(OSAL_TIM1_ChangePWM_State(curr_speed, MOTOR_FORWARD) != DRV_OK)
=======
				if(OSAL_MOTOR_ChangePWM_State(curr_speed, MOTOR_FORWARD) != DRV_OK)
>>>>>>> 0e76a80 (refactor: improve BSP architecture layer)
				{
					APP_STATE_Update_Error(ERR_TIM_PWM, EER_ACTIVE);
				}
				else
				{
					APP_STATE_Update_Error(ERR_TIM_PWM, EER_NOT_ACTIVE);
				}

				APP_STATE_Set_Motor_Values(curr_speed, curr_speed);

				break;
			}
			case MOTOR_STOP:
			{
				curr_speed = 0.0f;
<<<<<<< HEAD
				if(OSAL_TIM1_ChangePWM_State(curr_speed, MOTOR_ORDINARY_STOP) != DRV_OK)
=======
				if(OSAL_MOTOR_ChangePWM_State(curr_speed, MOTOR_ORDINARY_STOP) != DRV_OK)
>>>>>>> 0e76a80 (refactor: improve BSP architecture layer)
				{
					APP_STATE_Update_Error(ERR_TIM_PWM, EER_ACTIVE);
				}
				else
				{
					APP_STATE_Update_Error(ERR_TIM_PWM, EER_NOT_ACTIVE);
				}

				APP_STATE_Set_Motor_State(MOTOR_RECOVER);
				APP_STATE_Set_Motor_Values(curr_speed, curr_speed);
				break;
			}
			case MOTOR_RECOVER:
			{
				for(uint32_t i = 0; i < 70; ++i)
				{
<<<<<<< HEAD
					if(OSAL_TIM1_ChangePWM_State((float)i, MOTOR_FORWARD) != DRV_OK)
=======
					if(OSAL_MOTOR_ChangePWM_State((float)i, MOTOR_FORWARD) != DRV_OK)
>>>>>>> 0e76a80 (refactor: improve BSP architecture layer)
					{
						APP_STATE_Update_Error(ERR_TIM_PWM, EER_ACTIVE);
					}
					else
					{
						APP_STATE_Update_Error(ERR_TIM_PWM, EER_NOT_ACTIVE);
					}
					APP_STATE_Set_Motor_Values(curr_speed, curr_speed);
					vTaskDelay(pdMS_TO_TICKS(20));
				}

				APP_STATE_Set_Motor_Values(curr_speed, curr_speed);
				APP_STATE_Set_Motor_State(MOTOR_RUN);
				break;
			}
			case MOTOR_FAULT_STOP:
			{
				curr_speed = 0.0f;
<<<<<<< HEAD
				if(OSAL_TIM1_ChangePWM_State(curr_speed, MOTOR_EMERGENCY_STOP) != DRV_OK)
=======
				if(OSAL_MOTOR_ChangePWM_State(curr_speed, MOTOR_EMERGENCY_STOP) != DRV_OK)
>>>>>>> 0e76a80 (refactor: improve BSP architecture layer)
				{
					APP_STATE_Update_Error(ERR_TIM_PWM, EER_ACTIVE);
				}
				else
				{
					APP_STATE_Update_Error(ERR_TIM_PWM, EER_NOT_ACTIVE);
				}

				APP_STATE_Set_Motor_Values(curr_speed, curr_speed);
				APP_STATE_Set_Motor_State(MOTOR_RECOVER);
			}
			default:
				break;
		}

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(xFrequency));
	}
}
