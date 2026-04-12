#include "bsp_driver_tim.h"

static TIM_HandleTypeDef* local_ptim1 = NULL;

static void BSP_MOTOR_State(uint8_t MOTOR_STATE)
{
	if(MOTOR_STATE == MOTOR_FORWARD)
	{
		HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN_FORWARD, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN_BACKWARD, GPIO_PIN_RESET);
	}
	else if(MOTOR_STATE == MOTOR_BACKWARD)
	{
		HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN_FORWARD, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN_BACKWARD, GPIO_PIN_SET);
	}
	else if(MOTOR_STATE == MOTOR_ORDINARY_STOP)
	{
		HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN_FORWARD, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN_BACKWARD, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN_FORWARD, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN_BACKWARD, GPIO_PIN_SET);
	}
}

/* Get all timer pointers and appropriate them to local pointers */
DevStatus_t BSP_TIM_Init(TIM_HandleTypeDef* ptim1)
{
	DevStatus_t ret;
	if(ptim1 != NULL)
	{
		local_ptim1 = ptim1;
		ret = (DevStatus_t)HAL_TIM_PWM_Start(local_ptim1, TIM_CHANNEL_1);
	}
	else
	{
		return DRV_INIT_NEEDED;
	}
	return ret;
}

/* Set PWM value in percent and motor state */
DevStatus_t BSP_TIM1_ChangePWM_State(float pwm_percent, uint8_t MOTOR_STATE)
{
	if(local_ptim1 != NULL)
	{
		BSP_MOTOR_State(MOTOR_STATE);
		uint16_t compare_value = (uint16_t)((pwm_percent * (float)__HAL_TIM_GET_AUTORELOAD(local_ptim1)) / 100.0f);
		__HAL_TIM_SET_COMPARE(local_ptim1, TIM_CHANNEL_1, compare_value);
	}
	else
	{
		return DRV_INIT_NEEDED;
	}
	return DRV_OK;
}
