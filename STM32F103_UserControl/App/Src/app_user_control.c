#include "app_user_control.h"

/* private variables */
static TaskHandle_t motor_target_speed_handle = NULL;
static void motor_target_speed_task (void* pvParameters);
static uint32_t display_text_y_offset = 10;
/* Initialize BSP user speed control */
void APP_USER_CONTROL_Init(ADC_HandleTypeDef* padc1, SPI_HandleTypeDef* pspi1)
{
	APP_STATE_Update_Error_BeforeRTOSStart(ERR_POTENTIOMETER, ERR_ACTIVE);
	APP_STATE_Update_Error_BeforeRTOSStart(ERR_DISPLAY, ERR_ACTIVE);

	/* BSP potentiometer and display initialize */
	for(uint32_t i = 0; i < 3; ++i)
	{
		if(BSP_ST7735S_Init(pspi1) == DRV_OK)
		{
			APP_STATE_Update_Error_BeforeRTOSStart(ERR_DISPLAY, ERR_NOT_ACTIVE);
			HAL_Delay(200);
			break;
		}
		HAL_Delay(100);
	}

	for(uint32_t i = 0; i < 3; ++i)
	{
		if(BSP_POTENTIOMETER_Init(padc1) == DRV_OK)
		{
			APP_STATE_Update_Error_BeforeRTOSStart(ERR_POTENTIOMETER, ERR_NOT_ACTIVE);
			break;
		}
	}

	/* Create local task */
	xTaskCreate(motor_target_speed_task, "motor_target_speed_task", 1024, NULL, 4, &motor_target_speed_handle);
	configASSERT(motor_target_speed_handle != NULL);
}

/* Main task for get user target motor speed and display on screen */
static void motor_target_speed_task(void* pvParameters)
{
	static char display_buf_date[APP_USER_CONTROL_DISPLAY_BUF_LEN];
	static char display_buf_time_speed[APP_USER_CONTROL_DISPLAY_BUF_LEN];

	static char display_buf_motor_speed_state[APP_USER_CONTROL_DISPLAY_BUF_LEN];
	static char display_buf_motor_direction[APP_USER_CONTROL_DISPLAY_BUF_LEN];

	static char display_buf_current_voltage[APP_USER_CONTROL_DISPLAY_BUF_LEN];
	static char display_buf_temterature_logs_state[APP_USER_CONTROL_DISPLAY_BUF_LEN];

	static DevStatus_t res;
	static uint32_t motor_target_speed;
	static GlobalData_t globa_data_state;

	/* Fill display white and boards */
	vTaskDelay(pdMS_TO_TICKS(5));
	res = OSAL_DISPLAY_WriteRect(0, 0, 159, 79, RGB255(255, 255, 255));

	vTaskDelay(pdMS_TO_TICKS(5));
	res = OSAL_DISPLAY_WriteRect(2, 2, 7, 77, RGB255(113, 177, 185));

	vTaskDelay(pdMS_TO_TICKS(5));
	res = OSAL_DISPLAY_WriteRect(152, 2, 157, 77, RGB255(113, 177, 185));

	vTaskDelay(pdMS_TO_TICKS(5));
	res = OSAL_DISPLAY_WriteRect(2, 2, 157, 7, RGB255(113, 177, 185));

	vTaskDelay(pdMS_TO_TICKS(5));
	res = OSAL_DISPLAY_WriteRect(2, 72, 157, 77, RGB255(113, 177, 185));

	if(res == DRV_OK)
	{
		APP_STATE_Update_Error(ERR_DISPLAY, ERR_NOT_ACTIVE);
	}
	else
	{
		APP_STATE_Update_Error(ERR_DISPLAY, ERR_ACTIVE);
	}

	while(1)
	{
		res = OSAL_GET_MOTOR_TARGET_SPEED(&motor_target_speed);
		if(res == DRV_OK)
		{
			APP_STATE_Set_MotorTargetSpeed(motor_target_speed);
			APP_STATE_Update_Error(ERR_POTENTIOMETER, ERR_NOT_ACTIVE);
		}
		else
		{
			APP_STATE_Update_Error(ERR_POTENTIOMETER, ERR_ACTIVE);
		}

		/* Convert and draw current data on screen */
		globa_data_state = APP_STATE_Get_Data();

		snprintf((char*)display_buf_date, sizeof(display_buf_date), "date: %02d.%02d.%04d",
				globa_data_state.date.Date, globa_data_state.date.Month, globa_data_state.date.Year + 2000);

		snprintf((char*)display_buf_time_speed, sizeof(display_buf_time_speed), "time: %02d:%02d:%02d speed: %03lu",
				globa_data_state.time.Hours, globa_data_state.time.Minutes, globa_data_state.time.Seconds, globa_data_state.motor_target_speed);

		snprintf((char*)display_buf_motor_speed_state, sizeof(display_buf_motor_speed_state), "a_speed: %03lu motor_state: %02d",
						globa_data_state.motor_actual_speed, globa_data_state.motor_state);

		snprintf((char*)display_buf_motor_direction, sizeof(display_buf_motor_direction), "motor_dirrection: %d",
										globa_data_state.motor_direction);

		snprintf((char*)display_buf_current_voltage, sizeof(display_buf_current_voltage), "current_mA: %04lu voltage: %02lu",
						globa_data_state.current_mA, globa_data_state.voltage_V);

		snprintf((char*)display_buf_temterature_logs_state, sizeof(display_buf_temterature_logs_state), "temp_C: %03ld logs_state: %02d",
						globa_data_state.temperature_C, globa_data_state.logs_state);

		res = OSAL_DISPLAY_WriteString(display_buf_date, strlen(display_buf_date), FONT_WIDTH * 2, display_text_y_offset, RGB255(0, 0, 0), RGB255(255, 255, 255));

		if(res == DRV_OK)
		{
			APP_STATE_Update_Error(ERR_DISPLAY, ERR_NOT_ACTIVE);
		}
		else
		{
			APP_STATE_Update_Error(ERR_DISPLAY, ERR_ACTIVE);
		}

		vTaskDelay(pdMS_TO_TICKS(1));

		res = OSAL_DISPLAY_WriteString(display_buf_time_speed, strlen(display_buf_time_speed), FONT_WIDTH * 2, display_text_y_offset + FONT_HEIGHT, RGB255(0, 0, 0), RGB255(255, 255, 255));

		if(res == DRV_OK)
		{
			APP_STATE_Update_Error(ERR_DISPLAY, ERR_NOT_ACTIVE);
		}
		else
		{
			APP_STATE_Update_Error(ERR_DISPLAY, ERR_ACTIVE);
		}

		vTaskDelay(pdMS_TO_TICKS(1));

		res = OSAL_DISPLAY_WriteString(display_buf_motor_speed_state, strlen(display_buf_motor_speed_state), FONT_WIDTH * 2, display_text_y_offset + FONT_HEIGHT * 2, RGB255(0, 0, 0), RGB255(255, 255, 255));

		if(res == DRV_OK)
		{
			APP_STATE_Update_Error(ERR_DISPLAY, ERR_NOT_ACTIVE);
		}
		else
		{
			APP_STATE_Update_Error(ERR_DISPLAY, ERR_ACTIVE);
		}

		res = OSAL_DISPLAY_WriteString(display_buf_motor_direction, strlen(display_buf_motor_direction), FONT_WIDTH * 2, display_text_y_offset + FONT_HEIGHT * 3, RGB255(0, 0, 0), RGB255(255, 255, 255));

		if(res == DRV_OK)
		{
			APP_STATE_Update_Error(ERR_DISPLAY, ERR_NOT_ACTIVE);
		}
		else
		{
			APP_STATE_Update_Error(ERR_DISPLAY, ERR_ACTIVE);
		}

		vTaskDelay(pdMS_TO_TICKS(1));

		res = OSAL_DISPLAY_WriteString(display_buf_current_voltage, strlen(display_buf_current_voltage), FONT_WIDTH * 2, display_text_y_offset + FONT_HEIGHT * 4, RGB255(0, 0, 0), RGB255(255, 255, 255));

		if(res == DRV_OK)
		{
			APP_STATE_Update_Error(ERR_DISPLAY, ERR_NOT_ACTIVE);
		}
		else
		{
			APP_STATE_Update_Error(ERR_DISPLAY, ERR_ACTIVE);
		}

		vTaskDelay(pdMS_TO_TICKS(1));

		res = OSAL_DISPLAY_WriteString(display_buf_temterature_logs_state, strlen(display_buf_temterature_logs_state), FONT_WIDTH * 2, display_text_y_offset + FONT_HEIGHT * 5, RGB255(0, 0, 0), RGB255(255, 255, 255));

		if(res == DRV_OK)
		{
			APP_STATE_Update_Error(ERR_DISPLAY, ERR_NOT_ACTIVE);
		}
		else
		{
			APP_STATE_Update_Error(ERR_DISPLAY, ERR_ACTIVE);
		}
	}

	vTaskDelay(pdMS_TO_TICKS(10));
}
