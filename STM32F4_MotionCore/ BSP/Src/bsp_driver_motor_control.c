#include <bsp_driver_motor_control.h>

/* private variables and functions */
static TIM_HandleTypeDef* local_ptim1 = NULL;
static TIM_HandleTypeDef* local_ptim2 = NULL;

/* Type uint32_t for correct transformation */
static  volatile uint32_t encoder_last_cnt = 0;

/* Change motor direction */
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
DevStatus_t BSP_MOTOR_CONTROL_Init(TIM_HandleTypeDef* ptim1, TIM_HandleTypeDef* ptim2)
{
	DevStatus_t ret;
	if((ptim1 != NULL) && (ptim2 != NULL))
	{
		local_ptim1 = ptim1;
		local_ptim2 = ptim2;
		ret = (DevStatus_t)HAL_TIM_PWM_Start(local_ptim1, TIM_CHANNEL_1);
		if(ret != DRV_OK)
		{
			return DRV_INIT_NEEDED;
		}

		ret = (DevStatus_t)HAL_TIM_Encoder_Start(local_ptim2, TIM_CHANNEL_ALL);
		if(ret != DRV_OK)
		{
			return DRV_INIT_NEEDED;
		}
	}
	else
	{
		return DRV_INIT_NEEDED;
	}
	return ret;
}

/* Get difference between previous and current values */
int32_t BSP_MOTOR_CONTROL_GetEncoderDifference(void)
{
	uint32_t encoder_current_cnt = __HAL_TIM_GET_COUNTER(local_ptim2);

	/* transform limits proper from 0 to 2^32 and from 2^32 to 0 */
	int32_t diff_value = (int32_t)(encoder_current_cnt - encoder_last_cnt);

	encoder_last_cnt = encoder_current_cnt;

	return diff_value;
}

/* Set PWM value in percent and motor state */
DevStatus_t BSP_MOTOR_ChangePWM_State(float pwm_percent, uint8_t MOTOR_STATE)
{
	if(local_ptim1 != NULL)
	{
		/* reverse if PID need run otherwise */
		if(pwm_percent < 0 )
		{
			pwm_percent *= -1;
			if(MOTOR_STATE == MOTOR_FORWARD)
			{
				MOTOR_STATE = MOTOR_BACKWARD;
			}
			else if(MOTOR_STATE == MOTOR_BACKWARD)
			{
				MOTOR_STATE = MOTOR_FORWARD;
			}
		}

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
