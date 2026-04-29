#include "app_motor_control.h"

/* private variables */
static volatile float actual_speed_percent = 0.0f;
static volatile float current_recover_pwm = 0.0f;

static PID_Controller_t PID_Controller = {0};

static TaskHandle_t motor_control_handle = NULL;
static void motor_control_task(void* pvParameters);

/* Initialize PID controller */
static void APP_MOTOR_CONTROL_PID_Controller_init(PID_Controller_t* PID_Controller)
{
	if(PID_Controller != NULL)
	{
		PID_Controller->Kp = 0.5f;
		PID_Controller->Ki = 0.0f;
		PID_Controller->Kd = 0.005f;

		PID_Controller->min_result = -100.0f;
		PID_Controller->max_result = 100.0f;

		PID_Controller->prev_error = 0.0f;
		PID_Controller->prev_measurement = 0.0f;

		PID_Controller->proportional = 0.0f;
		PID_Controller->integrator = 0.0f;
		PID_Controller->differentiator = 0.0f;

		/* 10 * dt, dt = 50ms tau = 500ms */
		PID_Controller->tau = 0.5f;

		PID_Controller->out = 0.0f;
	}
}
/* Fuction which control the speed of the motors speed, using Proportional Integration Differential
    u(t) = Kp * e(t) + Ki * iteg(e(t)dt) + Kd * de(t) / dt
*/
static float APP_MOTOR_CONTROL_PID_Update(PID_Controller_t* PID_controler, float measurement, float setpoint)
{
	/* Calculate error */
	float curr_error = setpoint - measurement;

	float dt_f = 0.05f;

	/* Proportional */
	PID_controler->proportional = PID_controler->Kp * curr_error;

	/* Integrator Trapezoidal Rule */
	PID_controler->integrator = PID_controler->integrator + 0.5f * PID_controler->Ki * (PID_controler->prev_error + curr_error) * dt_f;

	/* Anti-windup clamp */
	if (PID_controler->integrator > PID_controler->max_result)
	{
		PID_controler->integrator = PID_controler->max_result;
	}
	if (PID_controler->integrator < PID_controler->min_result)
	{
		PID_controler->integrator = PID_controler->min_result;
	}

	if(PID_controler->integrator > PID_controler->max_result) PID_controler->integrator = PID_controler->max_result;
	if(PID_controler->integrator < PID_controler->min_result) PID_controler->integrator = PID_controler->min_result;

	/* Band-Limited Derivative */
	PID_controler->differentiator =  (2.0f * PID_controler->Kd * (measurement - PID_controler->prev_measurement)
								  +  (2.0f * PID_controler->tau - dt_f) * PID_controler->differentiator)
								  /  (2.0f * PID_controler->tau + dt_f);

	/* Set prev measurement */
	PID_controler->prev_error = curr_error;
	PID_controler->prev_measurement = measurement;

	/* Calculate output */
	PID_controler->out = PID_controler->proportional + PID_controler->integrator - PID_controler->differentiator;

	if(PID_controler->out > PID_controler->max_result)
	{
		PID_controler->out = PID_controler->max_result;
	}
	else if(PID_controler->out < PID_controler->min_result)
	{
		PID_controler->out = PID_controler->min_result;
	}

	return PID_controler->out;
}

/* Initialize timer1 PWM and check errors */
void APP_MOTOR_CONTROL_init(TIM_HandleTypeDef* ptim1, TIM_HandleTypeDef* ptim2)
{
	APP_STATE_Update_Error_BeforeRTOSStart(ERR_TIM_PWM, ERR_ACTIVE);

	for(uint32_t i = 0; i < 3; ++i)
	{
		if(BSP_MOTOR_CONTROL_Init(ptim1, ptim2) == DRV_OK)
		{
			APP_STATE_Update_Error_BeforeRTOSStart(ERR_TIM_PWM, ERR_NOT_ACTIVE);
			break;
		}
	}

	/* Initialize PID control system */
	APP_MOTOR_CONTROL_PID_Controller_init(&PID_Controller);

	xTaskCreate(motor_control_task, "motor_control_task", 1024, NULL, 2, &motor_control_handle);
	configASSERT(motor_control_handle != NULL);
}

/* Main task for Motor Control using State machine, PID and PWM */
static void motor_control_task(void* pvParameters)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 50;

	// Initialize the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		GlobalData_t global_state = APP_STATE_Get_Data();

		/* Start condition */
		if((global_state.motor_state == MOTOR_STOP) &&
		   (global_state.target_motor_speed != 0))
		{
			current_recover_pwm = 0.0f;
			APP_STATE_Set_Motor_State(MOTOR_RECOVER);
			global_state.motor_state = MOTOR_RECOVER;
		}

		float encoder_delta = (float)OSAL_MOTOR_CONTROL_GetEncoderDifference();
		encoder_delta = (encoder_delta >= 0) ? encoder_delta : (-encoder_delta);

		/* Calculate speed in percent according to motor max speed */
		// encoder_dt = 50.
		actual_speed_percent = ((encoder_delta * 100) / APP_MOTOR_CONTROL_MAX_ENCODER_VAL_P50MS);

		APP_STATE_Set_Motor_ActualSpeed(actual_speed_percent);

		/* Target PWM value from PID control system */
		float pid_pwm_percent = APP_MOTOR_CONTROL_PID_Update(&PID_Controller, actual_speed_percent, global_state.target_motor_speed);

		switch(global_state.motor_state)
		{
			case MOTOR_RUN:
			{
				if(OSAL_MOTOR_ChangePWM_State(pid_pwm_percent, global_state.motor_direction) != DRV_OK)
				{
					APP_STATE_Update_Error(ERR_TIM_PWM, ERR_ACTIVE);
				}
				else
				{
					APP_STATE_Update_Error(ERR_TIM_PWM, ERR_NOT_ACTIVE);
				}

				break;
			}
			case MOTOR_STOP:
			{
				actual_speed_percent = 0.0f;
				if(OSAL_MOTOR_ChangePWM_State(actual_speed_percent, MOTOR_ORDINARY_STOP) != DRV_OK)
				{
					APP_STATE_Update_Error(ERR_TIM_PWM, ERR_ACTIVE);
				}
				else
				{
					APP_STATE_Update_Error(ERR_TIM_PWM, ERR_NOT_ACTIVE);
				}
				break;
			}
			case MOTOR_RECOVER:
			{
				/* Increase value step by step */
				current_recover_pwm += 5.0f;

				if(current_recover_pwm >= pid_pwm_percent)
				{
					/* acceleration is finish */
					current_recover_pwm = pid_pwm_percent;
					APP_STATE_Set_Motor_State(MOTOR_RUN);
				}

				if(OSAL_MOTOR_ChangePWM_State(current_recover_pwm, global_state.motor_direction) != DRV_OK)
				{
					APP_STATE_Update_Error(ERR_TIM_PWM, ERR_ACTIVE);
				}
				else
				{
					APP_STATE_Update_Error(ERR_TIM_PWM, ERR_NOT_ACTIVE);
				}

				break;
			}
			case MOTOR_FAULT_STOP:
			{
				actual_speed_percent = 0.0f;
				if(OSAL_MOTOR_ChangePWM_State(actual_speed_percent, MOTOR_EMERGENCY_STOP) != DRV_OK)
				{
					APP_STATE_Update_Error(ERR_TIM_PWM, ERR_ACTIVE);
				}
				else
				{
					APP_STATE_Update_Error(ERR_TIM_PWM, ERR_NOT_ACTIVE);
				}

				/* Start recover speed */
				current_recover_pwm = 0.0f;
				APP_STATE_Set_Motor_State(MOTOR_RECOVER);
				break;
			}
			default:
				break;
		}

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(xFrequency));
	}
}
